#include "core/hepch.h"
#include "EditorPreferences.h"

#include "Math/float3.h"

#include "Application.h"
#include "modules/ModuleRender.h"
#include "core/rendering/BloomManager.h"

using namespace Hachiko;

EditorPreferences::EditorPreferences()
    : Preferences(Type::EDITOR)
{
    group_name = EDITOR_NODE;
}

void EditorPreferences::LoadConfigurationData(const YAML::Node& node)
{
    // TODO: This is inefficient a.f, convert this to be non loop:
    for (auto it = node.begin(); it != node.end(); ++it)
    {
        if (it->first.as<std::string>()._Equal(DISPLAY_CAMERA_SETTINGS))
        {
            display_camera_settings = it->second.as<bool>();
            continue;
        }

        if (it->first.as<std::string>()._Equal(FULLSCREEN_NODE))
        {
            fullscreen = it->second.as<bool>();
            continue;
        }

        if (it->first.as<std::string>()._Equal(THEME))
        {
            theme = Editor::Theme::FromString(it->second.as<std::string>());
            continue;
        }

        if (it->first.as<std::string>()._Equal(VSYNC))
        {
            vsync = it->second.as<bool>();
        }

        if (it->first.as<std::string>()._Equal(SCENE_AUTOSAVE))
        {
            scene_autosave = it->second.as<bool>();
        }

        if (it->first.as<std::string>()._Equal(RESIZABLE))
        {
            resizable = it->second.as<bool>();
        }

        if (it->first.as<std::string>()._Equal(DISPLAY_DEBUG_DRAW))
        {
            display_debug_draw = it->second.as<bool>();
            continue;
        }

        if (it->first.as<std::string>()._Equal(DISPLAY_SKYBOX))
        {
            draw_skybox = it->second.as<bool>();
        }

        if (it->first.as<std::string>()._Equal(DISPLAY_QUADTREE))
        {
            draw_quadtree = it->second.as<bool>();
        }

        if (it->first.as<std::string>()._Equal(DISPLAY_NAVMESH))
        {
            draw_navmesh = it->second.as<bool>();
        }

        if (it->first.as<std::string>()._Equal(UNDO_REDO_ACTIVE))
        {
            undo_redo_active = it->second.as<bool>();
        }

        if (it->first.as<std::string>()._Equal(SHADOW_MAP_GAUSSIAN_BLURRING_ENABLED))
        {
            shadow_mapping_gaussian_blurring_enabled = it->second.as<bool>();
        }

        if (it->first.as<std::string>()._Equal(SHADOW_PASS_ENABLED))
        {
            shadow_pass_enabled = it->second.as<bool>();
        }
    }

    App->renderer->GetBloomManager().LoadConfig(node);

    if (node[LOADING_SCREEN_CONFIG].IsDefined())
    {
        loading_screen_config = node[LOADING_SCREEN_CONFIG];
    }

    if (node[SSAO_ENABLED].IsDefined())
    {
        ssao_enabled = node[SSAO_ENABLED].as<bool>();
    }

    App->renderer->GetSSAOManager().LoadConfig(node);
}

void EditorPreferences::SaveConfigurationData(YAML::Node& node)
{
    node[group_name][DISPLAY_CAMERA_SETTINGS] = display_camera_settings;
    node[group_name][FULLSCREEN_NODE] = fullscreen;
    node[group_name][THEME] = Editor::Theme::ToString(theme);
    node[group_name][VSYNC] = vsync;
    node[group_name][SCENE_AUTOSAVE] = scene_autosave;
    node[group_name][RESIZABLE] = resizable;
    node[group_name][DISPLAY_DEBUG_DRAW] = display_debug_draw;
    node[group_name][DISPLAY_SKYBOX] = draw_skybox;
    node[group_name][DISPLAY_QUADTREE] = draw_quadtree;
    node[group_name][DISPLAY_NAVMESH] = draw_navmesh;
    node[group_name][UNDO_REDO_ACTIVE] = undo_redo_active;
    node[group_name][SHADOW_MAP_GAUSSIAN_BLURRING_ENABLED] = shadow_mapping_gaussian_blurring_enabled;
    node[group_name][SHADOW_PASS_ENABLED] = shadow_pass_enabled;

    App->renderer->GetBloomManager().SaveConfig(node[group_name]);
    App->renderer->GetSSAOManager().SaveConfig(node[group_name]);

    node[group_name][LOADING_SCREEN_CONFIG] = loading_screen_config;
}
