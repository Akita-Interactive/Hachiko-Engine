#include "core/hepch.h"
#include "modules/ModuleInput.h"
#include "modules/ModuleSceneManager.h"
#include "modules/ModuleCamera.h"
#include "modules/ModuleAudio.h"
#include "modules/ModuleRender.h"
#include "modules/ModuleResources.h"
#include "Gameplay.h"
#include "modules/ModuleNavigation.h"
#include "utils/FileSystem.h"
#include "debugdraw.h"

#include "components/ComponentAgent.h"

/*HACHIKO--------------------------------------------------------------------*/

void Hachiko::Quit()
{
    App->MarkAsQuitting(true);
}

/*---------------------------------------------------------------------------*/

/*TIME-----------------------------------------------------------------------*/

float Hachiko::Time::DeltaTime()
{
    // TODO: Return Gameplay Timer.
    // Added for easiness of exposing to scripts. We need to refactor that
    // timer code as it's unnecessarily complicated.
    return EngineTimer::delta_time;
}

/*---------------------------------------------------------------------------*/

/*INPUT----------------------------------------------------------------------*/

bool Hachiko::Input::IsKeyPressed(KeyCode key)
{
    return App->input->IsKeyPressed(static_cast<int>(key));
}

bool Hachiko::Input::IsKeyUp(KeyCode key)
{
    return App->input->IsKeyUp(static_cast<SDL_Scancode>(key));
}

bool Hachiko::Input::IsKeyDown(KeyCode key)
{
    return App->input->GetKey(static_cast<SDL_Scancode>(key)) == KeyState::KEY_DOWN;
}

bool Hachiko::Input::IsModifierPressed(KeyCode modifier)
{
    return App->input->IsModifierPressed(static_cast<SDL_Keymod>(modifier));
}

bool Hachiko::Input::IsMouseButtonPressed(MouseButton mouse_button)
{
#ifdef PLAY_BUILD
    return App->input->IsMouseButtonPressed(static_cast<int>(mouse_button));
#else
    return App->input->IsMouseButtonPressed(static_cast<int>(mouse_button)) && 
           App->editor->GetSceneWindow()->IsActive();
#endif // PLAY_BUILD
}

bool Hachiko::Input::IsMouseButtonUp(MouseButton mouse_button)
{
#ifdef PLAY_BUILD
    return App->input->IsMouseButtonUp(static_cast<int>(mouse_button));
#else
    return App->input->IsMouseButtonUp(static_cast<int>(mouse_button)) && 
           App->editor->GetSceneWindow()->IsActive();
#endif // PLAY_BUILD
}

bool Hachiko::Input::IsMouseButtonDown(MouseButton mouse_button)
{
#ifdef PLAY_BUILD
    return App->input->IsMouseButtonDown(static_cast<int>(mouse_button));
#else
    return App->input->IsMouseButtonDown(static_cast<int>(mouse_button)) && 
           App->editor->GetSceneWindow()->IsActive();
#endif // PLAY_BUILD
}

int Hachiko::Input::GetScrollWheelDelta()
{
#ifdef PLAY_BUILD
    return App->input->GetScrollDelta();
#else
    return App->editor->GetSceneWindow()->IsActive()
            ? App->input->GetScrollDelta()
            : 0;
#endif // PLAY_BUILD
}

float2 Hachiko::Input::GetMouseNormalizedMotion()
{
#ifdef PLAY_BUILD
    return App->input->GetMouseNormalizedMotion();
#else
    return App->editor->GetSceneWindow()->IsActive()
            ? App->input->GetMouseNormalizedMotion()
            : float2::zero;
#endif // PLAY_BUILD
}

const float2& Hachiko::Input::GetMousePixelsMotion()
{
    return App->input->GetMousePixelsMotion();
}

const float2& Hachiko::Input::GetMouseGlobalPixelPosition()
{
    return App->input->GetMouseGlobalPixelPosition();
}

float2 Hachiko::Input::GetMouseNormalizedPosition()
{
#ifdef PLAY_BUILD
    return App->input->GetMouseNormalizedPosition();
#else
    // Get the scene window:
    const WindowScene* __restrict scene_window = App->editor->GetSceneWindow();
    // If the scene window is non existent, return the full screen normalized
    // mouse position:
    if (scene_window == nullptr)
    {
        return App->input->GetMouseNormalizedPosition();
    }

    // Get the mouse position in pixels, which is global to the monitor:
    float2 mouse_position_pixels = App->input->GetMouseGlobalPixelPosition(); 
    // If the scene_window is non-nullptr, clamp the mouse position in pixels
    // to be inside the scene window, if mouse_position_pixels is not inside
    // scene window, it will be the last mouse position that was inside the
    // scene window:
    scene_window->ClampMousePosition(mouse_position_pixels);

    return scene_window->NormalizePositionToScene(mouse_position_pixels);
#endif // PLAY_BUILD
}

