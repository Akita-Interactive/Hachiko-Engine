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
    return App->input->IsMouseButtonPressed(static_cast<int>(mouse_button));
}

bool Hachiko::Input::IsMouseButtonUp(MouseButton mouse_button)
{
    return App->input->IsMouseButtonUp(static_cast<int>(mouse_button));
}

bool Hachiko::Input::IsMouseButtonDown(MouseButton mouse_button)
{
    return App->input->IsMouseButtonDown(static_cast<int>(mouse_button)); 
}

int Hachiko::Input::GetScrollWheelDelta()
{
    return App->input->GetScrollDelta();
}

const float2& Hachiko::Input::GetMouseNormalizedMotion()
{
    return App->input->GetMouseNormalizedMotion();
}

HACHIKO_API const float2& Hachiko::Input::GetMousePixelsMotion()
{
    return App->input->GetMousePixelsMotion();
}

const float2& Hachiko::Input::GetMousePixelPosition()
{
    return App->input->GetMousePixelPosition();
}

HACHIKO_API const float2& Hachiko::Input::GetMouseNormalizedPosition()
{
    //HE_LOG("Position got as: %f, %f", App->input->GetMouseNormalizedPosition().x, App->input->GetMouseNormalizedPosition().y);
    
    return App->input->GetMouseNormalizedPosition();
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

Hachiko::GameObject* Hachiko::SceneManagement::Raycast(const float3& origin, 
    const float3& destination)
{
    return App->scene_manager->Raycast(origin, destination);
}

Hachiko::GameObject* Hachiko::SceneManagement::BoundingRaycast(const float3& origin, const float3& destination)
{
    return App->scene_manager->Raycast(origin, destination);
}

Hachiko::GameObject* Hachiko::SceneManagement::FindInCurrentScene(
    unsigned long long id)
{
    return App->scene_manager->GetRoot()->Find(id);
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

void Hachiko::Debug::DebugDraw(const OBB& box)
{
    ddVec3 p[8];
    // This order was pure trial and error, i dont know how to really do it
    // Using center and points does not show the rotation
    static const int order[8] = {0, 1, 5, 4, 2, 3, 7, 6};
    for (int i = 0; i < 8; ++i)
    {
        p[i] = box.CornerPoint(order[i]);
    }
    dd::box(p, dd::colors::White);
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

    if ((*game_object) != nullptr)
    {
        ImGui::Text((*game_object)->GetName().c_str());
        
        ImGui::SameLine();
        
        if (ImGui::Button("X"))
        {
            *game_object = nullptr;
            changed = true;
        }
        
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

/*---------------------------------------------------------------------------*/