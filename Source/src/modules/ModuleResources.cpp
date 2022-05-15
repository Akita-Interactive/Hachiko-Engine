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

#include "components/ComponentMeshRenderer.h"
#include "resources/ResourceMaterial.h"
#include "resources/ResourceTexture.h"

using namespace Hachiko;

bool ModuleResources::Init()
{
    preferences = App->preferences->GetResourcesPreference();

    // create assets & library directory tree
    for (int i = 1; i < static_cast<int>(Resource::Type::COUNT); ++i)
    {
        FileSystem::CreateDir(preferences->GetLibraryPath(static_cast<Resource::Type>(i)));
        
    }

    for (int i = 1; i < static_cast<int>(Resource::AssetType::COUNT); ++i)
    {
        FileSystem::CreateDir(preferences->GetAssetsPath(static_cast<Resource::AssetType>(i)));
    }

    AssetsLibraryCheck();

    std::function handleAddedFile = [&](Event& evt)
    {
        const auto& e = evt.GetEventData<FileAddedEventPayload>();
        std::filesystem::path file = e.GetPath();
        HE_LOG("Handling dropped file: %s", file.string().c_str());
        ImportAssetFromAnyPath(file);
    };
    App->event->Subscribe(Event::Type::FILE_ADDED, handleAddedFile);
    return true;
}

bool ModuleResources::CleanUp()
{
    for (auto& it : loaded_resources)
    {
        RELEASE(it.second);
    }

    return true;
}

std::filesystem::path ModuleResources::GetLastResourceLoadedPath() const
{
    return last_resource_path.parent_path();
}

std::vector<UID> ModuleResources::ImportAssetFromAnyPath(const std::filesystem::path& path)
{
    Resource::AssetType type = GetAssetTypeFromPath(path);
    
    if (type == Resource::AssetType::UNKNOWN)
    {
        HE_LOG("Unknown resource type recevied, nothing to be done");
        return std::vector<UID>();
    }

    size_t relative_pos = path.string().find("assets\\");
    bool file_in_asset = relative_pos != std::string::npos;
    std::filesystem::path destination;

    if (file_in_asset)
    {
        HE_LOG("File in assets folder");
        destination = std::filesystem::relative(path, path.string().substr(0, relative_pos));
    }
    else
    {
        HE_LOG("File outside the assets folder");
        destination = preferences->GetAssetsPath(type);
        destination.append(path.filename().c_str());
        FileSystem::Copy(path.string().c_str(), destination.string().c_str(), true);
    }

    // Handle asset when it is in correct path
    ImportAsset(destination.string());
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
        return it->second;
    }
    // If not loaded try to load and return
    auto res = importer_manager.LoadResource(type, id);
    if (res != nullptr)
    {
        return loaded_resources.emplace(id, res).first->second;
    }

    return nullptr;
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
                Importer* importer = importer_manager.GetAssetImporter(Resource::AssetType::PREFAB);
                importer->Load(meta_node[PREFAB_ID].as<UID>());
                break;
            }
            
        case Resource::AssetType::PREFAB:
            {
                Importer* importer = importer_manager.GetAssetImporter(Resource::AssetType::PREFAB);
                // Prefab asset only contains a single prefab resource
                importer->Load(meta_node[RESOURCES][0].as<UID>());
                break;
            }
        case Resource::AssetType::MATERIAL:
            {
                Scene* scene = App->scene_manager->GetActiveScene();
                // Material asset only contains a single material resource
                auto material_res = static_cast<ResourceMaterial*>(App->resources->GetResource(Resource::Type::MATERIAL, meta_node[RESOURCES][0].as<UID>()));
                scene->HandleInputMaterial(material_res);
                break;
            }            
        }
    }
}

void Hachiko::ModuleResources::AssetsLibraryCheck()
{
    HE_LOG("Assets/Library check...");

    // Ignores files that dont need any short of importing themselves
    // Metas are searched for based on what's on assets
    std::vector<std::string> ignore_extensions {"meta", "scene"};
    PathNode assets_folder = FileSystem::GetAllFiles(ASSETS_FOLDER, nullptr, &ignore_extensions);
    GenerateLibrary(assets_folder);

    HE_LOG("Assets/Library check finished.");
}

void Hachiko::ModuleResources::GenerateLibrary(const PathNode& folder) 
{
    // Iterate all files found in assets except metas and scene
    for (PathNode path_node : folder.children)
    {
        if (!path_node.isFile)
        {
            GenerateLibrary(path_node);
            continue;
        }
        ImportAsset(path_node.path);
    }
}

std::vector<UID> Hachiko::ModuleResources::ImportAsset(const std::string& asset_path)
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
        return ImportAssetResources(std::filesystem::path(asset_path), meta_node);
    }

    // If it has meta align on library and refresh if necessary
    YAML::Node meta_node = YAML::LoadFile(meta_path);

    // Do validations over meta information
    uint64_t previous_asset_hash = meta_node[ASSET_HASH].IsDefined() ? meta_node[ASSET_HASH].as<uint64_t>() : 0;
    uint64_t asset_hash = FileSystem::HashFromPath(asset_path.c_str());

    // Get the asset hash and reimport if it has changed
    if (previous_asset_hash != asset_hash)
    {
        UpdateAssetHash(asset_path.c_str(), meta_node);
        return ImportAssetResources(std::filesystem::path(asset_path), meta_node);
    }
    
    // Reimport if any lib file is missing
    bool valid_lib = ValidateAssetResources(meta_node);
    if (!valid_lib)
    {
        return ImportAssetResources(std::filesystem::path(asset_path), meta_node);
    }
    // If it exists and has a valid lib do nothing
}

std::vector<UID> ModuleResources::ImportAssetResources(const std::filesystem::path& asset_path, YAML::Node& meta)
{
    // If meta exists, import resources from the asset resources list
    // If the meta resources list is empty create all the resources it can from that asset
    Resource::AssetType asset_type = GetAssetTypeFromPath(asset_path);
    // Returns all the generated resources uids
    return importer_manager.ImportAsset(asset_path.string(), asset_type, meta);
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
        if (!FileSystem::Exists(library_path.c_str())) return false;
    }
    return true;
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