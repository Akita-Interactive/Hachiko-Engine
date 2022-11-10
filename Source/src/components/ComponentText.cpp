#include "core/hepch.h"
#include "ComponentText.h"
#include "ComponentTransform2D.h"
#include "ComponentCamera.h"
#include "resources/ResourceFont.h"

#include "modules/ModuleCamera.h"
#include "modules/ModuleResources.h"

#include "core/rendering/Program.h"
#include "modules/ModuleResources.h"
#include "modules/ModuleSceneManager.h"

#include "modules/ModuleEvent.h"

#include "resources/ResourceFont.h"
#include "FTLabel.h"
#include "utils/ComponentUtility.h"

Hachiko::ComponentText::ComponentText(GameObject* container) :
    Component(Type::TEXT, container)
{
}

Hachiko::ComponentText::~ComponentText()
{
    App->resources->ReleaseResource(font);
}

void Hachiko::ComponentText::DrawGui()
{
    static const ImVec4 warn_color = ImVec4(255, 0, 0, 1);

    ImGui::PushID(this);
    if (ImGuiUtils::CollapsingHeader(this, "Text label"))
    {
        if (!font)
        {
            ImGui::TextColored(warn_color, "No loaded font!");
        }
        if (!label)
        {
            ImGui::TextColored(warn_color, "No generated label!");
        }

        if (Widgets::Input("Text", label_text, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            SetText(label_text.c_str());
        }

        float fz = font_size;
        Widgets::DragFloatConfig cfg;
        cfg.speed = 2.0f;
        cfg.format = "%.f";
        cfg.min = 0.0f;
        cfg.max = 1010.0f;
        if (DragFloat("Font size", fz, &cfg))
        {
            SetFontSize(fz);
        }

        if (ImGuiUtils::CompactColorPicker("Font color", font_color.ptr()))
        {
            SetFontColor(font_color);
        }

        const std::string title = "Select font";
        if (ImGui::Button("Change font", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
        {
            ImGuiFileDialog::Instance()->OpenDialog(title,
                                                    "Select font",
                                                    FONT_EXTENSION,
                                                    ASSETS_FOLDER_FONT,
                                                    1,
                                                    nullptr,
                                                    ImGuiFileDialogFlags_DontShowHiddenFiles | ImGuiFileDialogFlags_DisableCreateDirectoryButton | ImGuiFileDialogFlags_HideColumnType
                                                    | ImGuiFileDialogFlags_HideColumnDate);
        }

        if (ImGuiFileDialog::Instance()->Display(title))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string font_path = ImGuiFileDialog::Instance()->GetFilePathName();
                font_path.append(META_EXTENSION);
                YAML::Node font_node = YAML::LoadFile(font_path);
                LoadFont(font_node[RESOURCES][0][RESOURCE_ID].as<UID>());
                App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
            }

            ImGuiFileDialog::Instance()->Close();
        }

        ImGui::TextWrapped("No text = size is too small, nothing can be drawn");
    }
    ImGui::PopID();
}

void Hachiko::ComponentText::Draw(ComponentTransform2D* transform, Program* program)
{
    OPTICK_CATEGORY("Draw", Optick::Category::Rendering);

    if (build_font && font)
    {
        try        
        {
            BuildLabel(game_object->GetComponent<ComponentTransform2D>());
        }
        catch (std::exception& e)
        {
            // Catch exception and return unloaded font if fails
            HE_LOG("Failed to load font %s", std::to_string(font->GetID()).c_str());
        }    
    }
    if (!label)
    {
        return;
    }

    RefreshLabel(transform);
    // Program is activated inside HachikoRender
    label->HachikoRender(program);
}

void Hachiko::ComponentText::Save(YAML::Node& node) const
{
    node.SetTag("text");
    node[FONT_ID] = font ? font->GetID() : 0;
    node[FONT_COLOR] = font_color;
    node[FONT_SIZE] = font_size;
    node[FONT_LABEL_TEXT] = label_text;
}

void Hachiko::ComponentText::Load(const YAML::Node& node)
{
    font_color = node[FONT_COLOR].as<float4>();
    font_size = node[FONT_SIZE].as<int>();
    label_text = node[FONT_LABEL_TEXT].as<std::string>();
    LoadFont(node[FONT_ID].as<UID>());
    SetText(label_text.c_str());
    SetFontSize(font_size);
    SetFontColor(font_color);
}

void Hachiko::ComponentText::CollectResources(const YAML::Node& node, std::map<Resource::Type, std::set<UID>>& resources)
{
    ComponentUtility::CollectResource(
        Resource::Type::FONT,
        node[FONT_ID],
        resources);
}

void Hachiko::ComponentText::SetText(const char* new_text)
{
    if (label)
    {
        label->setText(new_text);
        label_text = new_text;
    }
}

void Hachiko::ComponentText::SetFontSize(int new_size)
{
    if (label)
    {
        label->setPixelSize(new_size);
        font_size = new_size;
    }
}

void Hachiko::ComponentText::SetFontColor(const float4& new_color)
{
    if (label)
    {
        label->setColor(new_color.x, new_color.y, new_color.z, new_color.w);
        font_color = new_color;
    }
}

void Hachiko::ComponentText::LoadFont(UID id)
{
    try
    {
        App->resources->ReleaseResource(font);
        font = static_cast<ResourceFont*>(App->resources->GetResource(Resource::Type::FONT, id));
        if (font && !App->scene_manager->IsLoadingScene())
        {
            BuildLabel(game_object->GetComponent<ComponentTransform2D>());
        }
    }
    catch (std::exception& e)
    {
        // Catch exception and return unloaded font if fails
        HE_LOG("Failed to load font %s", std::to_string(id).c_str());
    }
}

void Hachiko::ComponentText::RefreshLabel(ComponentTransform2D* transform)
{
    if (dirty)
    {
        try
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
        catch (std::exception& e)
        {
            // Catch exception and return unloaded font if fails
            HE_LOG("Failed to RefreshLabel");
        }
    }
}

void Hachiko::ComponentText::BuildLabel(ComponentTransform2D* transform)
{
    build_font = false;

    unsigned windowWidth, windowHeight;
    App->camera->GetRenderingCamera()->GetResolution(windowWidth, windowHeight);

    const float4x4& trf = transform->GetGlobalTransform();
    label = std::unique_ptr<FTLabel>(new FTLabel(font->gl_font.get(),
                                                 // Font face handle
                                                 windowWidth,
                                                 windowHeight));

    label->setText(label_text.c_str()); // Text in constructor seems to not have effect
    label->setColor(font_color.x, font_color.y, font_color.z, font_color.w);
    label->setPixelSize(font_size);
    label->setAlignment(FTLabel::FontFlags::CenterAligned);
    Invalidate();
}
