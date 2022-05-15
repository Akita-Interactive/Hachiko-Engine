#pragma once

#include "core/GameObject.h"

#include "ui/Window.h"
#include "ui/WindowAbout.h"
#include "ui/WindowConfiguration.h"
#include "ui/WindowHierarchy.h"
#include "ui/WindowInspector.h"
#include "ui/WindowScene.h"
#include "ui/WindowResource.h"
#include "ui/WindowConsole.h"
#include "ui/WindowTimers.h"
#include "ui/WindowProject.h"

#include "ModuleRender.h"

#include "ui/editor/Theme.h"

namespace Hachiko
{
    class ModuleEditor : public Module
    {
    public:
        ModuleEditor();

        bool Init() override;
        UpdateStatus PreUpdate(float delta) override;
        UpdateStatus Update(float delta) override;
        UpdateStatus PostUpdate(const float delta) override;
        bool CleanUp() override;

        void UpdateTheme() const;

        //Edit actions
        static bool CanUndo()
        {
            return false;
        }

        static bool CanRedo()
        {
            return false;
        }

        static bool CanPaste()
        {
            return false;
        }

        [[nodiscard]] GameObject* GetSelectedGameObject() const
        {
            return selected_go;
        }

        void SetSelectedGO(GameObject* const selected_game_object)
        {
            selected_go = selected_game_object;
        }

        [[nodiscard]] const WindowScene* GetSceneWindow() const
        {
            return &w_scene;
        }

        [[nodiscard]] Editor::Theme::Type GetTheme() const
        {
            return theme;
        }

        void SetTheme(const Editor::Theme::Type theme)
        {
            this->theme = theme;
        }

        ImFont* m_big_icon_font = nullptr;
        ImFont* m_small_icon_font = nullptr;

        // TODO: Save the info imgui.ini
        unsigned dock_up_id = 0;
        unsigned dock_main_id = 0;
        unsigned dock_left_id = 0;
        unsigned dock_right_id = 0;
        unsigned dock_down_id = 0;

        mutable float4 scene_background{0.1f, 0.1f, 0.1f, 0.1f};

    private:
        void GenerateDockingSpace();

        // Main menu bar
        UpdateStatus MainMenuBar();
        void FileMenu() const;
        void EditMenu() const;
        void GoMenu() const;
        void ViewMenu();
        void ThemeMenu() const;

        GameObject* selected_go = nullptr;

        std::vector<Window*> windows;

        WindowHierarchy w_hierarchy;
        WindowScene w_scene;
        WindowInspector w_inspector;
        WindowConfiguration w_configuration;
        WindowAbout w_about;
        WindowConsole w_console;
        WindowResource w_resource;
        WindowTimers w_timers;
        WindowProject w_project;

        Editor::Theme::Type theme = Editor::Theme::Type::DARK;
    };
}
