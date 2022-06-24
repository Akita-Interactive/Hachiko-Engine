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

Hachiko::ComponentImage::ComponentImage(GameObject* container) 
	: Component(Type::IMAGE, container) {
}

void Hachiko::ComponentImage::DrawGui()
{
    constexpr bool is_hover_image = true;

    ImGui::PushID(this);

    ImGui::Checkbox("Fill Window", &fill_window);

    if (ImGui::CollapsingHeader("Image", ImGuiTreeNodeFlags_DefaultOpen))
    {   

        ImGui::Text("Normal Image");
        ImGui::Checkbox("Use Image", &use_image);

        const std::string title = "Select Image";
        if (ImGui::Button(title.c_str()))
        {
            ImGuiFileDialog::Instance()->OpenDialog(title,
                                                    title,
                                                    ".png",
                                                    ASSETS_FOLDER_TEXTURE,
                                                    1,
                                                    nullptr,
                                                    ImGuiFileDialogFlags_DontShowHiddenFiles | ImGuiFileDialogFlags_DisableCreateDirectoryButton | ImGuiFileDialogFlags_HideColumnType
                                                        | ImGuiFileDialogFlags_HideColumnDate);
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

        if (!use_image || !image)
        {
            ImGuiUtils::CompactColorPicker("Image Color", color.ptr());
        }
        else
        {
            if (ImGui::Checkbox("Is tiled", &is_tiled))
            {
                if (!is_tiled)
                {
                    animation_index = {0.0f, 0.0f};
                    factor = float2::one;
                }
            }
            if (is_tiled)
            {
                if (ImGui::DragInt("Frames per second", &frames_per_second, 1, 1))
                {
                    time_per_frame = 1.0f / frames_per_second;
                }

                if (ImGui::DragInt("X tiles", &x_tiles, 1, 1))
                {
                    factor.x = 1.0f / x_tiles;
                }
                if (ImGui::DragInt("Y tiles", &y_tiles, 1, 1))
                {
                    factor.y = 1.0f / y_tiles;
                }
            }
        }

        ImGui::Text("Hover Image");

        const std::string hover_title = "Select Hover Image";
        if (ImGui::Button(hover_title.c_str()))
        {
            ImGuiFileDialog::Instance()->OpenDialog(hover_title,
                                                    hover_title,
                                                    ".png",
                                                    ASSETS_FOLDER_TEXTURE,
                                                    1,
                                                    nullptr,
                                                    ImGuiFileDialogFlags_DontShowHiddenFiles | ImGuiFileDialogFlags_DisableCreateDirectoryButton | ImGuiFileDialogFlags_HideColumnType
                                                        | ImGuiFileDialogFlags_HideColumnDate);
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

        if (!use_hover_image || !hover_image)
        {
            ImGuiUtils::CompactColorPicker("Hover Color", hover_color.ptr());
        }

        ImGui::Text("Image Loaded %d", image != nullptr);
        ImGui::Text("Hover Image Loaded %d", hover_image != nullptr);
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
    bool render_img = use_image;    

    ComponentButton* button = game_object->GetComponent<ComponentButton>();

    if (button && button->IsHovered())
    {
        img_to_draw = hover_image;
        render_color = &hover_color;
        render_img = use_hover_image;
    }

    program->BindUniformBool("diffuse_flag", img_to_draw && render_img);
    program->BindUniformFloat4("img_color", render_color->ptr());
    ModuleTexture::Bind(img_to_draw? img_to_draw->GetImageId(): 0, static_cast<int>(Hachiko::ModuleProgram::TextureSlots::DIFFUSE));

    program->BindUniformFloat2("factor", factor.ptr());
    program->BindUniformFloat2("animation_index", animation_index.ptr());

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Unbind square & deactivate program:
    App->ui->UnbindSquare();
    Program::Deactivate();
}

void Hachiko::ComponentImage::Update()
{
    if (fill_window)
    {
        UpdateSize();
    }
    if (use_image)
    {
        if (is_tiled)
        {
            elapse += EngineTimer::delta_time;
        
            while (elapse >= time_per_frame) 
            {
                elapse -= time_per_frame;

                if (animation_index.x < x_tiles - 1)
                {
                    animation_index.x += 1.0f;
                    break;
                }
                else if (animation_index.y < y_tiles - 1)
                {
                    animation_index.x = 0.0f;
                    animation_index.y += 1.0f;
                    break;
                }

                animation_index = {0.0f, 0.0f};
            }
        }
    }
}

void Hachiko::ComponentImage::Save(YAML::Node& node) const
{
    node.SetTag("image");
    node[IMAGE_IMAGE_ID] = image ? image->GetID() : 0;
    node[IMAGE_HOVER_IMAGE_ID] = hover_image ? hover_image->GetID() : 0;
    node[USE_IMAGE] = use_image;
    node[USE_HOVER_IMAGE] = use_hover_image;
    node[IMAGE_COLOR] = color;
    node[IMAGE_HOVER_COLOR] = hover_color;
    node["fill_window"] = fill_window;
}

void Hachiko::ComponentImage::Load(const YAML::Node& node)
{
    constexpr bool is_hover_image = true;
    if (node[USE_IMAGE].IsDefined())
    {
        use_image = node[USE_IMAGE].as<bool>();    
    }
    LoadImageResource(node[IMAGE_IMAGE_ID].as<UID>(), !is_hover_image);
    LoadImageResource(node[IMAGE_HOVER_IMAGE_ID].as<UID>(), is_hover_image);
    use_image = node[USE_IMAGE].as<bool>();
    use_hover_image = node[USE_HOVER_IMAGE].as<bool>();
    color = node[IMAGE_COLOR].as<float4>();
    hover_color = node[IMAGE_HOVER_COLOR].as<float4>();
    fill_window = node["fill_window"].IsDefined() ? node["fill_window"].as<bool>() : false;
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
