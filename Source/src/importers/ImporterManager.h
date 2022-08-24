#pragma once

#include "importers/Importer.h"

#include "MeshImporter.h"
#include "ModelImporter.h"
#include "TextureImporter.h"
#include "MaterialImporter.h"
#include "AnimationImporter.h"
#include "FontImporter.h"
#include "PrefabImporter.h"
#include "SceneImporter.h"
#include "NavmeshImporter.h"
#include "SkyboxImporter.h"
#include "StateMachineImporter.h"
#include "VideoImporter.h"

namespace Hachiko
{

    class ImporterManager
    {
        friend class ModuleResources;
    public:
        ~ImporterManager() = default;

    private:
        ImporterManager();

        std::map<Resource::AssetType, Importer*> asset_importers;
        std::map<Resource::Type, Importer*> resource_importers;
        
        // If any id is defined it will be assigned to the resource, used to keep meta id but regenerate it
        std::vector<UID> ImportAsset(const std::string& asset_path, Resource::AssetType asset_type, YAML::Node& meta);
        Resource* LoadResource(Resource::Type type, UID id);
        void DeleteResource(UID uid, Resource::Type resource_type) const;

        Importer* GetAssetImporter(Resource::AssetType type) const;
        Importer* GetResourceImporter(Resource::Type type) const;

        void SaveResource(UID uid, const Resource* resource) const;

        // They exist both as assets and as resources
        MaterialImporter material;
        TextureImporter texture;
        FontImporter font;
        PrefabImporter prefab;
        SceneImporter scene;
        SkyboxImporter skybox;
        StateMachineImporter state_machine;
        VideoImporter video;

        // It doesnt have its own resource type (we use prefabs)
        ModelImporter model;

        // They dont have their own asset type
        AnimationImporter animation;
        MeshImporter mesh;
        NavmeshImporter navmesh;
    };
} // namespace Hachiko
