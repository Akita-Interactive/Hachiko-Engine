#pragma once

#include "resources/Resource.h"
#include "importers/ImporterManager.h"
#include "core/preferences/src/ResourcesPreferences.h"
#include "utils/UUID.h"

namespace Hachiko
{
    class ResourceMesh;
    class ResourceMaterial;
    class ResourceTexture;
    class GameObject;

    class ModuleResources : public Module
    {

    public:
        bool Init() override;
        bool CleanUp() override;

        // TODO: Remove this should not exist
        [[nodiscard]] std::filesystem::path GetLastResourceLoadedPath() const;

        Hachiko::Resource::AssetType GetAssetTypeFromPath(const std::filesystem::path& file);

        Resource* GetResource(Resource::Type type, UID id);
        void ReleaseResource(Resource* resource);
        void ReleaseResource(UID id);
        
        void AssetsLibraryCheck();
        std::vector<UID> ImportAssetFromAnyPath(const std::filesystem::path& path, bool force = false);
        std::vector<UID> CreateAsset(Resource::Type type, const std::string& name) const;
        void LoadAsset(const std::string& path);
        std::vector<GameObject*> InstantiatePrefab(UID prefab_uid, GameObject* parent, unsigned n_instances);

    private:
        struct ResourceInstance
        {
            Resource* resource = nullptr;
            unsigned n_users = 0;
        };
        std::map<UID, ResourceInstance> loaded_resources;
        unsigned imported_assets = 0;
        unsigned cleaned_resources = 0;
        std::set<UID> managed_uids;

        std::vector<std::pair<Hachiko::Resource::AssetType, std::string>> supported_extensions = 
        {
            {Hachiko::Resource::AssetType::TEXTURE, PNG_TEXTURE_EXTENSION},
            {Hachiko::Resource::AssetType::TEXTURE, JPG_TEXTURE_EXTENSION},
            {Hachiko::Resource::AssetType::TEXTURE, TIF_TEXTURE_EXTENSION},
            {Hachiko::Resource::AssetType::MODEL, MODEL_EXTENSION},
            {Hachiko::Resource::AssetType::SCENE, SCENE_EXTENSION},
            {Hachiko::Resource::AssetType::MATERIAL, MATERIAL_EXTENSION},
            {Hachiko::Resource::AssetType::FONT, FONT_EXTENSION},
            {Hachiko::Resource::AssetType::PREFAB, PREFAB_EXTENSION},
            {Hachiko::Resource::AssetType::SKYBOX, SKYBOX_EXTENSION},
            {Hachiko::Resource::AssetType::STATE_MACHINE, STATE_MACHINE_EXTENSION},
            {Hachiko::Resource::AssetType::VIDEO, MP4_VIDEO_EXTENSION},
        };
        
        Hachiko::ResourcesPreferences* preferences = nullptr;
        Hachiko::ImporterManager importer_manager;
        std::filesystem::path last_resource_path;

        // Checks the current assets folder states and sets library to a valid state
        void GenerateLibrary(const PathNode& folder);
        // Gets an asset file and returns its related resources
        std::vector<UID> ImportAsset(const std::string& asset_path, bool force = false);
        // From an asset file creates its resources and sets the corresponding ids and types on meta
        // Keeps Previously existing ids if they can be matched
        std::vector<UID> ImportAssetResources(const std::string& asset_path, YAML::Node& meta);
        // Checks if all the asset resource files exist in library
        bool ValidateAssetResources(const YAML::Node& meta) const;

        // Removes UIDS not seen while importing from all library folders
        void ClearUnusedResources(const std::set<UID>& seen_uids);
        // Process Cleaning on a specific path node
        void ClearLibrary(const PathNode& folder, const std::set<UID>& seen_uids);


    public:
        // Create a clean initial meta node
        YAML::Node CreateMeta();
        // Add asset hash value to any meta
        void UpdateAssetHash(const char* path, YAML::Node& meta) const;
    };
}
