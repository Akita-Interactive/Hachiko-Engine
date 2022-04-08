#include "core/hepch.h"

#include "ModuleEditor.h"
#include "ModuleFileSystem.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleSceneManager.h"

Hachiko::ModuleEditor::ModuleEditor()
{
    HE_LOG("Creating windows");

#ifdef PLAY_BUILD
    windows.push_back(&w_configuration);
#else
    windows.push_back(&w_configuration);
    windows.push_back(&w_hierarchy);
    windows.push_back(&w_scene);
    windows.push_back(&w_inspector);
    windows.push_back(&w_about);
    windows.push_back(&w_console);
    // windows.push_back(&w_resource);
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

#ifdef PLAY_BUILD
    const UpdateStatus retval = UpdateStatus::UPDATE_CONTINUE;
#else
    const UpdateStatus retval = MainMenuBar();

    if (ImGuiFileDialog::Instance()->Display("LoadScene"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            const std::string file_path_name = ImGuiFileDialog::Instance()->GetFilePathName();

            //TODO: Make a function inside file sys to get relative path Assets/Scenes/X.scene
            const std::string file_name_extension = App->file_sys->GetFileNameAndExtension(file_path_name.c_str());

            const std::string file_path = std::string(ASSETS_FOLDER_SCENES) + "/" + file_name_extension;

            HE_LOG("Loading scene: %s", file_path_name.c_str());
            App->scene_manager->LoadScene(file_path.c_str());
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

    //RenderGui();
    return retval;
}

UpdateStatus Hachiko::ModuleEditor::MainMenuBar()
{
    auto status = UpdateStatus::UPDATE_CONTINUE;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 4));
    if (ImGui::BeginMainMenuBar())
    {
        FileMenu();
        EditMenu();
        GoMenu();
        ThemeMenu();
        ViewMenu();

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

            if (ImGui::MenuItem("Quit"))
            {
                status = UpdateStatus::UPDATE_STOP;
            }

            ImGui::EndMenu();
        }

        ImGui::PopStyleVar(2);
        ImGui::EndMainMenuBar();
    }

    return status;
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
    host_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
    {
        host_window_flags |= ImGuiWindowFlags_NoBackground;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Window", nullptr, host_window_flags);
    ImGui::PopStyleVar(3);

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

    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags, nullptr);
    ImGui::End();
}

void Hachiko::ModuleEditor::FileMenu() const
{
    // TODO: shortcuts
    Scene* current_scene = App->scene_manager->GetActiveScene();

    if (!ImGui::BeginMenu("File"))
    {
        return;
    }
    if (ImGui::MenuItem(ICON_FA_PLUS "New"))
    {
        App->scene_manager->CreateEmptyScene();
    }
    if (ImGui::MenuItem(ICON_FA_SAVE "Save", nullptr, false, true)) // TODO: Use internal timer to disable/enable
    {
        const std::string temp_scene_file_path = std::string(ASSETS_FOLDER_SCENES) + "/" + "untitled" + SCENE_EXTENSION;
        App->scene_manager->SaveScene(temp_scene_file_path.c_str());
    }
    if (ImGui::MenuItem("Save as", nullptr, false, true)) // TODO: Use internal timer
    {
        ImGui::OpenPopup("Save scene");
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Load"))
    {
        ImGuiFileDialog::Instance()->OpenDialog("LoadScene", "Load Scene", ".scene", ASSETS_FOLDER_SCENES);
    }

    ImGui::EndMenu();

    char file_name_buffer[32] = {'\0'};
    if (ImGui::BeginPopupModal("Save scene"))
    {
        ImGui::SetItemDefaultFocus();
        ImGui::InputText("File name", file_name_buffer, sizeof(file_name_buffer));
        if (ImGui::Button("Save"))
        {
            HE_LOG("Saving Scene");
            const std::string temp_scene_file_path = std::string(ASSETS_FOLDER_SCENES) + "/" + file_name_buffer + SCENE_EXTENSION;
            App->scene_manager->SaveScene(temp_scene_file_path.c_str());
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
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
        ImGui::EndMenu();
    }
}

void Hachiko::ModuleEditor::EditMenu() const
{
    // TODO: shortcuts
    const bool is_go_selected = GetSelectedGameObject() != nullptr;

    if (!ImGui::BeginMenu("Edit"))
        return;

    if (ImGui::MenuItem(ICON_FA_UNDO "Undo", nullptr, false, CanUndo()))
    {
        //TODO:
    }
    if (ImGui::MenuItem(ICON_FA_REDO "Redo", nullptr, false, CanRedo()))
    {
        //TODO:
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
        App->scene_manager->GetActiveScene()->CreateNewGameObject("GameObject", App->scene_manager->GetActiveScene()->GetRoot());
    }

    ImGui::EndMenu();
}

void Hachiko::ModuleEditor::RenderGui()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    SDL_GL_MakeCurrent(App->window->GetWindow(), App->renderer->GetGLContext());
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

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    return true;
}
