#pragma once

#include "core/preferences/Preferences.h"

namespace Hachiko
{
    class RenderPreferences final : public Preferences
    {
    public:
        RenderPreferences();
        ~RenderPreferences() override = default;
        void LoadConfigurationData(const YAML::Node& node) override;
        void SaveConfigurationData(YAML::Node& node) override;

    private:
        float max_fps = 250.0;
        float fps_threshold = 1000.0f / max_fps;
    };
}
