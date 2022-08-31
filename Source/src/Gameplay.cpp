#include "core/hepch.h"
#include "modules/ModuleInput.h"
#include "modules/ModuleSceneManager.h"
#include "modules/ModuleCamera.h"
#include "modules/ModuleAudio.h"
#include "modules/ModuleRender.h"
#include "modules/ModuleResources.h"
#include "Gameplay.h"
#include "modules/ModuleNavigation.h"
#include "modules/ModuleDebugDraw.h"
#include "utils/FileSystem.h"
#include "debugdraw.h"

#include "components/ComponentAgent.h"


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

const float2& Hachiko::Input::GetMousePixelPosition()
{
    return App->input->GetMousePixelPosition();
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
    float2 mouse_position_pixels = App->input->GetMousePixelPosition(); 
    // If the scene_window is non-nullptr, clamp the mouse position in pixels
    // to be inside the scene window, if mouse_position_pixels is not inside
    // scene window, it will be the last mouse position that was inside the
    // scene window:
    scene_window->ClampMousePosition(mouse_position_pixels);

    return scene_window->NormalizePositionToScene(mouse_position_pixels);
#endif // PLAY_BUILD
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

void Hachiko::Editor::ShowGameObjectDragDropArea(const char* field_name, 
    const char* field_type, Hachiko::GameObject** game_object, bool& changed)
{
    changed = false;

    if ((*game_object) != nullptr && game_object != nullptr)
    {
        ImGui::Text((*game_object)->GetName().c_str());
        
        ImGui::SameLine();
        
        ImGui::PushID(StringUtils::Concat(field_type, "@", field_name, ":CloseButton").c_str());
        if (ImGui::Button("X"))
        {
            *game_object = nullptr;
            changed = true;
        }
        ImGui::PopID();
        
        ImGui::SameLine();
    }

    // Form the label:
    std::string label = field_name;
    label = label + " (" + field_type + ")";

    ImGui::Selectable(label.c_str());
    
    if (ImGui::BeginDragDropTarget())
    {
        const ImGuiPayload* payload = 
            ImGui::AcceptDragDropPayload("GameObject");

        if (payload != nullptr)
        {
            IM_ASSERT(payload->DataSize == sizeof(std::intptr_t*));
            *game_object = *(GameObject**)payload->Data;

            HE_LOG("Drop: %s", (*game_object)->GetName().c_str());

            changed = true;
        }

        ImGui::EndDragDropTarget();
    }
}

void Hachiko::Editor::Show(const char* field_name, int& field)
{
    ImGui::DragInt(field_name, &field, 1.0f, INT_MIN, INT_MAX);
    ImGui::SameLine();
    ImGui::Text(" (int)");
}

void Hachiko::Editor::Show(const char* field_name, unsigned int& field)
{
    unsigned int uint_max = UINT32_MAX;
    unsigned int uint_min = 0;

    ImGui::DragScalar(field_name, 
        ImGuiDataType_U32, (void*)(&field), 1.0f, 
        (void*)(&uint_min), (void*)(&uint_max));
    ImGui::SameLine();
    ImGui::Text(" (unsigned int)");
}

void Hachiko::Editor::Show(const char* field_name, float& field)
{
    ImGui::DragFloat(field_name, &field, 0.1f);
    ImGui::SameLine();
    ImGui::Text(" (float)");
}

void Hachiko::Editor::Show(const char* field_name, double& field)
{
    ImGui::InputDouble(field_name, &field, 0.1, 1.0);
    ImGui::SameLine();
    ImGui::Text(" (double)");
}

void Hachiko::Editor::Show(const char* field_name, bool& field)
{
    ImGui::Checkbox(field_name, &field);
    ImGui::SameLine();
    ImGui::Text(" (bool)");
}

void Hachiko::Editor::Show(const char* field_name, math::float2& field)
{
    ImGui::DragFloat2(field_name, field.ptr(), 0.1f, -inf, inf);
    ImGui::SameLine();
    ImGui::Text(" (float2)");
}

void Hachiko::Editor::Show(const char* field_name, math::float3& field)
{
    ImGui::DragFloat3(field_name, field.ptr(), 0.1f, -inf, inf);
    ImGui::SameLine();
    ImGui::Text(" (float3)");
}

void Hachiko::Editor::Show(const char* field_name, math::float4& field)
{
    ImGui::DragFloat4(field_name, field.ptr(), 0.1f, -inf, inf);
    ImGui::SameLine();
    ImGui::Text(" (float4)");
}

void Hachiko::Editor::Show(const char* field_name, math::Quat& field) 
{
    math::float3 degrees = RadToDeg(field.ToEulerXYZ());

    if (ImGui::DragFloat3(field_name, degrees.ptr(), 0.1f, -inf, inf))
    {
        math::float3 radians = DegToRad(degrees);
        field = Quat::FromEulerXYZ(radians.x, radians.y, radians.z).Normalized();
    }

    ImGui::SameLine();
    ImGui::Text(" (Quat)");
}

void Hachiko::Editor::Show(const char* field_name, std::string& field)
{
    ImGui::InputText(field_name, &field);
    ImGui::SameLine();
    ImGui::Text(" (string)");
}

void Hachiko::Editor::Show(const char* field_name, GameObject*& field)
{
    // We don't need this here, just used to supply to the function.
    bool changed = false;

    ShowGameObjectDragDropArea(field_name, "GameObject*", &field, changed);
}

/*---------------------------------------------------------------------------*/

/*NAVIGATION-----------------------------------------------------------------*/

float Hachiko::Navigation::GetHeightFromPosition(const math::float3& position)
{
    return App->navigation->GetYFromPosition(position);    
}

math::float3 Hachiko::Navigation::GetCorrectedPosition(const math::float3& position, const math::float3& extents)
{
    return App->navigation->GetCorrectedPosition(position, extents);
}

void Hachiko::Navigation::CorrectPosition(math::float3& position, const math::float3& extents)
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