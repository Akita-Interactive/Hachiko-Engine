#include "core/hepch.h"
#include "WindowScene.h"

#include "modules/ModuleInput.h"
#include "modules/ModuleEditor.h"
#include "modules/ModuleCamera.h"
#include "modules/ModuleRender.h"
#include "modules/ModuleSceneManager.h"
#include "modules/ModuleDebugMode.h"

#include "components/ComponentCamera.h"
#include "components/ComponentTransform.h"

#include "modules/ModuleWindow.h"

#include "ui/ImGuiUtils.h"

Hachiko::WindowScene::WindowScene() : Window("Scene", true) {}

Hachiko::WindowScene::~WindowScene() = default;

void Hachiko::WindowScene::Update()
{
    if (!App->input->GetMouseButton(SDL_BUTTON_RIGHT))
    {
        if (App->input->GetKey(SDL_SCANCODE_W) == KeyState::KEY_REPEAT)
            guizmo_operation = ImGuizmo::TRANSLATE;
        if (App->input->GetKey(SDL_SCANCODE_E) == KeyState::KEY_REPEAT)
            guizmo_operation = ImGuizmo::ROTATE;
        if (App->input->GetKey(SDL_SCANCODE_R) == KeyState::KEY_REPEAT)
            guizmo_operation = ImGuizmo::SCALE;
    }
    ImGui::SetNextWindowDockID(App->editor->dock_main_id, ImGuiCond_FirstUseEver);
    if (ImGui::Begin((std::string(ICON_FA_GLOBE " ") + name).c_str(), &active))
    {
        focused = ImGui::IsWindowFocused();
        GuizmoOptionsController();
        ImGui::SameLine();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
        ImGui::SameLine();
        ToolbarMenu();
        DrawScene();
        Controller();
        ImGui::End();
    }
}

void Hachiko::WindowScene::CleanUp() {}

void Hachiko::WindowScene::GuizmoOptionsController()
{
    if (ImGuiUtils::ToolbarButton(App->editor->m_big_icon_font, ICON_FA_ARROWS_ALT, guizmo_operation == ImGuizmo::TRANSLATE, "Move Tool"))
    {
        guizmo_operation = ImGuizmo::TRANSLATE;
    }
    if (ImGuiUtils::ToolbarButton(App->editor->m_big_icon_font, ICON_FA_UNDO, guizmo_operation == ImGuizmo::ROTATE, "Rotate Tool"))
    {
        guizmo_operation = ImGuizmo::ROTATE;
    }
    if (ImGuiUtils::ToolbarButton(App->editor->m_big_icon_font, ICON_FA_EXPAND_ALT, guizmo_operation == ImGuizmo::SCALE, "Scale Tool"))
    {
        guizmo_operation = ImGuizmo::SCALE;
    }

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    if (ImGuiUtils::ToolbarButton(App->editor->m_big_icon_font, ICON_FA_HOME, guizmo_mode == ImGuizmo::LOCAL, "Switches to Local Gizmos mode.", guizmo_operation != ImGuizmo::SCALE))
    {
        guizmo_mode = ImGuizmo::LOCAL;
    }

    if (ImGuiUtils::ToolbarButton(App->editor->m_big_icon_font, ICON_FA_GLOBE, guizmo_mode == ImGuizmo::WORLD, "Switches to Global Gizmos mode.", guizmo_operation != ImGuizmo::SCALE))
    {
        guizmo_mode = ImGuizmo::WORLD;
    }
}

void Hachiko::WindowScene::ToolbarMenu() const
{
    const float w = (ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x) * 0.5f - 30 - ImGui::GetCursorPosX();
    ImGui::Dummy(ImVec2(w, ImGui::GetTextLineHeight()));

    bool playing = !GameTimer::paused && GameTimer::running;

    if (ImGuiUtils::ToolbarButton(App->editor->m_big_icon_font, ICON_FA_PAUSE, GameTimer::paused && GameTimer::running, "Pause"))
    {
        App->scene_manager->AttemptScenePause();
    }

    if (ImGuiUtils::ToolbarButton(App->editor->m_big_icon_font, ICON_FA_PLAY, !GameTimer::paused && GameTimer::running, "Play"))
    {
        App->scene_manager->AttemptScenePlay();
    }

    if (ImGuiUtils::ToolbarButton(App->editor->m_big_icon_font, ICON_FA_STOP, !GameTimer::running, "Stop"))
    {
        App->scene_manager->AttemptSceneStop();
    }

    ImGui::Separator();
}

