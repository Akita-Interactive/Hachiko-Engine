#include "core/hepch.h"

#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleSceneManager.h"
#include "ModuleEvent.h"
#include "ModuleProgram.h"
#include "core/preferences/src/EditorPreferences.h"
#include "ModuleResources.h"

Hachiko::ModuleEditor::ModuleEditor()
{
    HE_LOG("INITIALIZING MODULE: EDITOR");

    HE_LOG("Creating windows");

#ifndef PLAY_BUILD
    windows.push_back(&w_configuration);
    windows.push_back(&w_hierarchy);
    windows.push_back(&w_scene);
    windows.push_back(&w_inspector);
    windows.push_back(&w_about);
    windows.push_back(&w_console);
    windows.push_back(&w_resource);
    //windows.push_back(&w_project);
    windows.push_back(&w_timers);
#endif
}

void Hachiko::ModuleEditor::UpdateTheme() const
{
    if (theme == Editor::Theme::Type::LIGHT)
    {
        // UI style
        ImGuiStyle& style = ImGui::GetStyle();
        ImGui::StyleColorsLight();
        ImVec4* colors = style.Colors;
        colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 0.70f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 0.7f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 0.7f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.45f, 0.00f, 0.7f);
        scene_background = float4(0.9f, 0.9f, 0.9f, 1.0f);
    }
    else if (theme == Editor::Theme::Type::PINK)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImGui::StyleColorsLight();
        ImVec4* colors = style.Colors;
        colors[ImGuiCol_WindowBg] = ImColor(160, 160, 232);
        colors[ImGuiCol_TitleBgActive] = ImColor(232, 160, 160);
        colors[ImGuiCol_TitleBg] = ImColor(160, 232, 232);
        colors[ImGuiCol_MenuBarBg] = ImColor(217, 245, 217);
        colors[ImGuiCol_PopupBg] = ImColor(153, 225, 220);
        colors[ImGuiCol_HeaderHovered] = ImColor(139, 204, 199);
        colors[ImGuiCol_Text] = ImColor(64, 64, 92);
        colors[ImGuiCol_FrameBg] = ImColor(232, 232, 160);
        scene_background = float4(217, 217, 245, 1.0f);
    }
    else if (theme == Editor::Theme::Type::DARK)
    {
        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
        colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
        colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
        colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowPadding = ImVec2(10.00f, 10.00f);
        style.FramePadding = ImVec2(5.00f, 2.00f);
        style.CellPadding = ImVec2(8.00f, 8.00f);
        style.ItemSpacing = ImVec2(8.00f, 8.00f);
        style.ItemInnerSpacing = ImVec2(8.00f, 8.00f);
        style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
        style.IndentSpacing = 25;
        style.ScrollbarSize = 15;
        style.GrabMinSize = 10;
        style.WindowBorderSize = 1;
        style.ChildBorderSize = 1;
        style.PopupBorderSize = 1;
        style.FrameBorderSize = 1;
        style.TabBorderSize = 1;
        style.WindowRounding = 7;
        style.ChildRounding = 4;
        style.FrameRounding = 3;
        style.PopupRounding = 4;
        style.ScrollbarRounding = 9;
        style.GrabRounding = 3;
        style.LogSliderDeadzone = 4;
        style.TabRounding = 4;

        scene_background = float4(0.10f, 0.10f, 0.10f, 1.00f);
    }
    else
    {
        ImGui::StyleColorsDark();
        scene_background = float4(0.1f, 0.1f, 0.1f, 1.0f);
    }
}

bool Hachiko::ModuleEditor::Init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        io.ConfigViewportsNoAutoMerge = false;
        io.ConfigViewportsNoTaskBarIcon = true;
    }

    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        io.ConfigDockingTransparentPayload = true;
    }

    //TODO: ADD resource loader for fonts and icons
    // Fonts
    ImFontConfig config;
    config.MergeMode = true;
    config.GlyphMinAdvanceX = 16.0f;
    config.PixelSnapH = true;

    static constexpr ImWchar ICONS_RANGES[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    io.Fonts->AddFontFromFileTTF("fonts/JetBrainsMono-Light.ttf", 16.0f);

    m_big_icon_font = io.Fonts->AddFontFromFileTTF(StringUtils::Concat("fonts/",FONT_ICON_FILE_NAME_FAS).c_str(), 10.0f, &config, ICONS_RANGES);
    m_small_icon_font = io.Fonts->AddFontFromFileTTF(StringUtils::Concat("fonts/",FONT_ICON_FILE_NAME_FAR).c_str(), 10.0f, &config, ICONS_RANGES);

    // Setup style
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(App->window->GetWindow(), App->renderer->GetGLContext());
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);

    HE_LOG("Init windows");
    for (Window* panel : windows)
    {
        if (panel->active)
        {
            panel->Init();
        }
    }

    editor_prefs = App->preferences->GetEditorPreference();
    theme = editor_prefs->GetTheme();
    undo_redo_active = editor_prefs->GetUndoRedoActive();
    UpdateTheme();

