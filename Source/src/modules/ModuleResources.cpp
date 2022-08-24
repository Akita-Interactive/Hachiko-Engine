#include "core/hepch.h"

#include "ModuleResources.h"
#include "ModuleEvent.h"
#include "ModuleSceneManager.h"
#include "ModuleEditor.h"

#include "core/preferences/src/ResourcesPreferences.h"

#include "importers/MeshImporter.h"
#include "importers/TextureImporter.h"
#include "importers/MaterialImporter.h"
#include "importers/ModelImporter.h"
#include "importers/PrefabImporter.h"

#include "components/ComponentMeshRenderer.h"
#include "resources/ResourceMaterial.h"
#include "resources/ResourceTexture.h"
#include "resources/ResourceSkybox.h"

using namespace Hachiko;

bool ModuleResources::Init()
{
    HE_LOG("INITIALIZING MODULE: RESOURCES");

    preferences = App->preferences->GetResourcesPreference();

    // create assets & library directory tree

    for (auto& lib_path : preferences->GetLibraryPathsMap())
    {
        FileSystem::CreateDir(lib_path.second.c_str());
    }

#ifndef PLAY_BUILD
    for (auto& asset_path : preferences->GetAssetsPathsMap())
    {
        FileSystem::CreateDir(asset_path.second.c_str());
    }
    FileSystem::CreateDir(ASSETS_FOLDER_GENERATED_PREFAB);

    AssetsLibraryCheck();
    ClearUnusedResources(managed_uids);

    HE_LOG("Finished aligning assets: %d assets imported, %d unused resources cleaned", imported_assets, cleaned_resources);

    std::function handleAddedFile = [&](Event& evt) {
        const auto& e = evt.GetEventData<FileAddedEventPayload>();
        std::filesystem::path file = e.GetPath();
        HE_LOG("Handling dropped file: %s", file.string().c_str());
        ImportAssetFromAnyPath(file);
    };

    App->event->Subscribe(Event::Type::FILE_ADDED, handleAddedFile);
#endif

    return true;
}

bool ModuleResources::CleanUp()
{
    for (auto& it : loaded_resources)
    {
        HE_LOG("Removing unreleased resources (fbi coming)");
        delete it.second.resource;
    }

    return true;
}

std::filesystem::path ModuleResources::GetLastResourceLoadedPath() const
{
    return last_resource_path.parent_path();
}

std::vector<UID> ModuleResources::ImportAssetFromAnyPath(const std::filesystem::path& path, bool force)
{
    Resource::AssetType type = GetAssetTypeFromPath(path);
    
    if (type == Resource::AssetType::UNKNOWN)
    {
        return std::vector<UID>();
    }

    size_t relative_pos = path.string().find("assets/");
    bool file_in_asset = relative_pos != std::string::npos;
    std::filesystem::path destination;

    if (file_in_asset)
    {
        destination = path;
    }
    else
    {
        destination = preferences->GetAssetsPath(type);
        destination.append(path.filename().c_str());
        FileSystem::Copy(path.string().c_str(), destination.string().c_str(), true);
    }

    Hachiko::WindowResource::Reload(); // Update UI
    // Handle asset when it is in correct path
    return ImportAsset(destination.string(), force);
}

Resource::AssetType ModuleResources::GetAssetTypeFromPath(const std::filesystem::path& path)
{
    if (!path.has_extension())
    {
        return Resource::AssetType::UNKNOWN;
    }
    const std::filesystem::path extension = path.extension();

    auto isValidExtension = [&](const std::pair<Resource::AssetType, std::string>& element)
    {
        return element.second.size() == extension.string().size() &&
            std::equal(element.second.begin(), element.second.end(), extension.string().begin(),
                       [](auto a, auto b) { return std::tolower(a) == std::tolower(b); });
    };

    const auto it = std::find_if(supported_extensions.begin(), supported_extensions.end(), isValidExtension);
    if (it != supported_extensions.end())
    {
        // TODO: Remove exception when skybox is hdr since it will not creaate confusion
        if (path.string().find("skybox/") != std::string::npos)
        {
            return Resource::AssetType::SKYBOX;
        }
        return it->first;
    }
    return Resource::AssetType::UNKNOWN;
}

Resource* Hachiko::ModuleResources::GetResource(Resource::Type type, UID id)
{
    // If resource already loaded return
    auto it = loaded_resources.find(id);
    if (it != loaded_resources.end())
    {
        it->second.n_users += 1;
        return it->second.resource;
    }
    // If not loaded try to load and return
    auto res = importer_manager.LoadResource(type, id);
    if (res != nullptr)
    {
        ResourceInstance& resource_instance = loaded_resources.emplace(id, ResourceInstance {res, 1}).first->second;
        return resource_instance.resource;
    }
    return nullptr;
}

void Hachiko::ModuleResources::ReleaseResource(Resource* resource)
{
    if (resource)
    {
        ReleaseResource(resource->GetID());
    }
}