void Hachiko::WindowScene::DrawScene()
{
    ImVec2 size = ImGui::GetContentRegionAvail();
    if (size.x != texture_size.x || size.y != texture_size.y)
    {
        texture_size = {
            size.x,
            size.y,
        };
        App->camera->OnResize(static_cast<unsigned>(texture_size.x), static_cast<unsigned>(texture_size.y));
    }

    ComponentCamera* camera = App->camera->GetMainCamera();

    // Top left corner of the image in imgui coords, top is y = 0
    guizmo_rect_origin = ImGui::GetCursorScreenPos();
    // Bottom left corner in opengl coordinates, bottom is y = 0
    texture_position = float2(guizmo_rect_origin.x, static_cast<float>(App->window->GetHeight()) - guizmo_rect_origin.y - texture_size.y);

    ImGui::Image((void*)static_cast<intptr_t>(App->renderer->GetTextureId()), size, ImVec2 {0, 1}, ImVec2 {1, 0});
    // Checking hover on image for more intuitive controls
    hovering = ImGui::IsItemHovered();

    // TODO: Move to other section to take the input
    if (ImGui::BeginDragDropTarget())
    {
        if (auto* payload = ImGui::AcceptDragDropPayload("path"))
        {
            auto path = static_cast<const char*>(payload->Data);
        }
        ImGui::EndDragDropTarget();
    }

    // TO AVOID Camera orbit
    if (ImGui::IsWindowFocused())
    {
        bool guizmo_enabled = !(App->input->GetMouseButton(SDL_BUTTON_RIGHT) || App->input->GetKey(SDL_SCANCODE_LALT) == KeyState::KEY_DOWN);
        ImGuizmo::Enable(guizmo_enabled);
    }

    constexpr bool transposed = true;
    float4x4 view = camera->GetViewMatrix(transposed);

    //TODO ADD look at when manipulating cube
    ImGuizmo::ViewManipulate(view.ptr(), 4, ImVec2(guizmo_rect_origin.x + texture_size.x - imguizmo_size.x, guizmo_rect_origin.y + texture_size.y - imguizmo_size.x), imguizmo_size, 0x10101010);

    if (ImGui::IsWindowFocused())
    {
        float4x4 new_camera_view = view.InverseTransposed();
        App->camera->GetMainCamera()->GetFrustum()->SetFrame(new_camera_view.Col(3).xyz(), -new_camera_view.Col(2).xyz(), new_camera_view.Col(1).xyz());
    }

    GameObject* selected_object = App->editor->GetSelectedGameObject();
    if (!selected_object)
        return;

    if (selected_object)
    {
        // Using GL format which means transposing them
        view = camera->GetViewMatrix(transposed);
        float4x4 projection = camera->GetProjectionMatrix(transposed);
        float4x4 model = selected_object->GetTransform()->GetGlobalMatrix().Transposed();
        float4x4 delta;

        ImGuizmo::SetRect(guizmo_rect_origin.x, guizmo_rect_origin.y, texture_size.x, texture_size.y);
        ImGuizmo::SetDrawlist();

        // Scale will always be on local regarding on ImGuizmo MODE, check is done here to keep user's selected MODE
        const bool is_scaling = guizmo_operation == ImGuizmo::SCALE;
        Manipulate(view.ptr(), projection.ptr(), guizmo_operation, is_scaling ? ImGuizmo::LOCAL : guizmo_mode, model.ptr(), delta.ptr());

        using_guizmo = ImGuizmo::IsUsing();
        if (using_guizmo && !delta.IsIdentity())
        {
            // Transpose back again to store in our format
            model.Transpose();
            selected_object->GetTransform()->SetGlobalTransform(model);
        }
    }
}

void Hachiko::WindowScene::Controller() const
{
    if (!using_guizmo && focused && hovering && App->input->GetMouseButton(SDL_BUTTON_LEFT))
    {
        Scene* scene = App->scene_manager->GetActiveScene();
        GameObject* picked = SelectObject(App->camera->GetMainCamera(), scene);
        if (picked)
            App->editor->SetSelectedGO(picked);
    }
}

Hachiko::GameObject* Hachiko::WindowScene::SelectObject(ComponentCamera* camera, Scene* scene) const
{
    // sdl -> (0, 0) top left, (w, h) bottom right
    // // imgui -> (0,0) botton left
    // plane -> (1,1) top right , (-1, -1) bottom left

    const ImVec2 mouse = ImGui::GetMousePos();
    const auto mouse_viewport_pos = float2(mouse.x - guizmo_rect_origin.x, mouse.y - guizmo_rect_origin.y);

    // Fit in range and coordinate direction
    const float x_normalized = mouse_viewport_pos.x / texture_size.x * 2.f - 1.f;
    const float y_normalized = -(mouse_viewport_pos.y / texture_size.y * 2.f - 1.f);

    const LineSegment line = camera->Raycast(x_normalized, y_normalized);
    App->debug_mode->SetLine(line);
    GameObject* selected = scene->Raycast(line);

    return selected;
}

float2 Hachiko::WindowScene::ImguiToScreenPos(const float2& mouse_pos) const
{
    float2 mouse_viewport_pos = float2(mouse_pos.x - guizmo_rect_origin.x, mouse_pos.y - guizmo_rect_origin.y);
    float2 center = texture_size / 2;
    float2 mouse_ui_pos = float2(mouse_viewport_pos.x - center.x, (-mouse_viewport_pos.y) + center.y);
    return mouse_ui_pos;
}

float2 Hachiko::WindowScene::GetInterfaceClickPos() const
{
    // sdl -> (0, 0) top left, (w, h) bottom right
    // interface -> center 0,0 top right pos bot left neg
    const ImVec2 mouse_pos = ImGui::GetMousePos();
    return ImguiToScreenPos(float2(mouse_pos.x, mouse_pos.y));
}
