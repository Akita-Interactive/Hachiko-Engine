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
#include "modules/ModuleEvent.h"

#include "modules/ModuleWindow.h"

#include "ui/ImGuiUtils.h"

Hachiko::WindowScene::WindowScene() :
    Window("Scene", true)
{
}

void Hachiko::WindowScene::Init()
{
    std::function updateViewportSize = [&](Event& evt) {
        const auto state = evt.GetEventData<GameStateEventPayload>().GetState();
        if (state == GameStateEventPayload::State::STARTED)
        {
            DrawScene();
        }
    };
    App->event->Subscribe(Event::Type::GAME_STATE, updateViewportSize);
}

void Hachiko::WindowScene::Update()
{
    UpdatePlayModeBlinker();

    const bool is_in_play_mode = App->scene_manager->IsScenePlaying();

    if (!is_in_play_mode && focused && App->editor->GetSelectedGameObject())
    {
        if (App->input->IsKeyPressed(SDL_SCANCODE_W))
        {
            guizmo_operation = ImGuizmo::TRANSLATE;
        }
        if (App->input->IsKeyPressed(SDL_SCANCODE_E))
        {
            guizmo_operation = ImGuizmo::ROTATE;
        }
        if (App->input->IsKeyPressed(SDL_SCANCODE_R))
        {
            guizmo_operation = ImGuizmo::SCALE;
        }
    }

    ImGui::SetNextWindowDockID(App->editor->dock_main_id, ImGuiCond_FirstUseEver);

    if (!ImGui::Begin((std::string(ICON_FA_GLOBE " ") + name).c_str(), &active, ImGuiWindowFlags_NoNavInputs))
    {
        ImGui::End();
        return;
    }

    focused = ImGui::IsWindowFocused();

    GuizmoOptionsController();

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    ToolbarMenu();
    DrawScene();

    if (!is_in_play_mode)
    {
        Controller();
    }

    ImGui::End();
}

