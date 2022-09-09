#include "core/hepch.h"

#include "ImporterManager.h"

using namespace Hachiko;

ImporterManager::ImporterManager()
{

    // Importers used to generate resources from asset types
    asset_importers.emplace(Resource::AssetType::TEXTURE, &texture);
    asset_importers.emplace(Resource::AssetType::MATERIAL, &material);
    asset_importers.emplace(Resource::AssetType::FONT, &font);
    asset_importers.emplace(Resource::AssetType::PREFAB, &prefab);
    asset_importers.emplace(Resource::AssetType::MODEL, &model);
    asset_importers.emplace(Resource::AssetType::SCENE, &scene);
    asset_importers.emplace(Resource::AssetType::SKYBOX, &skybox);
    asset_importers.emplace(Resource::AssetType::STATE_MACHINE, &state_machine);
    asset_importers.emplace(Resource::AssetType::VIDEO, &video);


    // Importers used to save and load the different resource types
    resource_importers.emplace(Resource::Type::TEXTURE, &texture);
    resource_importers.emplace(Resource::Type::MATERIAL, &material);
    resource_importers.emplace(Resource::Type::FONT, &font);
    resource_importers.emplace(Resource::Type::PREFAB, &prefab);
    resource_importers.emplace(Resource::Type::MESH, &mesh);
    resource_importers.emplace(Resource::Type::ANIMATION, &animation);
    resource_importers.emplace(Resource::Type::SCENE, &scene);
    resource_importers.emplace(Resource::Type::NAVMESH, &navmesh);
    resource_importers.emplace(Resource::Type::SKYBOX, &skybox);
    resource_importers.emplace(Resource::Type::STATE_MACHINE, &state_machine);
    resource_importers.emplace(Resource::Type::VIDEO, &video);
}

std::vector<UID> ImporterManager::ImportAsset(const std::string& asset_path, const Resource::AssetType asset_type, YAML::Node& meta)
{
    // Find the corresponding importer for an asset, delegate import to it and them store the resulting meta
    Importer* importer = GetAssetImporter(asset_type);

    if (!importer)
    {
        HE_LOG("No valid importer found for %s", asset_path.c_str());
        return std::vector<UID>();
    }

    importer->Import(asset_path.c_str(), meta);

    std::vector<UID> generated_resource_ids;
    
    for (unsigned i = 0; i < meta[RESOURCES].size(); ++i)
    {
        generated_resource_ids.push_back(meta[RESOURCES][i][RESOURCE_ID].as<UID>());
    }
    return generated_resource_ids;
}

Resource* Hachiko::ImporterManager::LoadResource(Resource::Type type, UID id)
{
    return GetResourceImporter(type)->Load(id);
}

void Hachiko::ImporterManager::DeleteResource(UID uid, Resource::Type resource_type) const
{
    GetResourceImporter(resource_type)->Delete(uid, resource_type);
}

Importer* Hachiko::ImporterManager::GetAssetImporter(Resource::AssetType type) const
{
    auto it = asset_importers.find(type);
    if (it != asset_importers.end())
    {
        return it->second;
    }
    return nullptr;
}

Importer* Hachiko::ImporterManager::GetResourceImporter(Resource::Type type) const
{
    auto it = resource_importers.find(type);
    if (it != resource_importers.end())
    {
        return it->second;
    }
    return nullptr;    
}

void Hachiko::ImporterManager::SaveResource(UID uid, const Resource* resource) const 
{
    Importer* importer = GetResourceImporter(resource->GetType());

    importer->Save(uid, resource);
}

