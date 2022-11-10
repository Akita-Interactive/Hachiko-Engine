#include "core/hepch.h"
#include "ComponentImage.h"
#include "ComponentTransform2D.h"
#include "ComponentButton.h"

#include "modules/ModuleProgram.h"
#include "Application.h"
#include "modules/ModuleUserInterface.h"
#include "modules/ModuleResources.h"
#include "modules/ModuleTexture.h"
#include "resources/ResourceTexture.h"

#include "core/rendering/Program.h"
#include "modules/ModuleEvent.h"
#include "modules/ModuleCamera.h"
#include "ComponentCamera.h"
#include "utils/ComponentUtility.h"

Hachiko::ComponentImage::ComponentImage(GameObject* container) :
    Component(Type::IMAGE, container)
{
}

void Hachiko::ComponentImage::DrawGui()
{
    constexpr bool is_hover_image = true;

    ImGui::PushID(this);

    if (ImGuiUtils::CollapsingHeader(this, "Image"))
    {
        Widgets::Checkbox("Fill Window", &fill_window);

        ImGui::TextWrapped("Normal image");

        const std::string title = "Select image";
        if (ImGui::Button(title.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
        {
            const char* filters = "Image files{.png,.tif,.jpg,.tga}";
            ImGuiFileDialog::Instance()->OpenDialog(title,
                                                    title,
                                                    filters,
                                                    ASSETS_FOLDER_TEXTURE,
                                                    1,
                                                    nullptr,
                                                    ImGuiFileDialogFlags_DontShowHiddenFiles | ImGuiFileDialogFlags_DisableCreateDirectoryButton | ImGuiFileDialogFlags_HideColumnType
                                                    | ImGuiFileDialogFlags_HideColumnDate);
        }
        // ImGui::SameLine();
        if (image != nullptr)
        {
            if (ImGui::Button("Remove image", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
            {
                App->resources->ReleaseResource(image);
                image = nullptr;
            }
            else
            {
                Widgets::Label("Selected image", image->path);
            }
        }
        else
        {
            ImGui::TextWrapped("No image selected");
        }

        if (ImGuiFileDialog::Instance()->Display(title.c_str()))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string font_path = ImGuiFileDialog::Instance()->GetFilePathName();
                font_path.append(META_EXTENSION);
                YAML::Node font_node = YAML::LoadFile(font_path);
                LoadImageResource(font_node[RESOURCES][0][RESOURCE_ID].as<UID>(), !is_hover_image);
                App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
            }

            ImGuiFileDialog::Instance()->Close();
        }

        Widgets::ColorEdit4("Image color", color);

        if (image != nullptr)
        {
            if (Widgets::Checkbox("Is tiled", &is_tiled))
            {
                if (!is_tiled)
                {
                    animation_index = {0.0f, 0.0f};
                    factor = float2::one;
                }
                else
                {
                    factor = float2(1.0f / tiles.x, 1.0f / tiles.y);
                }
            }
            if (is_tiled)
            {
                Widgets::Checkbox("Randomize initial frame", &randomize_initial_frame);
                Widgets::DragFloatConfig cfg;
                cfg.format = "%.f";
                cfg.min = 1.0f;
                cfg.speed = 1.0f;
                if (DragFloat("Frames per second", frames_per_second, &cfg))
                {
                    time_per_frame = 1.0f / frames_per_second;
                }

                if (DragFloat("X tiles", tiles.x, &cfg))
                {
                    factor.x = 1.0f / tiles.x;
                }
                if (DragFloat("Y tiles", tiles.y, &cfg))
                {
                    factor.y = 1.0f / tiles.y;
                }
            }
        }

        ImGui::TextWrapped("Hover image");

        const std::string hover_title = "Select hover image";
        if (ImGui::Button(hover_title.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
        {
            const char* filters = "Image files{.png,.tif,.jpg,.tga}";
            ImGuiFileDialog::Instance()->OpenDialog(hover_title,
                                                    hover_title,
                                                    filters,
                                                    ASSETS_FOLDER_TEXTURE,
                                                    1,
                                                    nullptr,
                                                    ImGuiFileDialogFlags_DontShowHiddenFiles | ImGuiFileDialogFlags_DisableCreateDirectoryButton | ImGuiFileDialogFlags_HideColumnType
                                                    | ImGuiFileDialogFlags_HideColumnDate);
        }

        if (hover_image != nullptr)
        {
            if (ImGui::Button("Remove hover image",ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
            {
                App->resources->ReleaseResource(hover_image);
                hover_image = nullptr;
            }
            else
            {
                Widgets::Label("Selected hover image: ", hover_image->path);
            }
        }
        else
        {
            ImGui::TextWrapped("No hover image selected");
        }

        if (ImGuiFileDialog::Instance()->Display(hover_title.c_str()))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string font_path = ImGuiFileDialog::Instance()->GetFilePathName();
                font_path.append(META_EXTENSION);
                YAML::Node font_node = YAML::LoadFile(font_path);
                LoadImageResource(font_node[RESOURCES][0][RESOURCE_ID].as<UID>(), is_hover_image);
                App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
            }

            ImGuiFileDialog::Instance()->Close();
        }

        Widgets::ColorEdit4("Hover color", hover_color);
    }
    ImGui::PopID();
}

void Hachiko::ComponentImage::Draw(ComponentTransform2D* transform, Program* program) const
{
    OPTICK_CATEGORY("Draw", Optick::Category::Rendering);

    // Activate program & bind square:
    program->Activate();
    App->ui->BindSquare();

    program->BindUniformFloat4x4("model", transform->GetGlobalScaledTransform().ptr());
    const ResourceTexture* img_to_draw = image;
    const float4* render_color = &color;

    ComponentButton* button = game_object->GetComponent<ComponentButton>();

    if (button && button->IsHovered())
    {
        img_to_draw = hover_image;
        render_color = &hover_color;
    }

    program->BindUniformBool("diffuse_flag", img_to_draw != nullptr);
    program->BindUniformFloat4("img_color", render_color->ptr());
    ModuleTexture::Bind(img_to_draw ? img_to_draw->GetImageId() : 0, static_cast<int>(Hachiko::ModuleProgram::TextureSlots::DIFFUSE));

    program->BindUniformFloat2("factor", factor.ptr());
    program->BindUniformFloat2("animation_index", animation_index.ptr());

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Unbind square & deactivate program:
    App->ui->UnbindSquare();
    Program::Deactivate();
}

void Hachiko::ComponentImage::Start()
{
    if (image != nullptr && is_tiled && randomize_initial_frame)
    {
        animation_index = float2(
            RandomUtil::RandomIntBetween(0, tiles.x - 1),
            RandomUtil::RandomIntBetween(0, tiles.y - 1)
            );
    }
}

void Hachiko::ComponentImage::Update()
{
    if (fill_window)
    {
        UpdateSize();
    }
    if (image != nullptr && is_tiled)
    {
        elapse += EngineTimer::delta_time;

        while (elapse >= time_per_frame)
        {
            elapse -= time_per_frame;

            if (animation_index.x < tiles.x - 1)
            {
                animation_index.x += 1.0f;
                break;
            }
            else if (animation_index.y < tiles.y - 1)
            {
                animation_index.x = 0.0f;
                animation_index.y += 1.0f;
                break;
            }

            animation_index = {0.0f, 0.0f};
        }
    }
}

void Hachiko::ComponentImage::Save(YAML::Node& node) const
{
    node.SetTag("image");
    node[IMAGE_IMAGE_ID] = image ? image->GetID() : 0;
    node[IMAGE_HOVER_IMAGE_ID] = hover_image ? hover_image->GetID() : 0;
    node[IMAGE_COLOR] = color;
    node[IMAGE_HOVER_COLOR] = hover_color;
    node[IMAGE_TILED] = is_tiled;
    node[IMAGE_RANDOMIZE_INITIAL_FRAME] = randomize_initial_frame;
    node[IMAGE_X_TILES] = tiles.x;
    node[IMAGE_Y_TILES] = tiles.y;
    node[IMAGE_TILES_PER_SEC] = frames_per_second;
    node[IMAGE_FILL_WINDOW] = fill_window;
}

void Hachiko::ComponentImage::Load(const YAML::Node& node)
{
    constexpr bool is_hover_image = true;
    LoadImageResource(node[IMAGE_IMAGE_ID].as<UID>(), !is_hover_image);
    LoadImageResource(node[IMAGE_HOVER_IMAGE_ID].as<UID>(), is_hover_image);
    color = node[IMAGE_COLOR].as<float4>();
    hover_color = node[IMAGE_HOVER_COLOR].as<float4>();
    fill_window = node[IMAGE_FILL_WINDOW].IsDefined() ? node[IMAGE_FILL_WINDOW].as<bool>() : false;
    is_tiled = node[IMAGE_TILED].IsDefined() ? node[IMAGE_TILED].as<bool>() : false;
    randomize_initial_frame = node[IMAGE_RANDOMIZE_INITIAL_FRAME].IsDefined() ? node[IMAGE_RANDOMIZE_INITIAL_FRAME].as<bool>() : false;
    tiles.x = node[IMAGE_X_TILES].IsDefined() ? node[IMAGE_X_TILES].as<int>() : 1;
    tiles.y = node[IMAGE_Y_TILES].IsDefined() ? node[IMAGE_Y_TILES].as<int>() : 1;

    frames_per_second = node[IMAGE_TILES_PER_SEC].IsDefined() ? node[IMAGE_TILES_PER_SEC].as<int>() : 1;
    factor = float2(1.0f / tiles.x, 1.0f / tiles.y);
    time_per_frame = 1.0f / frames_per_second;
}

void Hachiko::ComponentImage::CollectResources(const YAML::Node& node, std::map<Resource::Type, std::set<UID>>& resources)
{
    // Collect default image resource:
    ComponentUtility::CollectResource(
        Resource::Type::TEXTURE,
        node[IMAGE_IMAGE_ID],
        resources);
    // Collect hover image resource:
    ComponentUtility::CollectResource(
        Resource::Type::TEXTURE,
        node[IMAGE_HOVER_IMAGE_ID],
        resources);
}

void Hachiko::ComponentImage::UpdateSize()
{
    unsigned width, height;
    App->camera->GetRenderingCamera()->GetResolution(width, height);
    if (width != size_x || height != size_y)
    {
        ComponentTransform2D* transform = game_object->GetComponent<ComponentTransform2D>();
        if (transform)
        {
            transform->SetSize(float2(width, height));
            size_x = width;
            size_y = height;
        }
    }
}

void Hachiko::ComponentImage::LoadImageResource(UID image_uid, bool is_hover)
{
    if (!is_hover)
    {
        App->resources->ReleaseResource(image);
        image = static_cast<ResourceTexture*>(App->resources->GetResource(Resource::Type::TEXTURE, image_uid));
        return;
    }
    App->resources->ReleaseResource(hover_image);
    hover_image = static_cast<ResourceTexture*>(App->resources->GetResource(Resource::Type::TEXTURE, image_uid));
}
