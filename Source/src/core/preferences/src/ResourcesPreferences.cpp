#include "core/hepch.h"
#include "ResourcesPreferences.h"

#include <cassert>

using namespace Hachiko;

ResourcesPreferences::ResourcesPreferences()
    : Preferences(Type::RESOURCES)
{
    group_name = RESOURCES_NODE;
}

void ResourcesPreferences::LoadConfigurationData(const YAML::Node& node)
{
    for (auto it = node.begin(); it != node.end(); ++it)
    {
        // Scene
        if (it->first.as<std::string>()._Equal(SCENE_NAME))
        {
            scene_name = std::move(it->second.as<std::string>());
            continue;
        }

        if (it->first.as<std::string>()._Equal(SCENE_ID))
        {
            scene_id = std::move(it->second.as<std::string>());
            continue;
        }

        // Assets path
        if (it->first.as<std::string>()._Equal(SCENE_ASSETS))
        {
            scenes_assets = std::move(it->second.as<std::string>());
            continue;
        }

        if (it->first.as<std::string>()._Equal(MODELS_ASSETS))
        {
            models_assets = std::move(it->second.as<std::string>());
            continue;
        }

        if (it->first.as<std::string>()._Equal(MESHES_ASSETS))
        {
            meshes_assets = std::move(it->second.as<std::string>());
            continue;
        }

        if (it->first.as<std::string>()._Equal(TEXTURES_ASSETS))
        {
            textures_assets = std::move(it->second.as<std::string>());
            continue;
        }

        if (it->first.as<std::string>()._Equal(AUDIO_ASSETS))
        {
            audio_assets = std::move(it->second.as<std::string>());
            continue;
        }

        if (it->first.as<std::string>()._Equal(VIDEO_ASSETS))
        {
            video_assets = std::move(it->second.as<std::string>());
            continue;
        }

        if (it->first.as<std::string>()._Equal(SCRIPTS_ASSETS))
        {
            script_assets = std::move(it->second.as<std::string>());
            continue;
        }

        // Library path
        if (it->first.as<std::string>()._Equal(SCENES_LIBRARY))
        {
            scenes_library = std::move(it->second.as<std::string>());
            continue;
        }

        if (it->first.as<std::string>()._Equal(MODELS_LIBRARY))
        {
            models_library = std::move(it->second.as<std::string>());
            continue;
        }

        if (it->first.as<std::string>()._Equal(MESHES_LIBRARY))
        {
            meshes_library = std::move(it->second.as<std::string>());
            continue;
        }

        if (it->first.as<std::string>()._Equal(TEXTURES_LIBRARY))
        {
            textures_library = std::move(it->second.as<std::string>());
            continue;
        }

        if (it->first.as<std::string>()._Equal(AUDIO_LIBRARY))
        {
            audio_library = std::move(it->second.as<std::string>());
            continue;
        }

        if (it->first.as<std::string>()._Equal(VIDEO_LIBRARY))
        {
            video_library = std::move(it->second.as<std::string>());
            continue;
        }

        if (it->first.as<std::string>()._Equal(SCRIPTS_LIBRARY))
        {
            script_library = std::move(it->second.as<std::string>());
        }
    }
}

void ResourcesPreferences::SaveConfigurationData(YAML::Node& node)
{
    node[group_name][SCENE_NAME] = scene_name;
    node[group_name][SCENE_ID] = scene_id;
    node[group_name][SCENE_ASSETS] = scenes_assets;
    node[group_name][MODELS_ASSETS] = models_assets;
    node[group_name][MESHES_ASSETS] = meshes_assets;
    node[group_name][TEXTURES_ASSETS] = textures_assets;
    node[group_name][AUDIO_ASSETS] = audio_assets;
    node[group_name][VIDEO_ASSETS] = video_assets;
    node[group_name][SCRIPTS_ASSETS] = script_assets;
    node[group_name][MATERIALS_ASSETS] = materials_assets;
    node[group_name][SHADERS_ASSETS] = shaders_assets;
    node[group_name][ANIMATIONS_ASSETS] = animations_assets;
    node[group_name][SKYBOX_ASSETS] = skybox_assets;

    node[group_name][SCENES_LIBRARY] = scenes_library;
    node[group_name][MODELS_LIBRARY] = models_library;
    node[group_name][MESHES_LIBRARY] = meshes_library;
    node[group_name][TEXTURES_LIBRARY] = textures_library;
    node[group_name][AUDIO_LIBRARY] = audio_library;
    node[group_name][VIDEO_LIBRARY] = video_library;
    node[group_name][SCRIPTS_LIBRARY] = script_library;
    node[group_name][MATERIALS_LIBRARY] = materials_library;
    node[group_name][SHADERS_LIBRARY] = shaders_library;
    node[group_name][ANIMATIONS_LIBRARY] = animations_library;
    node[group_name][SKYBOX_LIBRARY] = skybox_library;
}

const char* ResourcesPreferences::GetAssetsPath(Resource::Type type)
{
    switch (type)
    {
    case Resource::Type::SCENE:
        return scenes_assets.c_str();
    case Resource::Type::MODEL:
        return models_assets.c_str();
    case Resource::Type::MESH:
        return meshes_assets.c_str();
    case Resource::Type::TEXTURE:
        return textures_assets.c_str();
    case Resource::Type::AUDIO:
        return audio_assets.c_str();
    case Resource::Type::VIDEO:
        return video_assets.c_str();
    case Resource::Type::SCRIPT:
        return script_assets.c_str();
    case Resource::Type::MATERIAL:
        return materials_assets.c_str();
    case Resource::Type::SHADER:
        return shaders_assets.c_str();
    case Resource::Type::ANIMATION:
        return animations_assets.c_str();
    case Resource::Type::SKYBOX:
        return skybox_assets.c_str();
    case Resource::Type::UNKNOWN:
    default:
        assert(false);
    }
}

const char* ResourcesPreferences::GetLibraryPath(Resource::Type type) const
{
    switch (type)
    {
    case Resource::Type::SCENE:
        return scenes_library.c_str();
    case Resource::Type::MODEL:
        return models_library.c_str();
    case Resource::Type::MESH:
        return meshes_library.c_str();
    case Resource::Type::TEXTURE:
        return textures_library.c_str();
    case Resource::Type::AUDIO:
        return audio_library.c_str();
    case Resource::Type::VIDEO:
        return video_library.c_str();
    case Resource::Type::SCRIPT:
        return script_library.c_str();
    case Resource::Type::MATERIAL:
        return materials_library.c_str();
    case Resource::Type::SHADER:
        return shaders_library.c_str();
    case Resource::Type::ANIMATION:
        return animations_library.c_str();
    case Resource::Type::SKYBOX:
        return skybox_library.c_str();
    case Resource::Type::UNKNOWN:
    default:
        assert(false);
    }
}
