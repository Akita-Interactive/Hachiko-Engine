#pragma once

#include "resources/Resource.h"
#include "importers/ImporterManager.h"
#include "core/preferences/src/ResourcesPreferences.h"

namespace Hachiko
{
    class ResourceModel;
    class ResourceMesh;
    class ResourceMaterial;
    class ResourceTexture;
    class GameObject;

    class ModuleResources : public Module
    {

    public:
        bool Init() override;
        bool CleanUp() override;

        [[nodiscard]] std::filesystem::path GetLastResourceLoadedPath() const;
        Hachiko::Resource::Type GetType(const std::filesystem::path& file);
        
        ResourceModel* GetModel(const std::string& name);
        ResourceMesh* GetMesh(const UID uid, const std::string& model_path = std::string(), int mesh_index = -1);
        ResourceMaterial* GetMaterial(const std::string& material_name);
        ResourceTexture* GetTexture(const std::string& texture_name, const std::string& asset_path = std::string());

        void CreateResource(Resource::Type type, const std::string& name) const;


    private:
        std::map<std::string, ResourceModel*> models;
        std::map<std::string, ResourceMaterial*> materials;
        std::map<std::string, ResourceTexture*> textures;
        std::map<UID, ResourceMesh*> meshes;

        std::vector<std::pair<Hachiko::Resource::Type, std::string>> supported_extensions = 
        {{Hachiko::Resource::Type::TEXTURE, ".png"},
        {Hachiko::Resource::Type::TEXTURE, ".tif"},
        {Hachiko::Resource::Type::MODEL, ".fbx"},
        {Hachiko::Resource::Type::MODEL, ".obj"},
        {Hachiko::Resource::Type::SCENE, SCENE_EXTENSION},
        // Imported Resources
        {Hachiko::Resource::Type::MODEL, MODEL_EXTENSION},
        {Hachiko::Resource::Type::MATERIAL, MATERIAL_EXTENSION}};
        
        Hachiko::ResourcesPreferences* preferences = nullptr;
        Hachiko::ImporterManager importer_manager;
        std::filesystem::path last_resource_path;

        void ImportResource(const std::filesystem::path& asset, Hachiko::Resource::Type asset_type);
        void HandleResource(const std::filesystem::path& path);
    };
}