void Hachiko::ModuleResources::ReleaseResource(UID id)
{
    auto it = loaded_resources.find(id);
    if (it != loaded_resources.end())
    {
        ResourceInstance& res_instance = it->second;
        res_instance.n_users -= 1;
        if (res_instance.n_users == 0)
        {
            delete res_instance.resource;
            loaded_resources.erase(id);
        }
    }
}

std::vector<UID> Hachiko::ModuleResources::CreateAsset(Resource::Type type, const std::string& name) const
{
    std::vector<UID> new_resources = std::vector<UID>();
    switch (type)
    {
    case Resource::Type::MATERIAL:
        {
            MaterialImporter material_importer;
            new_resources.emplace_back(material_importer.CreateEmptyMaterial(name));
            break;
        }
    case Resource::Type::STATE_MACHINE:
        {
            StateMachineImporter state_machine_importer;
            new_resources.emplace_back(state_machine_importer.CreateAsset(name));
            break;
        }
    }
    return new_resources;
}

void Hachiko::ModuleResources::LoadAsset(const std::string& path)
{
    Resource::AssetType asset_type = GetAssetTypeFromPath(path);

    if (asset_type != Resource::AssetType::UNKNOWN)
    {
        // Infer meta path from the asset name, it should always already exist since everything gets imported on start
        std::string meta_path = StringUtils::Concat(path, META_EXTENSION);

        YAML::Node meta_node = YAML::LoadFile(meta_path);

        // If its a model or a prefab load them into scene, load already does so internally
        switch (asset_type)
        {
        case Resource::AssetType::MODEL:
            {
                PrefabImporter importer;
                constexpr unsigned n_instances = 1;
                importer.CreateObjectsFromPrefab(meta_node[PREFAB_ID].as<UID>(), nullptr, n_instances);
                break;
            }
            
        case Resource::AssetType::PREFAB:
            {
                PrefabImporter importer;
                constexpr unsigned n_instances = 1;
                importer.CreateObjectsFromPrefab(meta_node[RESOURCES][0][RESOURCE_ID].as<UID>(), nullptr, n_instances);    
                break;
            }
        case Resource::AssetType::MATERIAL:
            {
                Scene* scene = App->scene_manager->GetActiveScene();
                // Material asset only contains a single material resource
                ResourceMaterial* material_res = static_cast<ResourceMaterial*>(App->resources->GetResource(Resource::Type::MATERIAL, meta_node[RESOURCES][0].as<UID>()));
                scene->HandleInputMaterial(material_res);
                break;
            }
        case Resource::AssetType::SCENE:
            // Scene asset has to keep its scene id at the first position, navmesh is found inside scene serialization
            // It is also in resources array so it is reimported but no need to pass it here
            // Since when we load a scene asset we do it manually stop execution
            constexpr bool stop_scene = true;
            App->scene_manager->ChangeSceneById(meta_node[RESOURCES][0][RESOURCE_ID].as<UID>(), stop_scene);
        }
    }
}

std::vector<Hachiko::GameObject*> Hachiko::ModuleResources::InstantiatePrefab(UID prefab_uid, GameObject* parent, unsigned n_instances)
{
    return std::move(importer_manager.prefab.CreateObjectsFromPrefab(prefab_uid, parent, n_instances));
}

void Hachiko::ModuleResources::AssetsLibraryCheck()
{
    HE_LOG("Assets/Library check...");

    // Ignores files that dont need any short of importing themselves
    // Metas are searched for based on what's on assets
    std::vector<std::string> ignore_extensions {"meta"};    

    // Call it this way to control asset import order (we need meshes to exist to import scene navmesh)
    const auto& asset_paths = preferences->GetAssetsPathsMap();
    for (auto it = asset_paths.begin(); it != asset_paths.end(); ++it)
    {
        const PathNode folder = FileSystem::GetAllFiles(it->second.c_str(), nullptr, &ignore_extensions);
        GenerateLibrary(folder);
    }
    HE_LOG("Assets/Library check finished.");
}

void Hachiko::ModuleResources::GenerateLibrary(const PathNode& folder)
{
    // Iterate all files found in assets except metas and scene
    for (const PathNode& path_node : folder.children)
    {
        if (!path_node.isFile)
        {
            GenerateLibrary(path_node);
            continue;
        }
        std::vector<UID> new_uids = ImportAsset(path_node.path);
        for (auto& uid : new_uids)
        {
            managed_uids.emplace(uid);
        }
        
    }
}