#ifndef PLAY_BUILD
    if (undo_redo_active)
    {
        std::function create_history_entry = [&](Event& evt) {
            CreateSnapshot();
        };
        App->event->Subscribe(Event::Type::CREATE_EDITOR_HISTORY_ENTRY, create_history_entry);

        std::function enable_history = [&](Event& evt) {
            const auto state = evt.GetEventData<GameStateEventPayload>().GetState();
            history_enabled = state == GameStateEventPayload::State::STOPPED;
        };
        App->event->Subscribe(Event::Type::GAME_STATE, enable_history);
    }
#endif
    return true;
}

bool Hachiko::ModuleEditor::Start()
{
    history.Init();
    return true;
}

UpdateStatus Hachiko::ModuleEditor::PreUpdate(const float delta)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus Hachiko::ModuleEditor::Update(const float delta)
{
    ImGui::CaptureMouseFromApp(true);
    ImGui::CaptureKeyboardFromApp(true);

#ifndef PLAY_BUILD
    MainMenuBar();

    if (ImGuiFileDialog::Instance()->Display("LoadScene"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            const std::string file_path_name = ImGuiFileDialog::Instance()->GetFilePathName();

            //TODO: Make a function inside file sys to get relative path Assets/Scenes/X.scene
            const std::string file_name_extension = FileSystem::GetFileNameAndExtension(file_path_name.c_str());

            const std::string file_path = std::string(ASSETS_FOLDER_SCENE) + "/" + file_name_extension;

            HE_LOG("Loading scene: %s", file_path_name.c_str());

            history.CleanUp();
            App->resources->LoadAsset(file_path);
            history.Init();
        }

        ImGuiFileDialog::Instance()->Close();
    }

    GenerateDockingSpace();
#endif

    for (Window* panel : windows)
    {
        if (panel->active)
        {
            panel->Update();
        }
    }

    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus Hachiko::ModuleEditor::PostUpdate(const float delta)
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    const ImGuiIO& io = ImGui::GetIO();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SDL_GL_MakeCurrent(App->window->GetWindow(), App->renderer->GetGLContext());
    }

    if(to_remove)
    {
        to_remove->GetGameObject()->AttemptRemoveComponent(to_remove);
        to_remove = nullptr;
    }
    return UpdateStatus::UPDATE_CONTINUE;
}

void Hachiko::ModuleEditor::MainMenuBar()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 4));
    if (ImGui::BeginMainMenuBar())
    {
        FileMenu();
        EditMenu();
        GoMenu();
        ThemeMenu();
        ViewMenu();

        if (save_as_popup)
        {
            ImGui::OpenPopup("Save as");
            save_as_popup = false;
        }
        
        if (ImGui::BeginPopupModal("Save as", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::SetItemDefaultFocus();
            ImGui::InputText("File name", &file_name_buffer, sizeof(file_name_buffer));
            if (ImGui::Button("Save"))
            {
                HE_LOG("Saving Scene");
                App->scene_manager->SaveScene(file_name_buffer.c_str());
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("Documentation"))
            {
                Application::RequestBrowser("https://github.com/AkitaInteractive/Hachiko-Engine/wiki");
            }

            if (ImGui::MenuItem("Download latest"))
            {
                Application::RequestBrowser("https://github.com/AkitaInteractive/Hachiko-Engine/releases");
            }

            if (ImGui::MenuItem("Report a bug"))
            {
                Application::RequestBrowser("https://github.com/AkitaInteractive/Hachiko-Engine/issues");
            }

            if (ImGui::MenuItem("Download source!"))
            {
                Application::RequestBrowser("https://github.com/AkitaInteractive/Hachiko-Engine/archive/refs/heads/main.zip");
            }
            ImGui::EndMenu();
        }

#ifdef _DEBUG
        if (ImGui::BeginMenu("Debug"))
        {
            if (ImGui::MenuItem("Rebuild Gameplay.vcxproj"))
            {
                std::system("cd .. && cd Source && build_scripts.bat");
            }

            ImGui::EndMenu();
        }
#endif

        if (ImGui::BeginMenu("Shaders"))
        {
            if (ImGui::MenuItem("Recompile"))
            {
                App->program->RecompilePrograms();
            }

            ImGui::EndMenu();
        }

        ImGui::PopStyleVar(2);
        ImGui::EndMainMenuBar();
    }
}

