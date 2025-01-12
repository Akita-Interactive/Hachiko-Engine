#pragma once

#include "Importer.h"

namespace Hachiko
{
    class ResourcePrefab;
    class GameObject;

    class PrefabImporter : public Importer
    {
    public:
        PrefabImporter() = default;
        // Import already existing prefab asset 
        // We also generate prefabs from model and it doesnt go through this import call
        void Import(const char* path, YAML::Node& meta) override;
        void Save(UID id, const Resource* prefab_data) override;
        Resource* Load(UID id) override;
        // Root will be replaced by scene root on load, should be empy
        // Prefabs are like scene subsets but without storing uids
        UID CreatePrefabAsset(const char* name, GameObject* root);
        // To be used by model importer
        UID CreateGeneratedPrefab(const char* name, UID uid, GameObject* root);
        std::vector<GameObject*> CreateObjectsFromPrefab(UID prefab_uid, GameObject* parent, unsigned n_instances);

    private:
        ResourcePrefab* CreatePrefabResouce(const char* name, UID uid, GameObject* root);
        void SaveScriptReferences(ResourcePrefab* prefab, GameObject* root);
        void LoadScriptReferences(ResourcePrefab* prefab, GameObject* root);
    };
} // namespace Hachiko