float2 Hachiko::Input::GetMouseOpenGLPosition()
{
#ifdef PLAY_BUILD
    return App->input->GetMouseOpenGLPosition();
#else
    // Get the scene window:
    const WindowScene* __restrict scene_window = App->editor->GetSceneWindow();
    
    return ModuleInput::ConvertGlobalPixelToOpenGLPosition(
        scene_window->GetViewportSize(),
        scene_window->GetViewportPosition(),
        Input::GetMouseGlobalPixelPosition()
    );
#endif
}

bool Hachiko::Input::IsGamepadModeOn()
{
    return App->input->IsGamepadModeOn();
}

bool Hachiko::Input::IsGameControllerButtonUp(GameControllerButton id)
{
    return App->input->IsGameControllerButtonUp(static_cast<int>(id));
}

bool Hachiko::Input::IsGameControllerButtonDown(GameControllerButton id)
{
    return App->input->IsGameControllerButtonDown(static_cast<int>(id));
}

float Hachiko::Input::GetAxisNormalized(GameControllerAxis id)
{
    return App->input->GetAxisNormalized(static_cast<int>(id));
}

void Hachiko::Input::GoBrr(float strength, float duration)
{
    App->input->GoBrr(strength, duration);
}

/*---------------------------------------------------------------------------*/

/*SCENE MANAGEMENT-----------------------------------------------------------*/

void Hachiko::SceneManagement::SwitchScene(UID scene_uid)
{
    App->scene_manager->ChangeSceneById(scene_uid);
}

void Hachiko::SceneManagement::SetSkyboxActive(bool v)
{
    App->renderer->SetDrawSkybox(v);
}

Hachiko::GameObject* Hachiko::SceneManagement::Raycast(const float3& origin, const float3& destination, float3* closest_hit, GameObject* parent_filter, bool active_only)
{
    return App->scene_manager->Raycast(origin, destination, closest_hit, parent_filter, active_only);
}

Hachiko::GameObject* Hachiko::SceneManagement::BoundingRaycast(const float3& origin, const float3& destination, GameObject* parent_filter, bool active_only)
{
    return App->scene_manager->Raycast(origin, destination, nullptr, parent_filter, active_only);
}

Hachiko::GameObject* Hachiko::SceneManagement::FindInCurrentScene(
    unsigned long long id)
{
    return App->scene_manager->GetRoot()->Find(id);
}

HACHIKO_API Hachiko::GameObject* Hachiko::SceneManagement::FindInCurrentScene(const char* name)
{
    return App->scene_manager->GetRoot()->GetFirstChildWithName(name);
}

HACHIKO_API std::vector<Hachiko::GameObject*> Hachiko::SceneManagement::Instantiate(unsigned long long prefab_uid, GameObject* parent, unsigned n_instances)
{
    return App->resources->InstantiatePrefab(prefab_uid, parent, n_instances);
}

HACHIKO_API void Hachiko::SceneManagement::Destroy(GameObject* game_object)
{
    App->scene_manager->RemoveGameObject(game_object);
}

/*---------------------------------------------------------------------------*/

/*DEBUG----------------------------------------------------------------------*/

const Hachiko::ComponentCamera* Hachiko::Debug::GetRenderingCamera()
{
    return App->camera->GetRenderingCamera();
}

float Hachiko::Debug::GetFps()
{
    return App->renderer->GetCurrentFps();
}

unsigned int Hachiko::Debug::GetMs()
{
    return App->renderer->GetCurrentMs();
}

void Hachiko::Debug::SetPolygonMode(bool is_fill)
{
    glPolygonMode(GL_FRONT_AND_BACK, is_fill ? GL_FILL : GL_LINE);
}

void Hachiko::Debug::SetVsync(bool is_vsync)
{
    SDL_GL_SetSwapInterval(is_vsync);
}

bool Hachiko::Debug::GetVsync()
{
    return SDL_HINT_RENDER_VSYNC;
}

void Hachiko::Debug::DebugDraw(const OBB& box, float3 color)
{
    ddVec3 p[8];
    // This order was pure trial and error, i dont know how to really do it
    // Using center and points does not show the rotation
    static const int order[8] = {0, 1, 5, 4, 2, 3, 7, 6};
    for (int i = 0; i < 8; ++i)
    {
        p[i] = box.CornerPoint(order[i]);
    }
    dd::box(p, color);
}

HACHIKO_API void Hachiko::Debug::DrawNavmesh(bool is_navmesh)
{
    return App->renderer->SetDrawNavmesh(is_navmesh);
}


/*---------------------------------------------------------------------------*/

/*EDITOR---------------------------------------------------------------------*/

