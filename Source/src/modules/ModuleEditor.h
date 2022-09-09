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
    class EditorPreferences;

    class ModuleEditor : public Module
    {
    public:
        ModuleEditor();

        bool Init() override;
        bool Start() override;
        UpdateStatus PreUpdate(float delta) override;
        UpdateStatus Update(float delta) override;
        UpdateStatus PostUpdate(const float delta) override;
        bool CleanUp() override;


        void UpdateTheme() const;

        //Edit actions
        bool CanUndo() const
        {
            return history.CanUndo();
        }

        bool CanRedo() const
        {
            return history.CanRedo();
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
            w_hierarchy.GOSelected(selected_game_object);

            Event evt(Event::Type::SELECTION_CHANGED);
            evt.SetEventData<SelectionChangedEventPayload>(selected_go);
            App->event->Publish(evt);
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
        Component* to_remove = nullptr;

        bool undo_redo_active = true;

    private:
        void GenerateDockingSpace();

        // Main menu bar
        void MainMenuBar();
        void FileMenu();
        void EditMenu();
        void GoMenu() const;
        void ViewMenu();
        void ThemeMenu() const;

        void CreateSnapshot();
        void Undo();
        void Redo();

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

        EditorPreferences* editor_prefs = nullptr;
        Editor::Theme::Type theme = Editor::Theme::Type::DARK;

        class History
        {
        public:
            Scene::Memento* Undo();
            Scene::Memento* Redo();
            void Save(Scene::Memento*);
            [[nodiscard]] bool CanUndo() const;
            [[nodiscard]] bool CanRedo() const;

            void Init();
            void CleanUp();

        private:
            size_t current_position = 0;
            std::vector<std::shared_ptr<Scene::Memento>> mementos {};
        };

        History history;
        bool save_as_popup = false;
        std::string file_name_buffer {""};
#ifndef PLAY_BUILD
        inline static bool history_enabled = true;
#else
        inline static bool history_enabled = false;
#endif
    };
}
