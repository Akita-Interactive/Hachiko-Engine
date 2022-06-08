#include "core/hepch.h"

#include "core/ErrorHandler.h"

#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleProgram.h"
#include "ModuleCamera.h"
#include "ModuleDebugDraw.h"
#include "ModuleSceneManager.h"
#include "ModuleEditor.h"
#include "ModuleUserInterface.h"
#include "ModuleNavigation.h"
#include "ModuleInput.h"

#include "components/ComponentCamera.h"

Hachiko::ModuleRender::ModuleRender() = default;

Hachiko::ModuleRender::~ModuleRender() = default;

bool Hachiko::ModuleRender::Init()
{
    HE_LOG("INITIALIZING MODULE: RENDER");

    CreateContext();

    RetrieveGpuInfo();
    RetrieveLibVersions();

    SetGLOptions();

    GenerateDeferredQuad();
    GenerateFrameBuffer();

#ifdef _DEBUG
    glEnable(GL_DEBUG_OUTPUT); // Enable output callback
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(&ErrorHandler::HandleOpenGlError, nullptr); // Set the callback
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true); // Filter notifications
#endif

    fps_log = std::vector<float>(n_bins);
    ms_log = std::vector<float>(n_bins);

    GenerateParticlesBuffers();

    return true;
}

float2 Hachiko::ModuleRender::GetFrameBufferSize() const
{
    return float2(fb_width, fb_height);
}

