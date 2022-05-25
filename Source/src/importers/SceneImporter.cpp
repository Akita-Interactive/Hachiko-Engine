#include "core/hepch.h"

#include "SceneImporter.h"
#include "resources/ResourceScene.h"
#include "modules/ModuleSceneManager.h"
#include "modules/ModuleResources.h"
#include "NavmeshImporter.h"

void Hachiko::SceneImporter::Import(const char* path, YAML::Node& meta)
{
    // Only 1 scene per scene asset will exist
    static const int scene_resource_index = 0;
    static const int navmesh_resource_index = 1;
    UID scene_uid = ManageResourceUID(Resource::Type::SCENE, scene_resource_index, meta);    
    FileSystem::Copy(path, GetResourcePath(Resource::Type::SCENE, scene_uid).c_str());

    YAML::Node node = YAML::LoadFile(path);
    if (!node[NAVMESH_ID].IsDefined())
    {
        node[NAVMESH_ID] = UUID::GenerateUID();
    }
    UID navmesh_id = node[NAVMESH_ID].as<UID>(); 
    SetResource(navmesh_id, Resource::Type::NAVMESH, navmesh_resource_index, meta);
    NavmeshImporter navmesh_importer;

    Scene* temp_scene = new Scene();
    // Cant load a scene that is not imported yet
    const ResourceScene* scene = static_cast<const ResourceScene*>(Load(scene_uid));
    constexpr bool meshes_only = true;
    temp_scene->Load(scene->scene_data, meshes_only);
    navmesh_importer.CreateNavmeshFromScene(temp_scene, navmesh_id);
    delete temp_scene;
    delete scene;
}

void Hachiko::SceneImporter::Save(UID id, const Resource* resource)
{
    // Save both asset and resource since it is an asset we can change from editor
    const ResourceScene* scene = static_cast<const ResourceScene*>(resource);

    const std::string resource_path = GetResourcePath(Resource::Type::SCENE, id);
    const std::string scene_asset_path = StringUtils::Concat(GetResourcesPreferences()->GetAssetsPath(Resource::AssetType::SCENE), scene->name, SCENE_EXTENSION);
    FileSystem::Save(resource_path.c_str(), scene->scene_data);
    FileSystem::Save(scene_asset_path.c_str(), scene->scene_data);
}

Hachiko::Resource* Hachiko::SceneImporter::Load(UID id)
{
    // TODO: Make it go through the normal resource load pipeline and free afterwards
    const std::string scene_resource_path = GetResourcePath(Resource::Type::SCENE, id);
    if (!FileSystem::Exists(scene_resource_path.c_str()))
    {
        return nullptr;
    }
    YAML::Node node = YAML::LoadFile(scene_resource_path);    
    ResourceScene* scene_resource = new ResourceScene(id);
    scene_resource->scene_data = node;
    scene_resource->name = node[SCENE_NAME].as<std::string>();
    // We dont use scene resource outside of load
    return scene_resource;
}

Hachiko::UID Hachiko::SceneImporter::CreateSceneAsset(Scene* scene)
{
    // This id wont be used
    const std::string scene_asset_path = StringUtils::Concat(GetResourcesPreferences()->GetAssetsPath(Resource::AssetType::SCENE), scene->GetName(), SCENE_EXTENSION);
    ResourceScene* scene_resource = CreateSceneResource(scene);

    FileSystem::Save(scene_asset_path.c_str(), scene_resource->scene_data);

    // Note make sure the first uid is always the one from scene and not navmesh
    UID scene_uid = App->resources->ImportAssetFromAnyPath(scene_asset_path)[0];
    delete scene_resource;
    return scene_uid;
}

Hachiko::ResourceScene* Hachiko::SceneImporter::CreateSceneResource(Scene* scene)
{
    ResourceScene* scene_resource = new ResourceScene(0);
    
    RefreshSceneResource(scene_resource, scene);
    return scene_resource;
}

void Hachiko::SceneImporter::RefreshSceneResource(ResourceScene* resource, Scene* scene)
{
    resource->name = scene->GetName();

    YAML::Node scene_data;
    std::string output_path;
    scene->Save(scene_data);
    resource->scene_data = scene_data;
}
