#include "core/hepch.h"

#include "ComponentMaterial.h"

#include "modules/ModuleTexture.h"
#include "modules/ModuleResources.h"

#include "resources/ResourceMaterial.h"
#include "importers/MaterialImporter.h"

Hachiko::ComponentMaterial::ComponentMaterial(GameObject* container) 
    : Component(Type::MATERIAL, container) 
{
}

void Hachiko::ComponentMaterial::Save(YAML::Node& node) const
{
    if (material == nullptr)
    {
        return;
    }

    node[MATERIAL_NAME] = material->GetName();
}

void Hachiko::ComponentMaterial::Load(const YAML::Node& node)
{
    SetID(node[COMPONENT_ID].as<UID>());
    LoadMaterial(node[MATERIAL_NAME].as<std::string>());
    material->SetName(node[MATERIAL_NAME].as<std::string>());
}

void Hachiko::ComponentMaterial::DrawGui()
{
    ImGui::PushID(this);
    if (ImGuiUtils::CollapsingHeader(game_object, this, "Material"))
    {
        if (material != nullptr)
        {
            material->DrawGui();
        }
        else
        {
            ImGui::Text("No material resource");
            // TODO: material selection option
        }
    }
    ImGui::PopID();
}

void Hachiko::ComponentMaterial::LoadMaterial(const std::string& material_path)
{
    material = App->resources->GetMaterial(material_path);
}