void Hachiko::ModuleEditor::GenerateDockingSpace()
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGuiWindowFlags host_window_flags = 0;
    host_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
    host_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoNavInputs;
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
    {
        host_window_flags |= ImGuiWindowFlags_NoBackground;
    }

    const ImGuiID dockspace_id = ImGui::GetID("DockSpace");

    if (!ImGui::DockBuilderGetNode(dockspace_id))
    {
        ImGui::DockBuilderAddNode(dockspace_id);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->WorkSize);

        dock_main_id = dockspace_id;
        dock_up_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.2f, nullptr, &dock_main_id);
        ImGui::DockBuilderSetNodeSize(dock_up_id, ImVec2(viewport->WorkSize.x, 40));
        dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.2f, nullptr, &dock_main_id);
        dock_down_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.3f, nullptr, &dock_main_id);
        dock_left_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.25f, nullptr, &dock_main_id);
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    if (ImGui::Begin("DockSpace Window", nullptr, host_window_flags))
    {
        ImGui::PopStyleVar(3);
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags, nullptr);
        ImGui::End();
    }
}

void Hachiko::ModuleEditor::FileMenu()
{
    if (!ImGui::BeginMenu("File"))
    {
        return;
    }

    if (ImGui::MenuItem(ICON_FA_PLUS "New"))
    {
        history.CleanUp();
        // Create new scene with engine stopped, passing id 0 does that
        constexpr bool stop_scene = true;
        App->scene_manager->ChangeSceneById(0, stop_scene);
        history.Init();
    }
    if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK "Save", nullptr, false, true)) // TODO: Use internal timer to disable/enable
    {
        // TODO: Add the option to specify a name (no name uses scene internal name)
        App->scene_manager->SaveScene();
    }
    if (ImGui::MenuItem("Save as", nullptr, false, true)) // TODO: Use internal timer
    {
        save_as_popup = true;
        
    }

    ImGui::Separator();
    if (ImGui::MenuItem("Load"))
    {
        ImGuiFileDialog::Instance()->OpenDialog("LoadScene", "Load Scene", ".scene", ASSETS_FOLDER_SCENE);
    }

    ImGui::Separator();
    if (ImGui::MenuItem("Exit"))
    {
        App->MarkAsQuitting(true);
    }

    ImGui::EndMenu();
}

void Hachiko::ModuleEditor::ViewMenu()
{
    if (ImGui::BeginMenu("View"))
    {
        ImGui::MenuItem(w_scene.name, nullptr, &w_scene.active);
        ImGui::MenuItem(w_inspector.name, nullptr, &w_inspector.active);
        ImGui::MenuItem(w_hierarchy.name, nullptr, &w_hierarchy.active);
        ImGui::MenuItem(w_configuration.name, nullptr, &w_configuration.active);
        ImGui::MenuItem(w_about.name, nullptr, &w_about.active);
        ImGui::MenuItem(w_resource.name, nullptr, &w_resource.active);
        ImGui::MenuItem(w_project.name, nullptr, &w_project.active);
        ImGui::MenuItem(w_timers.name, nullptr, &w_timers.active);
        ImGui::EndMenu();
    }
}

void Hachiko::ModuleEditor::ThemeMenu() const
{
    if (ImGui::BeginMenu("Theme"))
    {
        bool light_theme = App->editor->GetTheme() == Editor::Theme::Type::LIGHT;
        bool dark_theme = App->editor->GetTheme() == Editor::Theme::Type::DARK;
        bool pink_theme = App->editor->GetTheme() == Editor::Theme::Type::PINK;
        bool classic_theme = App->editor->GetTheme() == Editor::Theme::Type::CLASSIC;

        if (ImGui::MenuItem("Light", nullptr, &light_theme))
        {
            HE_LOG("Selecting light mode");
            App->editor->SetTheme(Editor::Theme::Type::LIGHT);
            UpdateTheme();
        }
        if (ImGui::MenuItem("Dark", nullptr, &dark_theme))
        {
            HE_LOG("Selecting dark mode");
            App->editor->SetTheme(Editor::Theme::Type::DARK);
            UpdateTheme();
        }
        if (ImGui::MenuItem("Pink", nullptr, &pink_theme))
        {
            HE_LOG("Selecting pink mode");
            App->editor->SetTheme(Editor::Theme::Type::PINK);
            UpdateTheme();
        }
        if (ImGui::MenuItem("Classic", nullptr, &classic_theme))
        {
            HE_LOG("Selecting classic mode");
            App->editor->SetTheme(Editor::Theme::Type::CLASSIC);
            UpdateTheme();
        }
        ImGui::EndMenu();
    }
}

