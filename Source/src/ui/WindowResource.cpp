#include "core/hepch.h"
#include "WindowResource.h"
#include "events/Event.h"
#include "modules/ModuleResources.h"
#include "modules/ModuleSceneManager.h"
#include "modules/ModuleEvent.h"

Hachiko::WindowResource::WindowResource() : 
    Window("Resources", true)
{
}

void Hachiko::WindowResource::Update()
{
    ImGui::SetNextWindowSize(ImVec2(1100, 170), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin(StringUtils::Concat(ICON_FA_IMAGES, " ", name).c_str(), &active))
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
        }
    }

    ImGui::End();
}

void Hachiko::WindowResource::LoadResource(const std::string& path)
{
    HE_LOG("Resource file: %s", path.c_str());
    /* Hachiko::Event resource_file(Hachiko::Event::Type::FILE_ADDED);
    resource_file.SetEventData<Hachiko::FileAddedEventPayload>(path.c_str());
    App->event->Publish(resource_file);*/

    //
    std::string str_path = std::string(path);
    int extension_index = str_path.rfind('.');
    std::string extension = str_path.substr(extension_index + 1, str_path.length() - (extension_index + 1));
    //

    if (extension == "model")
    {
        Scene* scene = App->scene_manager->GetActiveScene();
        scene->HandleInputModel(App->resources->GetModel(path.c_str()));
    }
}
