#include "core/hepch.h"
#include "SceneSerializer.h"

#include "core/preferences/src/ResourcesPreferences.h"

Hachiko::Scene* Hachiko::SceneSerializer::Load(const char* path)
{
    YAML::Node scene_node = YAML::LoadFile(path);

    // Validate scene header
    if (!scene_node[SCENE_NAME].IsDefined())
    {
        return nullptr;
    }
    preferences = App->preferences->GetResourcesPreference();
    const auto scene_output = new Scene();
    
    if (!scene_node[CHILD_NODE].IsDefined())
    {
        return scene_output;
    }

    scene_output->Load(scene_node);

    return scene_output;
}

bool Hachiko::SceneSerializer::Save(const Scene* scene, const char* scene_path)
{
    YAML::Node scene_data;
    scene->Save(scene_data);
    std::ofstream fout;
    if (!scene_path)
    {
        fout.open(StringUtils::Concat(App->preferences->GetResourcesPreference()->GetAssetsPath(Resource::Type::SCENE), scene->GetName(), SCENE_EXTENSION));
    }
    else
    {
        fout.open(scene_path);
    }
    fout << scene_data;
    fout.close();
    return true;
}
