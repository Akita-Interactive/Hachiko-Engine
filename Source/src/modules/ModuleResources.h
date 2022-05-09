#pragma once

#include "resources/Resource.h"
#include "importers/ImporterManager.h"
#include "core/preferences/src/ResourcesPreferences.h"

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
        
        void AssetsLibraryCheck();
        std::vector<UID> ImportAssetFromAnyPath(const std::filesystem::path& path);
        std::vector<UID> CreateAsset(Resource::Type type, const std::string& name) const;
        void LoadAsset(const std::string& path);

    private:
        std::map<UID, Resource*> loaded_resources{};

        std::vector<std::pair<Hachiko::Resource::AssetType, std::string>> supported_extensions = 
        {
            {Hachiko::Resource::AssetType::TEXTURE, ".png"},
            {Hachiko::Resource::AssetType::TEXTURE, ".jpg"},
            {Hachiko::Resource::AssetType::TEXTURE, ".tif"},
            {Hachiko::Resource::AssetType::MODEL, ".fbx"},
            {Hachiko::Resource::AssetType::SCENE, SCENE_EXTENSION},
            {Hachiko::Resource::AssetType::MATERIAL, MATERIAL_EXTENSION},
            {Hachiko::Resource::AssetType::FONT, ".ttf"},
            {Hachiko::Resource::AssetType::PREFAB, PREFAB_EXTENSION},
        };
        
        Hachiko::ResourcesPreferences* preferences = nullptr;
        Hachiko::ImporterManager importer_manager;
        std::filesystem::path last_resource_path;

        // Checks the current assets folder states and sets library to a valid state
        void GenerateLibrary(const PathNode& folder);
        // Gets an asset file and returns its related resources
        std::vector<UID> ImportAsset(const std::string& asset_path);
        // From an asset file creates its resources and sets the corresponding ids and types on meta
        // Keeps Previously existing ids if they can be matched
        std::vector<UID> ImportAssetResources(const std::filesystem::path& asset, YAML::Node& meta);
        // Checks if all the asset resource files exist in library
        bool ValidateAssetResources(const YAML::Node& meta) const;

    public:
        // Create a clean initial meta node
        YAML::Node CreateMeta();
        // Add asset hash value to any meta
        void UpdateAssetHash(const char* path, YAML::Node& meta) const;
    };
}