void Hachiko::ModuleRender::GenerateFrameBuffer()
{
    fb_width = 800;
    fb_height = 600;

    glGenFramebuffers(1, &frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

    glGenTextures(1, &fb_texture);
    glBindTexture(GL_TEXTURE_2D, fb_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fb_width, fb_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    //Depth and stencil buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb_texture, 0);
    glGenRenderbuffers(1, &depth_stencil_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_stencil_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fb_width, fb_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_stencil_buffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        HE_LOG("Error creating frame buffer");
    }

    if (draw_deferred)
    {
        // Generate G-Buffer and associated textures:
        g_buffer.Generate();
    }
   
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Hachiko::ModuleRender::ResizeFrameBuffer(const int width, const int height) const
{
    // Frame buffer texture:
    glBindTexture(GL_TEXTURE_2D, fb_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    
    // Handle resizing the textures of g-buffer:
    g_buffer.Resize(width, height);
    
    // Unbind:
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, depth_stencil_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Hachiko::ModuleRender::ManageResolution(const ComponentCamera* camera)
{
    unsigned res_x, res_y;
    camera->GetResolution(res_x, res_y);
    if (res_x != fb_height || res_y != fb_width)
    {
        ResizeFrameBuffer(res_x, res_y);
        glViewport(0, 0, res_x, res_y);
        fb_height = res_y;
        fb_width = res_x;
    }
}

void Hachiko::ModuleRender::CreateContext()
{
    HE_LOG("Creating Renderer context");

    context = SDL_GL_CreateContext(App->window->GetWindow());
    GLenum err = glewInit();

    int value = 0;
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &value);
}

void Hachiko::ModuleRender::SetGLOptions()
{
    glEnable(GL_DEPTH_TEST); // Enable depth test
    glEnable(GL_CULL_FACE); // Enable cull backward faces
    glFrontFace(GL_CCW); // Front faces will be counter clockwise
    glEnable(GL_STENCIL_TEST); // Enable stencil test
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // Only replace stencil value if stencil and depth tests pass
}

UpdateStatus Hachiko::ModuleRender::PreUpdate(const float delta)
{
    render_list.PreUpdate();

    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus Hachiko::ModuleRender::Update(const float delta)
{    
    ComponentCamera* camera = App->camera->GetRenderingCamera();
    Scene* active_scene = App->scene_manager->GetActiveScene();

#ifdef PLAY_BUILD
    int width = 0; 
    int height = 0;
    App->window->GetWindowSize(width, height);
    
    glViewport(0, 0, width, height);
    
    App->camera->OnResize(width, height);
#endif

    ManageResolution(camera);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
      
    if (active_scene == nullptr)
    {
        return UpdateStatus::UPDATE_CONTINUE;
    }    

    active_scene->RebuildBatching();
    App->program->UpdateCamera(camera);

#ifdef PLAY_BUILD
    ComponentCamera* culling = App->camera->GetRenderingCamera();
#else
    ComponentCamera* culling = active_scene->GetCullingCamera();
#endif

    const ComponentDirLight* dir_light = nullptr;
    if (!active_scene->dir_lights.empty())
        dir_light = active_scene->dir_lights[0];

    App->program->UpdateLights(dir_light, active_scene->point_lights, active_scene->spot_lights);

    Draw(App->scene_manager->GetActiveScene(), camera, culling);
    
    if (draw_navmesh)
    {
        App->navigation->DebugDraw();
    }

    GLint polygonMode[2];
    glGetIntegerv(GL_POLYGON_MODE, polygonMode);
    if (polygonMode[0] == GL_LINE)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        App->ui->DrawUI(active_scene);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        App->ui->DrawUI(active_scene);
    }

    // If in play build, blit frame_buffer to the default frame buffer and render to the whole 
    // screen, if not, bind default frame buffer:
#ifndef PLAY_BUILD
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#else
    glBindFramebuffer(GL_READ_FRAMEBUFFER, frame_buffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, fb_width, fb_height, 0, 0, fb_width, fb_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
#endif

    return UpdateStatus::UPDATE_CONTINUE;
}

void Hachiko::ModuleRender::Draw(Scene* scene, ComponentCamera* camera, 
    ComponentCamera* culling)
{
    OPTICK_CATEGORY("Draw", Optick::Category::Rendering);

    BatchManager* batch_manager = scene->GetBatchManager();
    
    render_list.Update(culling, scene->GetQuadtree());
    
    if (draw_deferred)
    {
        DrawDeferred(scene, camera, batch_manager);
    }
    else
    {
        DrawPreForwardPass(scene, camera);
        // TODO: Forward rendering still has that weird stuttering bug, fix this.
        DrawForward(batch_manager);
    }
}

void Hachiko::ModuleRender::DrawDeferred(Scene* scene, ComponentCamera* camera, 
    BatchManager* batch_manager)
{
    Program* program = nullptr;

    if (App->input->GetKey(SDL_SCANCODE_F5) == KeyState::KEY_DOWN)
    {
        deferred_mode = (deferred_mode + 1) % 7;
    }

    if (App->input->GetKey(SDL_SCANCODE_F4) == KeyState::KEY_DOWN)
    {
        render_forward_pass = !render_forward_pass;
    }

    // ----------------------------- GEOMETRY PASS ----------------------------

    g_buffer.BindForDrawing();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Disable blending for deferred rendering as the meshes with transparent
    // materials are gonna be rendered with forward rendering after the
    // deferred lighting pass:
    glDisable(GL_BLEND);

    // Clear Opaque Batches List:
    batch_manager->ClearOpaqueBatchesDrawList();

    // Send mesh renderers with opaque materials to batch manager draw list:
    for (const RenderTarget& target : render_list.GetOpaqueTargets())
    {
        batch_manager->AddDrawComponent(target.mesh_renderer);
        // target.mesh_renderer->Draw(camera, program);
    }

    // Draw collected meshes with geometry pass rogram:
    program = App->program->GetDeferredGeometryProgram();
    program->Activate();
    batch_manager->DrawOpaqueBatches(program);
    Program::Deactivate();

    // ------------------------------ LIGHT PASS ------------------------------

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use Deferred rendering lighting pass program:
    program = App->program->GetDeferredLightingProgram();
    program->Activate();

    // Bind all g-buffer textures:
    g_buffer.BindTextures();

    // Bind deferred rendering mode. This can be configured from the editor,
    // and shader sets the fragment color according to this mode:
    program->BindUniformInts("mode", 1, &deferred_mode);

    // Render the final texture from deferred rendering on a quad that is,
    // 1x1 on NDC:
    RenderDeferredQuad();

    Program::Deactivate();

    // Blit g_buffer depth buffer to frame_buffer to be used for forward
    // rendering pass:
    g_buffer.BlitDepth(frame_buffer, fb_width, fb_height);

    // Enable blending for the next passes:
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ------------------------------ PRE FORWARD -----------------------------
    
    DrawPreForwardPass(scene, camera);

    // ----------------------------- FORWARD PASS -----------------------------

    // Clear Transparent Batches:
    batch_manager->ClearTransparentBatchesDrawList();

    // If forward pass is disabled on the settings, return:
    if (!render_forward_pass)
    {
        return;
    }

    // Get transparent meshes:
    const std::vector<RenderTarget>& transparent_targets = render_list.GetTransparentTargets();

    if (transparent_targets.size() <= 0)
    {
        return;
    }
    
    // Get the targets that has transparent materials. These targets will be
    // rendered with regular forward rendering pass:
    for (const RenderTarget& target : transparent_targets)
    {
        batch_manager->AddDrawComponent(target.mesh_renderer);
    }

    // Forward rendering pass for transparent game objects:
    program = App->program->GetForwardProgram();
    program->Activate();
    batch_manager->DrawTransparentBatches(program);
    Program::Deactivate();
}

void Hachiko::ModuleRender::DrawForward(BatchManager* batch_manager) 
{
    Program* program = App->program->GetForwardProgram();
    program->Activate();

    batch_manager->ClearOpaqueBatchesDrawList();
    batch_manager->ClearTransparentBatchesDrawList();

    // Add opaque targets:
    for (RenderTarget& target : render_list.GetOpaqueTargets())
    {
        batch_manager->AddDrawComponent(target.mesh_renderer);    
    }

    // Add transparent targets:
    for (RenderTarget& target : render_list.GetTransparentTargets())
    {
        batch_manager->AddDrawComponent(target.mesh_renderer);  
    }

    batch_manager->DrawOpaqueBatches(program);
    batch_manager->DrawTransparentBatches(program);

    Program::Deactivate();
}

void Hachiko::ModuleRender::DrawPreForwardPass(Scene* scene, ComponentCamera* camera) const
{
    // This method is to draw things that needs to be drawn before forward pass 
    // of deferred rendering or before forward rendering.

    if (draw_skybox)
    {
        scene->GetSkybox()->Draw(camera);
    }

    // Draw debug draw stuff:
    ModuleDebugDraw::Draw(camera->GetViewMatrix(), camera->GetProjectionMatrix(), fb_height, fb_width);

    const auto& scene_particles = scene->GetSceneParticles();
    if (!scene_particles.empty())
    {
        Program* particle_program = App->program->GetParticleProgram();
        particle_program->Activate();
        for (auto particle : scene_particles)
        {
            particle->Draw(camera, particle_program);
        }
        Program::Deactivate();
    }

    //GameObject* selected_go = App->editor->GetSelectedGameObject();
    /*if (outline_selection && selected_go)
    {
        glStencilFunc(GL_NOTEQUAL, 1, 0XFF);
        glStencilMask(0X00);
        glDisable(GL_DEPTH_TEST);

        Program* outline_program = App->program->GetStencilProgram();
        outline_program->Activate();
        selected_go->DrawStencil(camera, outline_program);
        Program::Deactivate();

        glStencilMask(0XFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glEnable(GL_DEPTH_TEST);
    }*/
}

void Hachiko::ModuleRender::SetRenderMode(bool is_deferred) 
{
    if (is_deferred == draw_deferred)
    {
        return;
    }

    draw_deferred = is_deferred;

    if (!draw_deferred)
    {
        g_buffer.Free();
    }
    else
    {
        g_buffer.Generate();
    }
}

UpdateStatus Hachiko::ModuleRender::PostUpdate(const float delta)
{
    SDL_GL_SwapWindow(App->window->GetWindow());
    AddFrame(delta);

    return UpdateStatus::UPDATE_CONTINUE;
}

void GLOptionCheck(GLenum option, bool enable)
{
    if (enable)
    {
        glEnable(option);
    }
    else
    {
        glDisable(option);
    }
}

void Hachiko::ModuleRender::OptionsMenu()
{
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Draw Options");
    ImGui::Checkbox("Debug Draw", &App->debug_draw->debug_draw);
    ImGui::Checkbox("Quadtree", &App->debug_draw->draw_quadtree);
    ImGui::Checkbox("Skybox", &draw_skybox);
    ImGui::Checkbox("Navmesh", &draw_navmesh);

    ImGui::NewLine();
    ImGui::Text("Rendering Mode");
    ImGui::Separator();
    ImGui::TextWrapped("Select whether the engine should use Deferred or Forward rendering.");
    ImGui::PushID("RendererMode");

    if (ImGui::RadioButton("Deferred", draw_deferred == true))
    {
        SetRenderMode(true);
    }

    if (ImGui::RadioButton("Forward", draw_deferred == false))
    {
        SetRenderMode(false);
    }
    ImGui::PopID();

    if (draw_deferred)
    {
        DeferredOptions();
    }

    if (!draw_skybox)
    {
        ImGuiUtils::CompactColorPicker("Background Color", App->editor->scene_background.ptr());
    }
}

void Hachiko::ModuleRender::DeferredOptions() 
{
    ImGui::NewLine();

    ImGui::Text("Deferred Rendering Options");
    ImGui::Separator();

    ImGui::PushID("Deferred Options");

    if (ImGui::RadioButton("Lighting Pass", deferred_mode == 0))
    {
        deferred_mode = 0;
    }

    if (ImGui::RadioButton("Diffuse", deferred_mode == 1))
    {
        deferred_mode = 1;
    }

    if (ImGui::RadioButton("Specular", deferred_mode == 2))
    {
        deferred_mode = 2;
    }
    
    if (ImGui::RadioButton("Smoothness", deferred_mode == 3))
    {
        deferred_mode = 3;
    }
    
    if (ImGui::RadioButton("Normal", deferred_mode == 4))
    {
        deferred_mode = 4;
    }

    if (ImGui::RadioButton("Position", deferred_mode == 5))
    {
        deferred_mode = 5;
    }

    if (ImGui::RadioButton("Emissive", deferred_mode == 6))
    {
        deferred_mode = 6;
    }

    ImGui::Checkbox("Forward Rendering Pass", &render_forward_pass);

    ImGui::NewLine();

    ImGui::PopID();
}

void Hachiko::ModuleRender::PerformanceMenu()
{

    const float vram_free_mb = gpu.vram_free / 1024.0f;
    const float vram_usage_mb = gpu.vram_budget_mb - vram_free_mb;
    ImGui::Text("VRAM Budget: %.1f Mb", gpu.vram_budget_mb);
    ImGui::Text("Vram Usage:  %.1f Mb", vram_usage_mb);
    ImGui::Text("Vram Avaliable:  %.1f Mb", vram_free_mb);
    ImGui::Separator();
    FpsGraph();
}

void Hachiko::ModuleRender::FpsGraph() const
{
    ImGui::Text("Fps: %.1f", current_fps);

    char title[25];
    sprintf_s(title, 25, "Framerate %.1f", current_fps);
    ImGui::PlotHistogram("##framerate", &fps_log[0], static_cast<int>(fps_log.size()), 0, title, 0.0f, 1000.f, ImVec2(310, 100));
    sprintf_s(title, 25, "Milliseconds %0.1f", current_ms);
    ImGui::PlotHistogram("##milliseconds", &ms_log[0], static_cast<int>(ms_log.size()), 0, title, 0.0f, 20.0f, ImVec2(310, 100));
}

void Hachiko::ModuleRender::AddFrame(const float delta)
{
    static constexpr float update_frequency_seconds = 0.5f;
    static int filled_bins = 0;
    static int frames = 0;
    static float time = 0;

    ++frames;
    time += delta;

    if (time >= update_frequency_seconds)
    {
        if (filled_bins == n_bins)
        {
            for (int i = 0; i < n_bins - 1; ++i)
            {
                fps_log[i] = fps_log[i + 1];
                ms_log[i] = ms_log[i + 1];
            }
        }
        else
        {
            ++filled_bins;
        }
        fps_log[filled_bins - 1] = static_cast<float>(frames) / time;
        current_fps = fps_log[filled_bins - 1];
        ms_log[filled_bins - 1] = time * 1000.0f / static_cast<float>(frames);
        current_ms = ms_log[filled_bins - 1];
        time = 0;
        frames = 0;
    }
}

void Hachiko::ModuleRender::SetOpenGLAttributes() const 
{
    // Enable hardware acceleration:
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    // Set desired major version of OpenGL:
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    // Set desired minor version of OpenGL:
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    // Set OpenGL to compatibility mode (we can use core mode as well): 
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    // Enable double buffer usage:
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // Set buffer size to 32 bits, Red 8, Green 8, Blue 8, Alpha 8:
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    // Set depth component size (bits):
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // TODO: Ask Carlos why this can't be 32.
    // Set red color component size (bits):
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    // Set green color component size (bits):
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    // Set blue color component size (bits):
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    // Set alpha color component size (bits):
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    // Set stencil component size (bits):
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    // Enable OpenGL context debug:
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
}

void Hachiko::ModuleRender::RetrieveLibVersions()
{
    HE_LOG("GPU Vendor: %s", glGetString(GL_VENDOR));
    HE_LOG("Renderer: %s", glGetString(GL_RENDERER));
    HE_LOG("Using Glew %s", glewGetString(GLEW_VERSION));
    HE_LOG("OpenGL version supported %s", glGetString(GL_VERSION));
    HE_LOG("GLSL: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void Hachiko::ModuleRender::RetrieveGpuInfo()
{
    gpu.name = (unsigned char*)glGetString(GL_RENDERER);
    gpu.brand = (unsigned char*)glGetString(GL_VENDOR);

            GLint count;
    glGetIntegerv(GL_NUM_EXTENSIONS, &count);
    for (GLint i = 0; i < count; ++i)
    {
        const char* extension = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i));
        if (!strcmp(extension, "GL_NVX_gpu_memory_info"))
        {    glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &gpu.vram_budget_mb);
            glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &gpu.vram_free);
        }
    }

    gpu.vram_budget_mb /= 1024;
}


void Hachiko::ModuleRender::GenerateParticlesBuffers()
{
    float positions[] = {
        0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // top right
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f,  0.0f, 0.0f, 1.0f // top left
    };

    unsigned int indices[] = {2, 1, 0, 0, 3, 2};

    glGenVertexArrays(1, &particle_vao);
    glBindVertexArray(particle_vao);

    glGenBuffers(1, &particle_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, particle_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));

    glGenBuffers(1, &particle_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, particle_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindVertexArray(0);
}

void Hachiko::ModuleRender::GenerateDeferredQuad() 
{
    constexpr const float vertices[] = {
        1.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top right
        1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
        -1.0f, 1.0f,  0.0f, 0.0f, 1.0f // top left
    };
    constexpr unsigned int indices[] = {2, 1, 0, 0, 3, 2};

    glGenVertexArrays(1, &deferred_quad_vao);
    glGenBuffers(1, &deferred_quad_vbo);
    glGenBuffers(1, &deferred_quad_ebo);

    glBindVertexArray(deferred_quad_vao);

    glBindBuffer(GL_ARRAY_BUFFER, deferred_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, deferred_quad_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Vertices:
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinates:
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Hachiko::ModuleRender::RenderDeferredQuad() const 
{
    glBindVertexArray(deferred_quad_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Hachiko::ModuleRender::FreeDeferredQuad() 
{
    glBindVertexArray(deferred_quad_vao);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDeleteBuffers(1, &deferred_quad_ebo);
    glDeleteBuffers(1, &deferred_quad_vbo);
    glDeleteVertexArrays(1, &deferred_quad_vao);
    glBindVertexArray(0);
}

bool Hachiko::ModuleRender::CleanUp()
{
    FreeDeferredQuad();

    glDeleteTextures(1, &fb_texture);
    glDeleteBuffers(1, &depth_stencil_buffer);
    glDeleteBuffers(1, &frame_buffer);

    SDL_GL_DeleteContext(context);

    return true;
}