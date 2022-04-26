#include "core/hepch.h"
#include "ComponentText.h"
#include "ComponentTransform2D.h"
#include "ComponentButton.h"
#include "ComponentCamera.h"

#include "modules/ModuleProgram.h"
#include "modules/ModuleCamera.h"

#include "Program.h"


Hachiko::ComponentText::ComponentText(GameObject* container) 
	: Component(Type::TEXT, container) {
}


void Hachiko::ComponentText::DrawGui()
{
    ImGui::PushID(this);
    if (ImGui::CollapsingHeader("Text Label", ImGuiTreeNodeFlags_DefaultOpen))
    {   
        if (ImGui::InputText("Font File", font_filename_buffer, MAX_PATH, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            std::string destination = std::string(ASSETS_FOLDER_FONTS) + "/" + font_filename_buffer;
            font = App->texture->LoadFont(destination.c_str());
            BuildLabel(game_object->GetComponent<ComponentTransform2D>());
        }
        

        if (!label)
        {
            ImGui::Text("Load a font to enable font controls");
            ImGui::PopID();
            return;
        }

        ImGui::Text("No text = size is too small, nothing can be drawn");

        if (ImGui::InputText("Text", &label_text, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            label->setText(label_text.c_str());
            Invalidate();
        }

        if (ImGui::DragFloat("Font Size", &font_size, 2.0f, 0.0f, FLT_MAX))
        {
            label->setPixelSize(static_cast<int>(font_size));
            Invalidate();
        }

        if (ImGuiUtils::CompactColorPicker("Font Color", font_color.ptr()))
        {
            label->setColor(font_color.x, font_color.y, font_color.z, font_color.w);
            Invalidate();
        }

	}
    ImGui::PopID();
}

void Hachiko::ComponentText::Draw(ComponentTransform2D* transform, Program* program)
{   
    if (!label)
    {
        return;
    }
    
    // Make sure transform is refreshed
    transform->GetGlobalTransform();
    RefreshLabel(transform);
    // Program is activated inside hachikorender
    label->HachikoRender(program);   
}

void Hachiko::ComponentText::Save(YAML::Node& node) const
{
    node[FONT_PATH] = font.path.c_str();
    node[FONT_COLOR] = font_color;
    node[FONT_SIZE] = font_size;
    node[FONT_LABEL_TEXT] = label_text;
}

void Hachiko::ComponentText::Load(const YAML::Node& node)
{
    const std::string font_path = node[FONT_PATH].as<std::string>();

    if (!font_path.empty())
    {
        font = App->texture->LoadFont(font_path.c_str());
        font_color = node[FONT_COLOR].as<float4>();
        font_size = node[FONT_SIZE].as<float>();
        label_text = node[FONT_LABEL_TEXT].as<std::string>();
        BuildLabel(game_object->GetComponent<ComponentTransform2D>());
    }
}

void Hachiko::ComponentText::RefreshLabel(ComponentTransform2D* transform)
{
    if (dirty)
    {
        unsigned windowWidth, windowHeight;
        App->camera->GetMainCamera()->GetResolution(windowWidth, windowHeight);
        label->setWindowSize(windowWidth, windowHeight);
        const float2& size = transform->GetSize();
        
        const float4x4& trf = transform->GetGlobalTransform();
        const float3& pos = trf.TranslatePart();
        label->scale(trf.scaleX, trf.scaleY, trf.scaleZ);
        label->setSize(size.x, size.y);
        label->setPosition(pos.x + (windowWidth / 2), pos.y + (windowHeight / 2));
        dirty = false;
    }
}

void Hachiko::ComponentText::BuildLabel(ComponentTransform2D* transform)
{
    unsigned windowWidth, windowHeight;
    App->camera->GetMainCamera()->GetResolution(windowWidth, windowHeight);

    const float4x4& trf = transform->GetGlobalTransform();
    
    label = std::unique_ptr<FTLabel>(new FTLabel(font.gl_font, // Font face handle
                                                 "", // Text to render
                                                 0,
                                                 0,
                                                 windowWidth,
                                                 windowHeight));
    
    label->setText(label_text.c_str()); // Text in constructor seems to not have effect
    label->setColor(font_color.x, font_color.y, font_color.z, font_color.w);
    label->setPixelSize(font_size);
    label->setAlignment(FTLabel::FontFlags::CenterAligned);
    RefreshLabel(transform);
}
