#include "core/hepch.h"
#include "EditorPreferences.h"

#include "Math/float3.h"

using namespace Hachiko;

EditorPreferences::EditorPreferences()
    : Preferences(Type::EDITOR)
{
    group_name = EDITOR_NODE;
}

void EditorPreferences::LoadConfigurationData(const YAML::Node& node)
{
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

        if (it->first.as<std::string>()._Equal(DISPLAY_DEBUG_DRAW))
        {
            display_debug_draw = it->second.as<bool>();
            continue;
        }

        if (it->first.as<std::string>()._Equal(THEME))
        {
            theme = Editor::Theme::FromString(it->second.as<std::string>());
            continue;
        }

        if (it->first.as<std::string>()._Equal(SCENE_BACKGROUND_COLOR))
        {
            scene_background_color = it->second.as<float3>();
            continue;
        }

        if (it->first.as<std::string>()._Equal(VSYNC))
        {
            vsync = it->second.as<int>();
        }

        if (it->first.as<std::string>()._Equal(SCENE_AUTOSAVE))
        {
            scene_autosave = it->second.as<bool>();
        }
    }
}

void EditorPreferences::SaveConfigurationData(YAML::Node& node)
{
    node[group_name][DISPLAY_CAMERA_SETTINGS] = display_camera_settings;
    node[group_name][FULLSCREEN_NODE] = fullscreen;
    node[group_name][DISPLAY_DEBUG_DRAW] = display_debug_draw;
    node[group_name][THEME] = Editor::Theme::ToString(theme);
    node[group_name][VSYNC] = vsync;
    node[group_name][SCENE_BACKGROUND_COLOR] = scene_background_color;
    node[group_name][SCENE_AUTOSAVE] = scene_autosave;
}
