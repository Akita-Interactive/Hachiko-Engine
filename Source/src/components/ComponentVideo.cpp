#include "core/hepch.h"

#include "ComponentVideo.h"

#include "modules/ModuleWindow.h"
#include "modules/ModuleProgram.h"
#include "modules/ModuleResources.h"

#include "ComponentCamera.h"
#include "ComponentTransform.h"

#include "resources/ResourceVideo.h"

Hachiko::ComponentVideo::ComponentVideo(GameObject* container)
: Component(Component::Type::VIDEO, container)
{}

Hachiko::ComponentVideo::~ComponentVideo()
{
    FreeVideoReader();
    glDeleteTextures(1, &frame_texture);
    DetachFromScene();
    RemoveVideoResource();
}

void Hachiko::ComponentVideo::Start()
{
    if (!in_scene)
    {
        PublishIntoScene();
    }

    glGenTextures(1, &frame_texture);
    OpenVideo();
}

void Hachiko::ComponentVideo::Restart()
{
    int result = Reset();
    if (result >= 0)
    {
        state = VideoState::PLAYING;
    }
}

void Hachiko::ComponentVideo::Update()
{
#ifndef PLAY_BUILD
    if (!in_scene)
    {
        Start();
    }
#endif

    time += EngineTimer::delta_time;

    if (time < (1.0 / fps))
    {
        able_to_capture = false;
    }
    else
    {
        able_to_capture = true;
        time = 0.0f;
    }
}