bool Hachiko::Editor::ShowGameObjectDragDropArea(const char* field_name, const char* field_type, GameObject** game_object)
{
    bool changed = false;
    ImGui::PushID(field_name);
    ImGui::PushID(field_type);
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 2));
    ImGui::BeginTable("", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_NoSavedSettings);
    ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch, 0.25f);
    ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch, 0.75f);
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    ImGui::TextWrapped(field_name);
    ImGui::Spacing();
    ImGuiUtils::DisplayTooltip(field_type);

    ImGui::TableNextColumn();
    ImGui::PushItemWidth(-FLT_MIN);

    if ((*game_object) != nullptr && game_object != nullptr)
    {
        ImGui::Text((*game_object)->GetName().c_str());
        ImGui::SameLine();

        if (ImGui::Button(std::string(ICON_FA_XMARK).c_str()))
        {
            *game_object = nullptr;
            changed = true;
        }
    }
    else
    {
        ImGui::TextWrapped("<Drag content here>");
        ImGuiUtils::DisplayTooltip(field_type);
    }

    ImGui::PopItemWidth();
    ImGui::EndTable();
    ImGui::PopStyleVar();
    ImGui::PopID();
    ImGui::PopID();

    if (ImGui::BeginDragDropTarget())
    {
        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject");

        if (payload != nullptr)
        {
            IM_ASSERT(payload->DataSize == sizeof(std::intptr_t*));
            *game_object = *static_cast<GameObject**>(payload->Data);

            HE_LOG("Dropped: %s", (*game_object)->GetName().c_str());

            changed = true;
        }

        ImGui::EndDragDropTarget();
    }
    return changed;
}

void Hachiko::Editor::Show(const char* field_name, int& field)
{
    Widgets::DragFloatConfig cfg;
    cfg.format = "%.f";
    cfg.speed = 1.0f;
    cfg.min = INT_MIN;
    cfg.max = INT_MAX;

    float value = field;
    if (DragFloat(field_name, value, &cfg))
    {
        field = value;
    }
    ImGuiUtils::DisplayTooltip("int");
}

void Hachiko::Editor::Show(const char* field_name, unsigned int& field)
{
    Widgets::DragFloatConfig cfg;
    cfg.format = "%.f";
    cfg.speed = 1.0f;
    cfg.min = 0;
    cfg.max = UINT32_MAX;

    float value = field;
    if (DragFloat(field_name, value, &cfg))
    {
        field = value;
    }
    ImGuiUtils::DisplayTooltip("unsigned int");
}

void Hachiko::Editor::Show(const char* field_name, bool& field)
{
    Widgets::Checkbox(field_name, &field);
    ImGuiUtils::DisplayTooltip("boolean");
}

void Hachiko::Editor::Show(const char* field_name, double& field)
{
    float value = field;
    if(Widgets::DragFloat(field_name, value))
    {
        field = value;
    }
    ImGuiUtils::DisplayTooltip("double");
}

void Hachiko::Editor::Show(const char* field_name, float& field)
{
    Widgets::DragFloat(field_name, field);
    ImGuiUtils::DisplayTooltip("float");
}

void Hachiko::Editor::Show(const char* field_name, float2& field)
{
    Widgets::DragFloat2(field_name, field);
    ImGuiUtils::DisplayTooltip("float2");
}

void Hachiko::Editor::Show(const char* field_name, float3& field)
{
    Widgets::DragFloat3(field_name, field);
    ImGuiUtils::DisplayTooltip("float3");
}

void Hachiko::Editor::Show(const char* field_name, float4& field)
{
    ImGui::DragFloat4(field_name, field.ptr(), 0.1f, -inf, inf);
    ImGui::SameLine();
    ImGuiUtils::DisplayTooltip("float4");
}

void Hachiko::Editor::Show(const char* field_name, Quat& field) 
{
    float3 degrees = RadToDeg(field.ToEulerXYZ());

    if (Widgets::DragFloat3(field_name, degrees))
    {
        const float3 radians = DegToRad(degrees);
        field = Quat::FromEulerXYZ(radians.x, radians.y, radians.z).Normalized();
    }

    ImGuiUtils::DisplayTooltip("Quat");
}

void Hachiko::Editor::Show(const char* field_name, std::string& field)
{
    Widgets::Input(field_name, field);
    ImGuiUtils::DisplayTooltip("string");
}

void Hachiko::Editor::Show(const char* field_name, GameObject*& field)
{
    ShowGameObjectDragDropArea(field_name, "GameObject*", &field);
}

/*---------------------------------------------------------------------------*/

/*NAVIGATION-----------------------------------------------------------------*/

float Hachiko::Navigation::GetHeightFromPosition(const float3& position)
{
    return App->navigation->GetYFromPosition(position);
}

float3 Hachiko::Navigation::GetCorrectedPosition(const float3& position, const float3& extents)
{
    return App->navigation->GetCorrectedPosition(position, extents);
}

void Hachiko::Navigation::CorrectPosition(float3& position, const float3& extents)
{
    return App->navigation->CorrectPosition(position, extents);
}

bool Hachiko::Navigation::Raycast(const float3& start_pos, const float3& end_pos, float3& hit_position)
{
    return App->navigation->Raycast(start_pos, end_pos, hit_position);
}

/*---------------------------------------------------------------------------*/

/*PROPERTIES-----------------------------------------------------------------*/

HACHIKO_API const std::string& Hachiko::FileUtility::GetWorkingDirectory()
{
    return App->file_system.GetWorkingDirectory();
}

/*---------------------------------------------------------------------------*/