void Hachiko::ModuleEditor::CreateSnapshot()
{
    history.Save(App->scene_manager->GetActiveScene()->CreateSnapshot());
}

void Hachiko::ModuleEditor::Undo()
{
    const auto memento = history.Undo();
    if (memento)
    {
        App->scene_manager->GetActiveScene()->Restore(memento);
    }
}

void Hachiko::ModuleEditor::Redo()
{
    const auto memento = history.Redo();
    if (memento)
    {
        App->scene_manager->GetActiveScene()->Restore(memento);
    }
}

void Hachiko::ModuleEditor::EditMenu()
{
    if (App->input->IsModifierPressed(KMOD_CTRL) && App->input->IsKeyDown(SDL_SCANCODE_Z))
    {
        Undo();
    }

    if (App->input->IsModifierPressed(KMOD_CTRL) && App->input->IsKeyDown(SDL_SCANCODE_Y))
    {
        Redo();
    }

    const bool is_go_selected = GetSelectedGameObject() != nullptr;

    if (!ImGui::BeginMenu("Edit"))
    {
        return;
    }

    if (ImGui::MenuItem(ICON_FA_ROTATE_LEFT "Undo", "CTRL+Z", false, CanUndo()))
    {
        Undo();
    }
    if (ImGui::MenuItem(ICON_FA_ROTATE_RIGHT "Redo", "CTRL+Y", false, CanRedo()))
    {
        Redo();
    }
    ImGui::Separator();
    if (ImGui::MenuItem(ICON_FA_CLIPBOARD "Copy", nullptr, false, is_go_selected))
    {
        //TODO:
    }
    if (ImGui::MenuItem(ICON_FA_PASTE "Paste", nullptr, false, CanPaste()))
    {
        //TODO:
    }
    if (ImGui::MenuItem(ICON_FA_CLONE "Duplicate", nullptr, false, is_go_selected))
    {
        //TODO:
    }
    ImGui::EndMenu();
}

void Hachiko::ModuleEditor::GoMenu() const
{
    if (!ImGui::BeginMenu("GameObject"))
    {
        return;
    }

    if (ImGui::MenuItem("Add GameObject"))
    {
        App->scene_manager->GetActiveScene()->CreateNewGameObject(App->scene_manager->GetActiveScene()->GetRoot(), "GameObject");
        App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
    }

    ImGui::EndMenu();
}

bool Hachiko::ModuleEditor::CleanUp()
{
    HE_LOG("Cleaning windows");

    for (Window* panel : windows)
    {
        if (panel->active)
        {
            panel->CleanUp();
        }
    }

    history_enabled = true; //ensure history is enabled so we can clear it
    history.CleanUp();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    editor_prefs->SetTheme(theme);
    editor_prefs->SetUndoRedoActive(undo_redo_active);
    return true;
}


Hachiko::Scene::Memento* Hachiko::ModuleEditor::History::Undo()
{
    if (CanUndo())
    {
        return mementos[--current_position - 1].get();
    }
    return nullptr;
}

Hachiko::Scene::Memento* Hachiko::ModuleEditor::History::Redo()
{
    if (CanRedo())
    {
        return mementos[current_position++].get();
    }
    return nullptr;
}

void Hachiko::ModuleEditor::History::Save(Scene::Memento* memento)
{
    if (history_enabled)
    {
        if (CanRedo())
        {
            //remove unused mementos branch
            const auto it = mementos.begin() + current_position;
            mementos.erase(it, mementos.end());
        }
        current_position++;
        mementos.emplace_back(memento);
    }
}

bool Hachiko::ModuleEditor::History::CanUndo() const
{
    return history_enabled && current_position > 1;
}

bool Hachiko::ModuleEditor::History::CanRedo() const
{
    return history_enabled && current_position < mementos.size();
}

void Hachiko::ModuleEditor::History::Init()
{
    if (history_enabled)
    {
        current_position = 0;
        Save(App->scene_manager->GetActiveScene()->CreateSnapshot());
    }
}

void Hachiko::ModuleEditor::History::CleanUp()
{
    if (history_enabled)
    {
        HE_LOG("Clearing action history");
        mementos.erase(mementos.begin(), mementos.end());
        mementos.clear();
    }
}