std::vector<UID> Hachiko::ModuleResources::ImportAsset(const std::string& asset_path, bool force)
{
    Resource::AssetType type = GetAssetTypeFromPath(asset_path);
    
    if (type == Resource::AssetType::UNKNOWN)
    {
        // Discard unrecognised types
        return std::vector<UID>();
    }
    
    // Infer meta path from the asset name
    std::string meta_path = StringUtils::Concat(asset_path, META_EXTENSION);

    if (!FileSystem::Exists(meta_path.c_str()))
    {
        // If it doesnt have meta create asset from scratch, pass empty meta resources list to do so
        YAML::Node meta_node = CreateMeta();
        UpdateAssetHash(asset_path.c_str(), meta_node);
        return ImportAssetResources(asset_path, meta_node);
    }

    // If it has meta align on library and refresh if necessary
    YAML::Node meta_node = YAML::LoadFile(meta_path);

    // Do validations over meta information
    uint64_t previous_asset_hash = meta_node[ASSET_HASH].IsDefined() ? meta_node[ASSET_HASH].as<uint64_t>() : 0;
    uint64_t asset_hash = FileSystem::HashFromPath(asset_path.c_str());

    // Get the asset hash and reimport if it has changed or if we force it
    if (force || previous_asset_hash != asset_hash)
    {
        UpdateAssetHash(asset_path.c_str(), meta_node);
        return ImportAssetResources(asset_path, meta_node);
    }
        
    // Reimport if any lib file is missing
    bool valid_lib = ValidateAssetResources(meta_node);
    if (!valid_lib)
    {
        return ImportAssetResources(asset_path, meta_node);
    }
    
    // If it exists and has a valid lib just collect the resource ids to return them
    std::vector<UID> resource_ids = std::vector<UID>();
    for (unsigned i = 0; i < meta_node[RESOURCES].size(); ++i)
    {
        resource_ids.push_back(meta_node[RESOURCES][i][RESOURCE_ID].as<UID>());
    }
    return resource_ids;
}

std::vector<UID> ModuleResources::ImportAssetResources(const std::string& asset_path, YAML::Node& meta)
{
    // Import asset with its corresponding importer
    Resource::AssetType asset_type = GetAssetTypeFromPath(asset_path);
    // Get and propagate outwards all the generated uids
    std::vector<UID> imported_ids = importer_manager.ImportAsset(asset_path, asset_type, meta);
    for (unsigned i = 0; i < imported_ids.size(); ++i)
    {
        // Refresh the meta file resource hashes
        Resource::Type resource_type = static_cast<Resource::Type>(meta[RESOURCES][i][RESOURCE_TYPE].as<int>());
        std::string library_path = StringUtils::Concat(preferences->GetLibraryPath(resource_type), std::to_string(imported_ids[i]));
        if (resource_type == Resource::Type::FONT)
        {
            library_path += FONT_EXTENSION;
        }
        meta[RESOURCES][i][RESOURCE_HASH] = FileSystem::HashFromPath(library_path.c_str());
    }
    std::string meta_path = asset_path + META_EXTENSION;
    if (imported_ids.size() > 0)
    {
        FileSystem::Save(meta_path.c_str(), meta);
    }

    ++imported_assets;
    return imported_ids;
}

bool Hachiko::ModuleResources::ValidateAssetResources(const YAML::Node& meta) const
{
    // Validate the resources direcly handled by that asset (on the resources list)
    // It might refer to other resources on the meta definition but he is not in charge of maintaining them
    for (unsigned i = 0; i < meta[RESOURCES].size(); ++i)
    {
        UID resource_uid = meta[RESOURCES][i][RESOURCE_ID].as<UID>();
        Resource::Type resource_type = static_cast<Resource::Type>(meta[RESOURCES][i][RESOURCE_TYPE].as<int>());
        std::string library_path = StringUtils::Concat(preferences->GetLibraryPath(resource_type), std::to_string(resource_uid));
        if (resource_type == Resource::Type::FONT)
        {
            library_path += FONT_EXTENSION;
        }

        // If it resource is missing it is invalid
        if (!FileSystem::Exists(library_path.c_str()))
        {
            return false;
        }

        // If resource hash does not match meta it is invalid
        uint64_t meta_resource_hash = meta[RESOURCES][i][RESOURCE_HASH].IsDefined() ? meta[RESOURCES][i][RESOURCE_HASH].as<uint64_t>() : 0;
        uint64_t reosuce_hash = FileSystem::HashFromPath(library_path.c_str());

        if (meta_resource_hash != reosuce_hash)
        {
            return false;
        }
    }
    return true;
}

void Hachiko::ModuleResources::ClearUnusedResources(const std::set<UID>& seen_uids)
{
    const auto& library_paths = preferences->GetLibraryPathsMap();
    for (auto it = library_paths.begin(); it != library_paths.end(); ++it)
    {
        const PathNode folder = FileSystem::GetAllFiles(it->second.c_str());
        ClearLibrary(folder, seen_uids);
    }
    HE_LOG("Library cleanup finished.");
}

void Hachiko::ModuleResources::ClearLibrary(const PathNode& folder, const std::set<UID>& seen_uids)
{
    // Iterate all files found in assets except metas and scene
    for (const PathNode& path_node : folder.children)
    {
        UID file_name = static_cast<UID>(std::stoull(FileSystem::GetFileName(path_node.path.c_str())));
        if (seen_uids.find(file_name) == seen_uids.end())
        {
            FileSystem::Delete(path_node.path.c_str());
            ++cleaned_resources;
        }
    }
}


YAML::Node Hachiko::ModuleResources::CreateMeta()
{
    return YAML::Node();
}

void Hachiko::ModuleResources::UpdateAssetHash(const char* path, YAML::Node& meta) const
{
    uint64_t file_hash = FileSystem::HashFromPath(path);
    meta[ASSET_HASH] = file_hash;
}