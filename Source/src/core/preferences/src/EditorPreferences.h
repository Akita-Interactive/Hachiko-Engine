#pragma once

#include "core/preferences/Preferences.h"
#include "ui/editor/Theme.h"

namespace Hachiko
{
    class EditorPreferences final : public Preferences
    {
    public:
        EditorPreferences();
        ~EditorPreferences() override = default;
        void LoadConfigurationData(const YAML::Node& node) override;
        void SaveConfigurationData(YAML::Node& node) override;

        void SetDisplayDebugDraw(const bool displayDebugDraw)
        {
            display_debug_draw = displayDebugDraw;
        }

        [[nodiscard]] bool GetDisplayDebugDraw() const
        {
            return display_debug_draw;
        }

        [[nodiscard]] float GetMaxFPS() const
        {
            return max_fps;
        }

        void SetMaxFPS(const float maxFps)
        {
            max_fps = std::max(24.0f, maxFps);
            max_fps = std::min(max_fps, 250.0f);
            fps_threshold = 1000.0f / max_fps;
        }

        [[nodiscard]] float GetFPSThreshold() const
        {
            return fps_threshold;
        }

        [[nodiscard]] bool IsFullscreen() const
        {
            return fullscreen;
        }

        void SetResizable(const bool val)
        {
            resizable = val;
        }

        [[nodiscard]] bool IsResizable() const
        {
            return resizable;
        }

        void SetFullscreen(const bool fullscreen)
        {
            this->fullscreen = fullscreen;
        }

        void SetVsync(const bool val)
        {
            vsync = val;
        }

        [[nodiscard]] bool IsVsyncActive() const
        {
            return vsync;
        }

        void SetDisplayCameraSettings(const bool displayCameraSettings)
        {
            display_camera_settings = displayCameraSettings;
        }

        [[nodiscard]] bool GetDisplayCameraSettings() const
        {
            return display_camera_settings;
        }

        void SetTheme(const Editor::Theme::Type theme)
        {
            this->theme = theme;
        }

        [[nodiscard]] Editor::Theme::Type GetTheme() const
        {
            return theme;
        }

        void SetAutosave(bool value)
        {
            scene_autosave = value;
        }

        [[nodiscard]] bool GetAutosave()
        {
            return scene_autosave;
        }

        void SetDrawSkybox(bool value)
        {
            draw_skybox = value;
        }

        [[nodiscard]] bool GetDrawSkybox()
        {
            return draw_skybox;
        }

        void SetDrawNavmesh(bool value)
        {
            draw_navmesh = value;
        }

        [[nodiscard]] bool GetDrawNavmesh()
        {
            return draw_navmesh;
        }

        void SetDrawQuadtree(bool value)
        {
            draw_quadtree = value;
        }

        [[nodiscard]] bool GetDrawQuadtree()
        {
            return draw_quadtree;
        }

        void SetUndoRedoActive(bool value)
        {
            undo_redo_active = value;
        }

        [[nodiscard]] bool GetUndoRedoActive() const
        {
            return undo_redo_active;
        }

        void SetShadowMappingGaussianBlurringEnabled(bool enabled) 
        {
            shadow_mapping_gaussian_blurring_enabled = enabled;
        }

        [[nodiscard]] bool GetShadowMapGaussianBlurringEnabled() const 
        {
            return shadow_mapping_gaussian_blurring_enabled;
        }

        [[nodiscard]] bool GetShadowPassEnabled() const 
        {
            return shadow_pass_enabled;
        }

        void SetShadowPassEnabled(bool enabled) 
        {
            shadow_pass_enabled = enabled;
        }

        [[nodiscard]] YAML::Node GetLoadingScreenConfig() const
        {
            return loading_screen_config;
        }

        void SetLoadingScreenConfig(YAML::Node& node)
        {
            loading_screen_config = node;
        }

        [[nodiscard]] bool GetSSAOEnabled() const
        {
            return ssao_enabled;
        }

        void SetSSAOEnabled(bool enabled)
        {
            ssao_enabled = enabled;
        }

    private:
        bool display_debug_draw = false;

        //TODO remove this property in favor of the camera component
        bool display_camera_settings = false;

        float max_fps = 250.0f;
        bool fullscreen = false;
        bool scene_autosave = false;
        bool vsync = true;
        bool resizable = true;
        bool draw_skybox = true;
        bool draw_navmesh = false;
        bool draw_quadtree = false;
        bool shadow_mapping_gaussian_blurring_enabled = true;
        bool shadow_pass_enabled = true;
        bool ssao_enabled = true;
        float fps_threshold = 1000.0f / max_fps;
        Editor::Theme::Type theme = Editor::Theme::Type::DARK;
        bool undo_redo_active = true;

        YAML::Node loading_screen_config;
    };
}