void Hachiko::WindowScene::GuizmoOptionsController()
{
    if (ImGuiUtils::ToolbarButton(App->editor->m_big_icon_font, ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT, guizmo_operation == ImGuizmo::TRANSLATE, "Move Tool"))
    {
        guizmo_operation = ImGuizmo::TRANSLATE;
    }
    if (ImGuiUtils::ToolbarButton(App->editor->m_big_icon_font, ICON_FA_ARROW_ROTATE_LEFT, guizmo_operation == ImGuizmo::ROTATE, "Rotate Tool"))
    {
        guizmo_operation = ImGuizmo::ROTATE;
    }
    if (ImGuiUtils::ToolbarButton(App->editor->m_big_icon_font, ICON_FA_MAXIMIZE, guizmo_operation == ImGuizmo::SCALE, "Scale Tool"))
    {
        guizmo_operation = ImGuizmo::SCALE;
    }

    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();

    if (ImGuiUtils::ToolbarButton(App->editor->m_big_icon_font, ICON_FA_HOUSE, guizmo_mode == ImGuizmo::LOCAL, "Switches to Local Gizmos mode.", guizmo_operation != ImGuizmo::SCALE))
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

float2 Hachiko::WindowScene::NormalizePositionToScene(const float2& position) const
{
    const auto mouse_viewport_pos = float2(position.x - guizmo_rect_origin.x, position.y - guizmo_rect_origin.y);

    return float2(mouse_viewport_pos.x / texture_size.x, mouse_viewport_pos.y / texture_size.y);
}

void Hachiko::WindowScene::DrawScene()
{
    ImVec2 size = ImGui::GetContentRegionAvail();

    bool size_changed = size.x != texture_size.x || size.y != texture_size.y;
    bool size_valid = (size.x > 0 && size.y > 0);

    if (size_changed && size_valid)
    {
        texture_size = {size.x, size.y};

        viewport_position = float2(
            ImGui::GetWindowPos().x,
            ImGui::GetWindowPos().y
            + ImGui::GetStyle().WindowPadding.y * 2
            + ImGui::GetStyle().FramePadding.y * 2
            + ImGui::GetTextLineHeight() * 2
            + ImGui::GetStyle().ItemSpacing.y);

        viewport_size = float2(size.x, size.y);

        App->camera->OnResize(
            static_cast<unsigned>(texture_size.x), 
            static_cast<unsigned>(texture_size.y));
    }

    // Top left corner of the image in imgui coords, top is y = 0
    guizmo_rect_origin = ImGui::GetCursorScreenPos();

    // Bottom left corner in opengl coordinates, bottom is y = 0
    const float window_height(App->window->GetHeight());
    texture_position = float2(
        guizmo_rect_origin.x, 
         window_height - (guizmo_rect_origin.y + texture_size.y));

    void* texture_pointer = reinterpret_cast<void*>(
        static_cast<intptr_t>(App->renderer->GetTextureId()));

    // Construct the image of frame buffer from size and uvs:
    const bool is_in_play_mode = App->scene_manager->IsScenePlaying();
    const ImVec4 border_color = is_in_play_mode ? 
            ImVec4(FLOAT4_TO_ARGS(play_mode_blinker.current_color)) :
            ImVec4(0, 0, 0, 0);
    const float border_thickness = is_in_play_mode ? 6.0f : 0.0f;
    const ImVec2 image_size(size.x, size.y - border_thickness * 0.5f);
    ImGui::Image(
        texture_pointer, 
        image_size, 
        ImVec2(0, 1), 
        ImVec2(1, 0), 
        ImVec4(1, 1, 1, 1),
        border_color, border_thickness);

    // Get if the mouse is on the scene window:
    hovering = ImGui::IsItemHovered();

    // Update viewport min and max positions, these can be used for clamping
    // mouse movement and/or checking if a pixel position is inside the
    // viewport:
    viewport_max_position = 
        float2(FLOAT2_TO_ARGS(guizmo_rect_origin)) + 
        float2(FLOAT2_TO_ARGS(viewport_size));
    viewport_min_position = float2(FLOAT2_TO_ARGS(guizmo_rect_origin));

    // If the mouse is on the scene window, update the last hover position to
    // be the current mouse position. Equivalent to ImGui::GetMousePosition():
    if (hovering)
    {
        last_hover_position = App->input->GetMouseGlobalPixelPosition();
    }

    // TODO: Move to other section to take the input
    if (ImGui::BeginDragDropTarget())
    {
        if (auto* payload = ImGui::AcceptDragDropPayload("path"))
        {
            auto path = static_cast<const char*>(payload->Data);
        }
        ImGui::EndDragDropTarget();
    }

    // To avoid camera orbiting:
    if (ImGui::IsWindowFocused())
    {
        const bool guizmo_enabled = 
            !(App->input->IsMouseButtonPressed(SDL_BUTTON_RIGHT) || 
             App->input->IsKeyPressed(SDL_SCANCODE_LALT));

        ImGuizmo::Enable(guizmo_enabled);
    }

    constexpr bool IS_TRANSPOSED = true;

    ComponentCamera* camera = App->camera->GetRenderingCamera();
    float4x4 view = camera->GetViewMatrix(IS_TRANSPOSED);

    ImGuizmo::ViewManipulate(
        view.ptr(), 
        4, 
        ImVec2(
            guizmo_rect_origin.x + texture_size.x - imguizmo_size.x, 
            guizmo_rect_origin.y + texture_size.y - imguizmo_size.x), 
        imguizmo_size, 
        0x10101010);

    if (ImGui::IsWindowFocused())
    {
        const float4x4 new_camera_view = view.InverseTransposed();

        App->camera->GetRenderingCamera()->SetFrame(
            new_camera_view.Col(3).xyz(), 
            -new_camera_view.Col(2).xyz(), 
            new_camera_view.Col(1).xyz());
    }

    GameObject* selected_object = App->editor->GetSelectedGameObject();
    
    if (!selected_object)
    {
        return;
    }
    ImGuizmo::SetRect(guizmo_rect_origin.x, guizmo_rect_origin.y, texture_size.x, texture_size.y);
    ImGuizmo::SetDrawlist();

    // Scale will always be on local regarding on ImGuizmo MODE, check is done
    // here to keep user's selected MODE
    const bool is_scaling = guizmo_operation == ImGuizmo::SCALE;

    // Using GL format which means transposing them:
    view = camera->GetViewMatrix(IS_TRANSPOSED);
    float4x4 projection = camera->GetProjectionMatrix(IS_TRANSPOSED);
    float4x4 model = selected_object->GetTransform()->GetGlobalMatrix().Transposed();
    float4x4 delta(float4x4::identity);
    Manipulate(view.ptr(), projection.ptr(), guizmo_operation, is_scaling ? ImGuizmo::LOCAL : guizmo_mode, model.ptr(), delta.ptr());

    using_guizmo = ImGuizmo::IsUsing();
    if (using_guizmo && !delta.IsIdentity())
    {
        // Transpose back again to store in our format
        model.Transpose();
        selected_object->GetTransform()->SetGlobalTransform(model);
        changed_game_object = true;
    }

    if (!using_guizmo && changed_game_object)
    {
        changed_game_object = false;
        App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
    }
}

void Hachiko::WindowScene::Controller() const
{
    if (!using_guizmo && focused && hovering && App->input->IsMouseButtonPressed(SDL_BUTTON_LEFT))
    {
        const Scene* scene = App->scene_manager->GetActiveScene();
        GameObject* picked = SelectObject(App->camera->GetRenderingCamera(), scene);
        if (picked)
        {
            App->editor->SetSelectedGO(picked);
        }
    }
}

void Hachiko::WindowScene::UpdatePlayModeBlinker()
{
    if (!App->scene_manager->IsScenePlaying())
    {
        return;
    }

    if (play_mode_blinker.progress == 1.0f)
    {
        play_mode_blinker.progress = 0.0f;

        play_mode_blinker.is_passive_to_active = 
            !play_mode_blinker.is_passive_to_active;
    }

    const float delta_time = Time::DeltaTime();

    play_mode_blinker.progress += delta_time / play_mode_blinker.duration;
    play_mode_blinker.progress = play_mode_blinker.progress > 1.0f
                                 ? 1.0f
                                 : play_mode_blinker.progress;

    if (play_mode_blinker.is_passive_to_active)
    {
        play_mode_blinker.current_color = float4::Lerp(
            play_mode_blinker.passive_color, 
            play_mode_blinker.active_color, 
            play_mode_blinker.progress);
    }
    else
    {
        play_mode_blinker.current_color = float4::Lerp(
            play_mode_blinker.active_color, 
            play_mode_blinker.passive_color, 
            play_mode_blinker.progress);
    }
}

Hachiko::GameObject* Hachiko::WindowScene::SelectObject(const ComponentCamera* camera, const Scene* scene) const
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

void Hachiko::WindowScene::ClampMousePosition(float2& mouse_position) const
{
    bool is_partially_outside = true;
    is_partially_outside |= mouse_position.x < viewport_min_position.x;
    is_partially_outside |= mouse_position.y < viewport_min_position.y;
    is_partially_outside |= mouse_position.x > viewport_max_position.x;
    is_partially_outside |= mouse_position.y > viewport_max_position.y;

    if (!is_partially_outside)
    {
        return;
    }

    mouse_position = last_hover_position;
}

const float2& Hachiko::WindowScene::GetViewportSize() const
{
    return viewport_size;
}

const float2& Hachiko::WindowScene::GetViewportPosition() const
{
    return viewport_position;
}
