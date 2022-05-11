#include "core/hepch.h"
#include "ComponentImage.h"
#include "ComponentTransform2D.h"
#include "ComponentButton.h"

#include "modules/ModuleProgram.h"
#include "Application.h"
#include "modules/ModuleUserInterface.h"

#include "core/rendering/Program.h"



Hachiko::ComponentImage::ComponentImage(GameObject* container) 
	: Component(Type::IMAGE, container) {
}


void Hachiko::ComponentImage::DrawGui()
{
    ImGui::PushID(this);
    if (ImGui::CollapsingHeader("Image", ImGuiTreeNodeFlags_DefaultOpen))
    {   

        ImGui::Checkbox("Use Image", &use_image);
        
        if (ImGui::InputText("Image File", image_filename_buffer, MAX_PATH, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            ModuleTexture::Unload(image);
            std::string destination = std::string(ASSETS_FOLDER_TEXTURES) + "/" + image_filename_buffer;
            image = ModuleTexture::Load(destination.c_str());
        }

        if (!use_image || !image.loaded)
        {
            ImGuiUtils::CompactColorPicker("Color", color.ptr());
        }       

        ImGui::Checkbox("Use Hover Image", &use_hover_image);

        if (ImGui::InputText("Hover Image File", hover_image_filename_buffer, MAX_PATH, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            ModuleTexture::Unload(hover_image);
            std::string destination = std::string(ASSETS_FOLDER_TEXTURES) + "/" + hover_image_filename_buffer;
            hover_image = ModuleTexture::Load(destination.c_str());
        }

        if (!use_hover_image || !hover_image.loaded)
        {
            ImGuiUtils::CompactColorPicker("Hover Color", hover_color.ptr());
        }

        ImGui::Text("Image Loaded %d", image.loaded);
        ImGui::Text("Hover Image Loaded %d", hover_image.loaded);
	}
    ImGui::PopID();
}

void Hachiko::ComponentImage::Draw(ComponentTransform2D* transform, Program* program) const
{
    OPTICK_CATEGORY("Draw", Optick::Category::Rendering);

	// Bind matrix
    program->Activate();
    App->ui->BindSquare();
    program->BindUniformFloat4x4("model", transform->GetGlobalScaledTransform().ptr());
    const Texture* img_to_draw = &image;
    const float4* render_color = &color;
    bool render_img = use_image;    
    

    ComponentButton* button = game_object->GetComponent<ComponentButton>();

    if (button && button->IsHovered())
    {
        img_to_draw = &hover_image;
        render_color = &hover_color;
        render_img = use_hover_image;
    }

    program->BindUniformBool("diffuse_flag", img_to_draw->loaded && render_img);
    program->BindUniformFloat4("img_color", render_color->ptr());
    ModuleTexture::Bind(img_to_draw->id, static_cast<int>(Hachiko::ModuleProgram::TextureSlots::DIFFUSE));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Hachiko::ComponentImage::Save(YAML::Node& node) const
{
    node[IMAGE_PATH] = image.path.c_str();
    node[HOVER_IMAGE_PATH] = hover_image.path.c_str();
}

void Hachiko::ComponentImage::Load(const YAML::Node& node)
{
    const std::string image_path = node[IMAGE_PATH].as<std::string>();
    const std::string hover_image_path = node[HOVER_IMAGE_PATH].as<std::string>();

    if (!image_path.empty())
    {
        image = ModuleTexture::Load(image_path.c_str());
        if (image.loaded)
        {
            use_image = true;
        }
    }

    if (!hover_image_path.empty())
    {
        hover_image = ModuleTexture::Load(hover_image_path.c_str());
        use_hover_image = true;
    }
}

void Hachiko::ComponentImage::Import(const char* path)
{
    image = ModuleTexture::Load(path);
    if (image.loaded)
    {
        use_image = true;
    }
}
