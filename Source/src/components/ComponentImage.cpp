#include "core/hepch.h"
#include "ComponentImage.h"
#include "ComponentTransform2D.h"
#include "ComponentButton.h"

#include "modules/ModuleProgram.h"

#include "Program.h"



Hachiko::ComponentImage::ComponentImage(GameObject* container) 
	: Component(Type::IMAGE, container) {
}


void Hachiko::ComponentImage::DrawGui()
{
    ImGui::PushID(this);
    if (ImGui::CollapsingHeader("Image", ImGuiTreeNodeFlags_DefaultOpen))
    {   

        ImGui::Checkbox("Use Image", &use_image);

        static char image_filename_buffer[MAX_PATH] = "Image Filename\0";
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

        static char hover_image_filename_buffer[MAX_PATH] = "Hover Image Filename\0";
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
	// Bind matrix
    program->BindUniformFloat4x4("model", transform->GetGlobalScaledTransform().ptr());
    // TODO
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

void Hachiko::ComponentImage::Save(JsonFormatterValue j_component) const
{
    const JsonFormatterValue j_image_path = j_component["ImagePath"];
    const JsonFormatterValue j_hover_image_path = j_component["HoverImagePath"];
    j_image_path = image.path.c_str();
    j_hover_image_path = hover_image.path.c_str();
}

void Hachiko::ComponentImage::Load(JsonFormatterValue j_component)
{
    const JsonFormatterValue j_image_path = j_component["ImagePath"];
    const JsonFormatterValue j_hover_image_path = j_component["HoverImagePath"];
    const std::string image_path = j_image_path;
    const std::string hover_image_path = j_hover_image_path;

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
