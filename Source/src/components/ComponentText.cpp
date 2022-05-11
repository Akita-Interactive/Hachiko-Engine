#include "core/hepch.h"
#include "ComponentText.h"
#include "ComponentTransform2D.h"
#include "ComponentButton.h"
#include "ComponentCamera.h"
#include "resources/ResourceFont.h"

#include "modules/ModuleProgram.h"
#include "modules/ModuleCamera.h"
#include "modules/ModuleResources.h"

#include "core/rendering/Program.h"


Hachiko::ComponentText::ComponentText(GameObject* container) 
	: Component(Type::TEXT, container) {
}


void Hachiko::ComponentText::DrawGui()
{
    static const ImVec4 warn_color = ImVec4(255, 0, 0, 1);

    ImGui::PushID(this);
    if (ImGui::CollapsingHeader("Text Label", ImGuiTreeNodeFlags_DefaultOpen))
    {   
        if (!font)
        {
            ImGui::TextColored(warn_color, "No Loaded Font!");
        }
        if (!label)
        {
            ImGui::TextColored(warn_color, "No Generated Label!");
        }
        

        if (ImGui::InputText("Text", &label_text, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            SetText(label_text.c_str());
        }

        if (ImGui::DragFloat("Font Size", &font_size, 2.0f, 0.0f, FLT_MAX))
        {
            
            SetFontSize(static_cast<int>(font_size));
        }

        if (ImGuiUtils::CompactColorPicker("Font Color", font_color.ptr()))
        {
            SetFontColor(font_color);
        }

        const std::string title = "Select Font";
        if (ImGui::Button("Change Font"))
        {
            ImGuiFileDialog::Instance()->OpenDialog(title,
                                                    "Select Font",
                                                    ".ttf,",
                                                    "./assets/fonts/",
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
                LoadFont(font_node[GENERAL_NODE][GENERAL_ID].as<UID>());
            }

            ImGuiFileDialog::Instance()->Close();
        }

        ImGui::Text("No text = size is too small, nothing can be drawn");
	}
    ImGui::PopID();
}

void Hachiko::ComponentText::Draw(ComponentTransform2D* transform, Program* program)
{   
    OPTICK_CATEGORY("Draw", Optick::Category::Rendering);

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
    node[FONT_COLOR] = font_color;
    node[FONT_SIZE] = font_size;
    node[FONT_LABEL_TEXT] = label_text;
}

void Hachiko::ComponentText::Load(const YAML::Node& node)
{
    SetID(node[COMPONENT_ID].as<UID>());   
    font_color = node[FONT_COLOR].as<float4>();
    font_size = node[FONT_SIZE].as<float>();
    label_text = node[FONT_LABEL_TEXT].as<std::string>();
    LoadFont(node[COMPONENT_ID].as<UID>());
}

void Hachiko::ComponentText::SetText(const char* new_text)
{
    if (label)
    {
        label->setText(new_text);
    }
}

void Hachiko::ComponentText::SetFontSize(int new_size)
{
    if (label)
    {
        label->setPixelSize(static_cast<int>(font_size));
    }
}

void Hachiko::ComponentText::SetFontColor(const float4& new_color)
{
    if (label)
    {
        label->setColor(font_color.x, font_color.y, font_color.z, font_color.w);
    }
}

void Hachiko::ComponentText::LoadFont(UID id)
{
    try
    {
        font = static_cast<ResourceFont*>(App->resources->GetResource(Resource::Type::FONT, id));
        BuildLabel(game_object->GetComponent<ComponentTransform2D>());
        // TODO: Fix how this works, right now it uses component id to find font
        SetID(id);
    }
    catch (std::exception& e)
    {
        // Catch exception and return unloaded font if fails
        HE_LOG("Failed to load font %d", id);
    }    
}

void Hachiko::ComponentText::RefreshLabel(ComponentTransform2D* transform)
{
    if (dirty)
    {
        unsigned windowWidth, windowHeight;
        App->camera->GetRenderingCamera()->GetResolution(windowWidth, windowHeight);
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
    App->camera->GetRenderingCamera()->GetResolution(windowWidth, windowHeight);

    const float4x4& trf = transform->GetGlobalTransform();
    label = std::unique_ptr<FTLabel>(new FTLabel(font->gl_font.get(), // Font face handle
                                                 windowWidth,
                                                 windowHeight));
    
    label->setText(label_text.c_str()); // Text in constructor seems to not have effect
    label->setColor(font_color.x, font_color.y, font_color.z, font_color.w);
    label->setPixelSize(font_size);
    label->setAlignment(FTLabel::FontFlags::CenterAligned);
    RefreshLabel(transform);
}
