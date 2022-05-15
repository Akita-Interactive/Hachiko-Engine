#include "core/hepch.h"
#include "WindowResource.h"
#include "events/Event.h"
#include "modules/ModuleResources.h"
#include "modules/ModuleSceneManager.h"
#include "modules/ModuleEditor.h"
#include "modules/ModuleEvent.h"
#include "resources/ResourceModel.h"
#include "resources/ResourceMaterial.h"

Hachiko::WindowResource::WindowResource() : 
    Window("Resources", true)
{
}

void Hachiko::WindowResource::Update()
{
    ImGui::SetNextWindowDockID(App->editor->dock_down_id, ImGuiCond_FirstUseEver);
    if (!ImGui::Begin(StringUtils::Concat(ICON_FA_IMAGES, " ", name).c_str(), &active, ImGuiWindowFlags_NoNavInputs))
    {
        ImGui::End();
        return;
    }

    // WIP: create material 
    if (ImGui::Button("Create material (WIP)"))
    {
        ImGui::OpenPopup("CreateMaterialPopup");
        auxiliary_name = "NewMaterial";
    }

    if (ImGui::BeginPopup("CreateMaterialPopup"))
    {
        ImGui::InputText("Name", &auxiliary_name[0], 64);
        if (ImGui::Button("Create material"))
        {
            App->resources->CreateResource(Resource::Type::MATERIAL, auxiliary_name);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    //

    std::filesystem::path library_path("./");

    if (current_directory != library_path)
    {
        if (ImGui::Button(ICON_FA_FOLDER_MINUS))
        {
            current_directory = current_directory.parent_path();
        }
    }

    for (auto& directory_entry : std::filesystem::directory_iterator(current_directory))
    {
        const auto& path = directory_entry.path();
        auto relative_path = relative(path, library_path);
        std::string filename = relative_path.filename().string();

        if (directory_entry.is_directory())
        {
            if (ImGui::Button(filename.c_str()))
            {
                current_directory /= path.filename();
            }
        }
        else
        {
            auto selection = ImGui::Selectable(filename.c_str(), ImGuiSelectableFlags_AllowDoubleClick);
            if (ImGui::IsMouseDoubleClicked(selection) && ImGui::IsItemHovered())
            {
                filename.insert(0, "\\");
                filename.insert(0, current_directory.string().c_str());
                LoadResource(filename);
            }

            /* if (ImGui::IsMouseClicked(1))
            {
                ImGui::OpenPopup(filename.c_str());
            }

            if (ImGui::BeginPopup(filename.c_str()))
            {
                if (ImGui::MenuItem("Refresh asset"))
                {
                    App->resources->ReimportAsset(filename);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            } */
        }
    }
    ImGui::End();
}

void Hachiko::WindowResource::LoadResource(const std::string& path)
{
    HE_LOG("Resource file: %s", path.c_str());
    if (FileSystem::GetFileExtension(path.c_str())._Equal(META_EXTENSION))
    {
        
        YAML::Node node = YAML::LoadFile(path);
        Resource::Type type = static_cast<Resource::Type>(node[GENERAL_NODE][GENERAL_TYPE].as<unsigned>());
        switch (type)
        {
        case Resource::Type::MODEL:
            LoadModelIntoScene(node);
            break;
        case Resource::Type::MATERIAL:
            LoadMaterialIntoSelectedObject(node);
            break;
        }
    }
}

void Hachiko::WindowResource::LoadModelIntoScene(YAML::Node& node)
{
    HE_LOG("Adding a model into scene");
    Scene* scene = App->scene_manager->GetActiveScene();
    auto model_res = static_cast<ResourceModel*>(App->resources->GetResource(Resource::Type::MODEL, node[GENERAL_NODE][GENERAL_ID].as<UID>()));
    scene->HandleInputModel(model_res);
}

void Hachiko::WindowResource::LoadMaterialIntoSelectedObject(YAML::Node& node)
{
    HE_LOG("Adding a material into selected game object");
    Scene* scene = App->scene_manager->GetActiveScene();
    auto material_res = static_cast<ResourceMaterial*>(App->resources->GetResource(Resource::Type::MATERIAL, node[GENERAL_NODE][GENERAL_ID].as<UID>()));
    scene->HandleInputMaterial(material_res);
}
