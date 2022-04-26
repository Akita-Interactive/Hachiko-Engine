#pragma once

#include "Preferences.h"

namespace Hachiko
{
    class GlobalPreferences;
    class EditorPreferences;
    class RenderPreferences;
    class CameraPreferences;
    class ResourcesPreferences;

    class PreferenceManager
    {
    public:
        PreferenceManager(const char* cfg);
        ~PreferenceManager();
        void LoadConfigurationFile() const;
        void SaveConfigurationFile() const;
        [[nodiscard]] Preferences* GetPreference(Preferences::Type type) const;
        [[nodiscard]] ResourcesPreferences* GetResourcesPreference() const
        {
            return resources;
        }

        [[nodiscard]] EditorPreferences* GetEditorPreference() const
        {
            return editor;
        }

        [[nodiscard]] CameraPreferences* GetCameraPreference() const
        {
            return camera;
        }

    private:
        std::vector<Preferences*> preferences{};
        std::vector<YAML::Node> nodes_vec{};
        std::string config_file;

        GlobalPreferences* globals = nullptr;
        EditorPreferences* editor = nullptr;
        RenderPreferences* render = nullptr;
        CameraPreferences* camera = nullptr;
        ResourcesPreferences* resources = nullptr;
    };
}
