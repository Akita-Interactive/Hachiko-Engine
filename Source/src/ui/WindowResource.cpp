#include "core/hepch.h"
#include "WindowResource.h"
#include "events/Event.h"
#include "modules/ModuleResources.h"
#include "modules/ModuleSceneManager.h"
#include "modules/ModuleEditor.h"
#include "modules/ModuleEvent.h"
#include "resources/ResourceMaterial.h"

int constexpr MAX_COLUMN_FILES = 3;
int constexpr MAX_FILES_PER_COLUMN = 20;
static bool updated;

Hachiko::WindowResource::WindowResource() : 
    Window("Resources", true)
{
}

void Hachiko::WindowResource::Init()
{
    root_directory = "./assets";
    current_path = root_directory;
    updated = false;
}

void Hachiko::WindowResource::Update()
{

    ImGui::SetNextWindowDockID(App->editor->dock_down_id, ImGuiCond_FirstUseEver);
    if (!ImGui::Begin(StringUtils::Concat(ICON_FA_IMAGES, " ", name).c_str(), &active, ImGuiWindowFlags_NoNavInputs))
    {
        ImGui::End();
        return;
    }

    if (!updated)
    {
        UpdateDirectoryInfo();
        ImGui::End();
        return;
    }

    static float padding = 16.0f;
    static float thumbnail_size = 128.0f; 
    static float cell_size = thumbnail_size + padding;
    float panel_width = ImGui::GetContentRegionAvail().x;
    int column_count = (int)(panel_width / cell_size);
    if (column_count < 1)
    {
        column_count = 1;
    }

    ImGui::Text(current_path.string().substr(2).c_str());
    ImGui::Separator();
    ImGui::Columns(column_count, 0, false);

    if (!current_path.string()._Equal(root_directory.string()))
    {
        if (ImGui::Button(ICON_FA_ARROW_LEFT, {thumbnail_size * 0.5f, thumbnail_size * 0.3f}))
        {
            current_path = current_path.parent_path();
            Reload();
            ImGui::End();
            return;
        }
        ImGui::NextColumn();
    }

    CreateMaterialGui({thumbnail_size, thumbnail_size * 0.3f});

    for (const auto& directory_entry : current_content)
    {
        if (directory_entry.is_directory)
        {
            if (ImGui::Button(StringUtils::Concat(ICON_FA_FOLDER, " ", directory_entry.name).c_str(), {thumbnail_size, thumbnail_size * 0.3f}))
            {
                current_path = directory_entry.path;
                Reload();
                break;
            }
            ImGui::NextColumn();
        }
    }

    ImGui::Columns(MAX_COLUMN_FILES);
    ImGui::Separator();

    int items_counter = 0;

    for (const auto& directory_entry : current_content)
    {
        if (!directory_entry.is_directory)
        {
            if (FileSystem::GetFileExtension(directory_entry.name.c_str()) == META_EXTENSION)
            {
                continue;
            }
            
            auto selection = ImGui::Selectable(directory_entry.name.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick);
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered())
            {
                LoadAsset(directory_entry.path.string());
                App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
            }

            ++items_counter;

            if (items_counter > MAX_FILES_PER_COLUMN)
            {
                ImGui::NextColumn();
                items_counter = 0;
            }
        }
    }

    ImGui::Columns(1);
    ImGui::End();
}

void Hachiko::WindowResource::Reload()
{
    updated = false;
}

void Hachiko::WindowResource::LoadAsset(const std::string& path)
{
    HE_LOG("Resource file: %s", path.c_str());
    if (!FileSystem::GetFileExtension(path.c_str())._Equal(META_EXTENSION))
    {
        App->resources->LoadAsset(path);
    }
}

void Hachiko::WindowResource::CreateMaterialGui(const ImVec2 &button_size)
{
    if (current_path != "./assets/materials")
    {
        return;
    }

    if (ImGui::Button("Create material", button_size))
    {
        ImGui::OpenPopup("CreateMaterialPopup");
        auxiliary_name = "NewMaterial";
    }

    if (ImGui::BeginPopup("CreateMaterialPopup"))
    {
        ImGui::InputText("Name", &auxiliary_name[0], 64);
        if (ImGui::Button("Create material"))
        {
            App->resources->CreateAsset(Resource::Type::MATERIAL, auxiliary_name);
            ImGui::CloseCurrentPopup();
            Reload();
        }
        ImGui::EndPopup();
    }

    ImGui::NextColumn();
}

void Hachiko::WindowResource::UpdateDirectoryInfo()
{
    current_content.clear();

    for (auto& directory_entry : std::filesystem::directory_iterator(current_path))
    {
        Entry tmp_entry;
        tmp_entry.path = directory_entry.path();

        if (directory_entry.is_directory())
        {
            tmp_entry.is_directory = true;
            tmp_entry.name = std::filesystem::relative(directory_entry.path(), current_path).string();
        }
        else
        {
            tmp_entry.is_directory = false;
            tmp_entry.name = directory_entry.path().filename().string();
        }

        current_content.emplace_back(tmp_entry);
    }

    updated = true;
}