void Hachiko::ComponentVideo::DrawGui()
{
    ImGui::PushID(this);
    if (ImGuiUtils::CollapsingHeader(game_object, this, "Video"))
    {
        if (video == nullptr)
        {
            AddVideo();
        }
        else if (ImGui::Button("Remove video", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
        {
            Stop();
            RemoveVideoResource();
        }

        Widgets::Checkbox("Loop", &loop);
        Widgets::Checkbox("Projected", &projected);
        Widgets::Checkbox("Flip", &flip_vertical);
        Widgets::DragFloatConfig cfg;
        cfg.min = 1.0f;
        cfg.speed = 0.1f;
        cfg.format = "%.2f";
        Widgets::DragFloat("Fps", fps, &cfg);
    }
    ImGui::PopID();

#ifndef PLAY_BUILD
    if (video != nullptr)
    {
        DisplayControls();
    }
#endif
}

void Hachiko::ComponentVideo::Draw(ComponentCamera* camera, Program* /*program*/)
{
    if (state == VideoState::STOPPED || video == nullptr)
    {
        return;
    }
    
    const Hachiko::Program* program = App->program->GetVideoProgram();
    program->Activate();
    glDepthMask(GL_FALSE);

    if (IsPlaying() && able_to_capture)
    {
        ReadNextFrame();
    }

    BindFrameToGLTexture();
    SetProjectionMatrices(camera, program);

    App->renderer->RenderFullScreenQuad();

    // Clear
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDepthMask(GL_TRUE);
    Hachiko::Program::Deactivate();
}

void Hachiko::ComponentVideo::Save(YAML::Node& node) const
{
    node.SetTag("video");
    node[VIDEO_ID] = video != nullptr ? video->GetID() : 0;
    node[VIDEO_PROJECTED] = projected;
    node[VIDEO_LOOP] = loop;
    node[VIDEO_FLIP] = flip_vertical;
    node[VIDEO_FPS] = fps;
}

void Hachiko::ComponentVideo::Load(const YAML::Node& node)
{
    if (node[VIDEO_ID].IsDefined() && node[VIDEO_ID].as<UID>() != 0)
    {
        video = static_cast<ResourceVideo*>(App->resources->GetResource(Resource::Type::VIDEO, node[VIDEO_ID].as<UID>()));
    }

    projected = node[VIDEO_PROJECTED].IsDefined() ? node[VIDEO_PROJECTED].as<bool>() : projected;
    loop = node[VIDEO_LOOP].IsDefined() ? node[VIDEO_LOOP].as<bool>() : loop;
    flip_vertical = node[VIDEO_FLIP].IsDefined() ? node[VIDEO_FLIP].as<bool>() : flip_vertical;
    fps = node[VIDEO_FPS].IsDefined() ? node[VIDEO_FPS].as<float>() : fps;
}

void Hachiko::ComponentVideo::Play()
{
    state = VideoState::PLAYING;
}

void Hachiko::ComponentVideo::Pause()
{
    state = VideoState::PAUSED;
}

void Hachiko::ComponentVideo::Stop()
{
    Reset();
    state = VideoState::STOPPED;
}

void Hachiko::ComponentVideo::BindFrameToGLTexture() const
{
    // allocate memory and set texture data
    glBindTexture(GL_TEXTURE_2D, frame_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Set texture clamping method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame_width, frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame_data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Hachiko::ComponentVideo::ReadNextFrame()
{
    int response = -1;
    int error = 0;

    while (error >= 0)
    {
        error = av_read_frame(format_ctx, av_packet);

        if (av_packet->stream_index != video_stream_index)
        {
            av_packet_unref(av_packet);
            continue;
        }

        //SEEK to frame 0 -> Restart the video timestamp
        if (error == AVERROR_EOF)
        {
            if (loop)
            {
                Reset();
            }
            else
            {
                Stop();
            }

            av_packet_unref(av_packet);
            break;
        }

        response = avcodec_send_packet(video_codec_ctx, av_packet);
        if (response < 0)
        {
            HE_LOG("Failed to decode packet");
            return;
        }

        response = avcodec_receive_frame(video_codec_ctx, av_frame);
        if (response == AVERROR(EAGAIN) || response == AVERROR_EOF)
        {
            av_packet_unref(av_packet);
            continue;
        }

        if (response < 0)
        {
            HE_LOG("Failed to decode frame");
            return;
        }

        av_packet_unref(av_packet);
        break;
    }

    video_frame_time = static_cast<float>(av_frame->pts * time_base.num) / static_cast<float>(time_base.den);
    if (video_frame_time == 0)
    {
        time = 0;
    }

    if (!scaler_ctx)
    {
        // Set SwScaler - Scale frame size + Pixel converter to RGB
        scaler_ctx = sws_getContext(frame_width, frame_height, video_codec_ctx->pix_fmt, frame_width, frame_height, AV_PIX_FMT_BGR32, SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);

        if (!scaler_ctx)
        {
            HE_LOG("Couldn't initialise SwScaler.");
            return;
        }
    }

    // Transform pixel format to RGB and send the data to the framebuffer
    // By default the image is flipped, so consider the negate
    if (!flip_vertical)
    {
        FlipImage();
    }

    uint8_t* dest[4] = {frame_data, nullptr, nullptr, nullptr};
    int lin_size[4] = {frame_width * 4, 0, 0, 0};
    sws_scale(scaler_ctx, av_frame->data, av_frame->linesize, 0, frame_height, dest, lin_size);
}

bool Hachiko::ComponentVideo::IsPlaying() const
{
    return state == VideoState::PLAYING;
}

void Hachiko::ComponentVideo::SetProjectionMatrices(const ComponentCamera* camera, const Program* program)
{
    ComponentTransform* transform = GetGameObject()->GetComponent<ComponentTransform>();
    program->BindUniformInt("ignore_camera", projected ? 0 : 1);
    program->BindUniformFloat4x4("model", transform->GetGlobalMatrix().ptr());
    program->BindUniformFloat4x4("view", &camera->GetViewMatrix()[0][0]);
    program->BindUniformFloat4x4("proj", &camera->GetProjectionMatrix()[0][0]);
}

void Hachiko::ComponentVideo::AddVideo()
{
    const char* title = "Select video";
    std::string video_path;

    if (ImGui::Button("Add video", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
    {
        ImGuiFileDialog::Instance()->OpenDialog(title, "Select Video", ".mp4", "./assets/video/", 1, nullptr,
           ImGuiFileDialogFlags_DontShowHiddenFiles | ImGuiFileDialogFlags_DisableCreateDirectoryButton | 
           ImGuiFileDialogFlags_HideColumnType | ImGuiFileDialogFlags_HideColumnDate);
    }

    if (ImGuiFileDialog::Instance()->Display(title))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            video_path = ImGuiFileDialog::Instance()->GetFilePathName();
        }

        ImGuiFileDialog::Instance()->Close();
    }

    video_path.append(META_EXTENSION);

    if (!std::filesystem::exists(video_path.c_str()))
    {
        return;
    }

    YAML::Node video_node = YAML::LoadFile(video_path);
    ResourceVideo* res = static_cast<ResourceVideo*>(App->resources->GetResource(Resource::Type::VIDEO, video_node[RESOURCES][0][RESOURCE_ID].as<UID>()));

    if (res != nullptr)
    {
        video = res;
        OpenVideo();
    }
}

void Hachiko::ComponentVideo::RemoveVideoResource()
{
    App->resources->ReleaseResource(video);
    video = nullptr;
}

void Hachiko::ComponentVideo::FreeVideoReader()
{
    // Close libAV context -  free allocated memory
    sws_freeContext(scaler_ctx);
    scaler_ctx = nullptr;
    avformat_close_input(&format_ctx);
    avformat_free_context(format_ctx);
    avcodec_free_context(&video_codec_ctx);
    avcodec_free_context(&audio_codec_ctx);
    av_frame_free(&av_frame);
    av_packet_free(&av_packet);

    // Release frame data buffer
    RELEASE(frame_data);
}

void Hachiko::ComponentVideo::OpenVideo()
{
    if (video == nullptr)
    {
        return;
    }

    // Open video file
    format_ctx = avformat_alloc_context();
    if (!format_ctx)
    {
        HE_LOG("Couldn't allocate AVFormatContext.");
        return;
    }
    if (avformat_open_input(&format_ctx, video->GetVideoPath().string().c_str(), nullptr, nullptr) != 0)
    {
        HE_LOG("Couldn't open video file.");
        return;
    }

    // DECODING VIDEO
    // Find a valid video stream in the file
    video_stream_index = -1;
    video_stream_index = av_find_best_stream(format_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (video_stream_index < 0)
    {
        HE_LOG("Couldn't find valid video stream inside file.");
        return;
    }

    // Find an appropiate video decoder
    const AVCodecParameters* videoCodecParams = format_ctx->streams[video_stream_index]->codecpar;
    const AVCodec* videoDecoder = avcodec_find_decoder(videoCodecParams->codec_id);
    if (!videoDecoder)
    {
        HE_LOG("Couldn't find valid video decoder.");
        return;
    }

    // Set up a video codec context for the decoder
    video_codec_ctx = avcodec_alloc_context3(videoDecoder);
    if (!video_codec_ctx)
    {
        HE_LOG("Couldn't allocate AVCodecContext.");
        return;
    }

    if (avcodec_parameters_to_context(video_codec_ctx, videoCodecParams) < 0)
    {
        HE_LOG("Couldn't initialise AVCodecContext.");
        return;
    }

    if (avcodec_open2(video_codec_ctx, videoDecoder, nullptr) < 0)
    {
        HE_LOG("Couldn't open video codec.");
        return;
    }

    // Set video parameters and Allocate frame buffer
    frame_width = videoCodecParams->width;
    frame_height = videoCodecParams->height;
    time_base = format_ctx->streams[video_stream_index]->time_base;
    frame_data = new uint8_t[frame_width * frame_height * 4];
    CleanFrameBuffer();

    // Allocate memory for packets and frames
    av_packet = av_packet_alloc();
    if (!av_packet)
    {
        HE_LOG("Couldn't allocate AVPacket.");
        return;
    }

    av_frame = av_frame_alloc();
    if (!av_frame)
    {
        HE_LOG("Couldn't allocate AVFrame.");
        return;
    }
}

int Hachiko::ComponentVideo::Reset()
{
    avio_seek(format_ctx->pb, 0, SEEK_SET);
    return av_seek_frame(format_ctx, video_stream_index, -1, 0);
}

void Hachiko::ComponentVideo::FlipImage()
{
    av_frame->data[0] += av_frame->linesize[0] * (video_codec_ctx->height - 1);
    av_frame->linesize[0] *= -1;
    av_frame->data[1] += av_frame->linesize[1] * (video_codec_ctx->height / 2 - 1);
    av_frame->linesize[1] *= -1;
    av_frame->data[2] += av_frame->linesize[2] * (video_codec_ctx->height / 2 - 1);
    av_frame->linesize[2] *= -1;
}

void Hachiko::ComponentVideo::PublishIntoScene()
{
    auto scene = game_object->scene_owner;

    if (scene)
    {
        scene->AddParticleComponent(this);
        in_scene = true;
    }
}

void Hachiko::ComponentVideo::DetachFromScene()
{
    auto scene = game_object->scene_owner;

    if (scene)
    {
        scene->RemoveParticleComponent(GetID());
        in_scene = false;
    }
}
void Hachiko::ComponentVideo::DisplayControls()
{
    const auto& pos = App->editor->GetSceneWindow()->GetViewportPosition();
    const auto& viewport_size = App->editor->GetSceneWindow()->GetViewportSize();

    ImGui::SetNextWindowPos(ImVec2(pos.x + ImGui::GetStyle().FramePadding.x * 3, pos.y));
    ImGui::SetNextWindowBgAlpha(1.0f);

    const std::string go_label = StringUtils::Concat("GameObject: ", GetGameObject()->GetName().c_str());
    const float min_size_x = std::min(ImGui::CalcTextSize(go_label.c_str()).x + ImGui::GetFontSize() * 2, viewport_size.x / 4.0f);
    ImGui::SetNextWindowSize(ImVec2(std::max(min_size_x, 200.0f), 0));
    ImGui::Begin("Video", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);

    ImGui::TextWrapped(go_label.c_str());
    ImGui::Text("Video fps: %.1f", fps);

    ImGui::Separator();

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
    ImGui::BeginTable("##video", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_NoSavedSettings);
    ImGui::TableSetupColumn("##video", ImGuiTableColumnFlags_WidthStretch, 0.33f);
    ImGui::TableSetupColumn("##video", ImGuiTableColumnFlags_WidthStretch, 0.34f);
    ImGui::TableSetupColumn("##video", ImGuiTableColumnFlags_WidthStretch, 0.33f);
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    if (state == VideoState::PLAYING)
    {
        if (ImGui::Button("Pause", ImVec2(-1, 0)))
        {
            Pause();
        }
    }
    else
    {
        if (ImGui::Button("Play", ImVec2(-1, 0)))
        {
            Play();
        }
    }
    ImGui::TableNextColumn();
    if (ImGui::Button("Restart", ImVec2(-1, 0)))
    {
        Restart();
    }
    ImGui::TableNextColumn();
    if (ImGui::Button("Stop", ImVec2(-1, 0)))
    {
        Stop();
    }
    ImGui::EndTable();
    ImGui::PopStyleVar();
    ImGui::End();
}

void Hachiko::ComponentVideo::CleanFrameBuffer()
{
    memset(frame_data, 0, frame_width * frame_height * 4);
}