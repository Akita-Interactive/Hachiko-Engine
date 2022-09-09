#include "core/hepch.h"

#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "ModuleSceneManager.h"

#include "components/ComponentTransform.h"
#include "components/ComponentCamera.h"

#include "ui/WindowScene.h"

#include "core/preferences/src/CameraPreferences.h"


Hachiko::ModuleCamera::ModuleCamera() = default;

Hachiko::ModuleCamera::~ModuleCamera() = default;

bool Hachiko::ModuleCamera::Init()
{
    HE_LOG("INITIALIZING MODULE: CAMERA");
    HE_LOG("Creating Editor Camera");
    camera_prefs = App->preferences->GetCameraPreference();

    GameObject* editor_camera_go = new GameObject("Editor Camera");
    editor_camera = static_cast<ComponentCamera*>(editor_camera_go->CreateComponent(Component::Type::CAMERA));
    SetRenderingCamera(editor_camera);

    editor_camera_go->GetTransform()->SetGlobalPosition(camera_prefs->GetPosition());
    editor_camera_go->GetTransform()->LookAtTarget(float3::zero);
    editor_camera_go->Update();

    ImGuizmo::Enable(true);
    return true;
}

UpdateStatus Hachiko::ModuleCamera::Update(const float delta)
{
    CheckImGuizmoViewManipulateUsed();

#ifndef PLAY_BUILD
    if (App->input->IsKeyDown(SDL_SCANCODE_F3))
    {
        ToggleCamera();
    }

    if (editor_camera == rendering_camera || !GameTimer::running)
    {
        Controller(delta);
    }
#endif // !PLAY_BUILD
    return UpdateStatus::UPDATE_CONTINUE;
}

bool Hachiko::ModuleCamera::CleanUp()
{
    // Save to prefs
    camera_prefs->SetPosition(rendering_camera->GetGameObject()->GetTransform()->GetGlobalPosition());

    camera_buffer.erase(camera_buffer.begin());
    camera_buffer.clear();
    App->scene_manager->RemoveGameObject(rendering_camera->GetGameObject());
    return true;
}

void Hachiko::ModuleCamera::OnResize(unsigned int screen_width, unsigned int screen_height)
{
    for (ComponentCamera* camera : camera_buffer)
    {
        camera->SetResolution(screen_width, screen_height);
    }
}

void Hachiko::ModuleCamera::Controller(const float delta)
{
    static const float zoom_speed = 3.0f;
    static const float rot_speed = 2.0f;
    static const float perpendicular_movement_speed = 2.0f;

#ifndef PLAY_BUILD
    if (!App->editor->GetSceneWindow()->IsHovering() && !moving_camera)
    {
        return;
    }
#endif

    // Keyboard movement ---------------
    if (App->input->IsMouseButtonPressed(SDL_BUTTON_RIGHT))
    {
        const float2 moved = App->input->GetMousePixelsMotion();

        Rotate(-moved.x * delta * rot_speed, moved.y * delta * rot_speed);
        MovementController(delta);

        moving_camera = true;
    }

    // Mouse ----------------------------

    const int scrolled_y = App->input->GetScrollDelta();
    if (scrolled_y != 0)
    {
        Zoom(static_cast<float>(scrolled_y) * zoom_speed);
    }

    if (App->input->IsKeyPressed(SDL_SCANCODE_LALT))
    {
        const float2 moved = App->input->GetMousePixelsMotion();
        Orbit(moved.x * 1.5f, moved.y * 1.5f);
    }
    if (App->input->IsKeyPressed(SDL_SCANCODE_F))
    {
        const float distance = (rendering_camera->reference_point - rendering_camera->GetGameObject()->GetTransform()->GetLocalPosition()).Length();
        GameObject* go = App->editor->GetSelectedGameObject();
        if (go != nullptr)
        {
            FocusOnModel(go->GetTransform()->GetLocalPosition(), distance);
        }
    }
    if (App->input->IsMouseButtonPressed(SDL_BUTTON_MIDDLE))
    {
        const float2 moved = App->input->GetMousePixelsMotion();

        PerpendicularMovement(moved.x * delta * perpendicular_movement_speed, moved.y * delta * perpendicular_movement_speed);

        moving_camera = true;
    }

    if (!App->input->IsMouseButtonPressed(SDL_BUTTON_RIGHT) && !App->input->IsMouseButtonPressed(SDL_BUTTON_MIDDLE))
    {
        moving_camera = false;
    }
}

void Hachiko::ModuleCamera::AddCamera(ComponentCamera* camera)
{
    camera_buffer.push_back(camera);
}

void Hachiko::ModuleCamera::RemoveCamera(ComponentCamera* camera)
{
    assert(camera != editor_camera);
    if (rendering_camera == camera)
    {
        RestoreEditorCamera();
    }
    camera_buffer.erase(std::remove(camera_buffer.begin(), camera_buffer.end(), camera), camera_buffer.end());
}

void Hachiko::ModuleCamera::ToggleCamera()
{
    ++camera_idx;
    if (camera_idx >= camera_buffer.size())
    {
        camera_idx = 0;
    }
    SetRenderingCamera(camera_buffer[camera_idx]);
    if (!rendering_camera->GetGameObject()->IsActive())
    {
        ToggleCamera();
    }
}

void Hachiko::ModuleCamera::SetRenderingCamera(ComponentCamera* camera)
{
    rendering_camera = camera;
}

void Hachiko::ModuleCamera::RestoreEditorCamera()
{
    SetRenderingCamera(editor_camera);
}

void Hachiko::ModuleCamera::Zoom(float zoom) const
{
    auto* transform = rendering_camera->GetGameObject()->GetTransform();
    const float distance = rendering_camera->reference_point.Distance(transform->GetLocalPosition());
    const vec newPos = transform->GetLocalPosition() + zoom * transform->GetFront() * distance * 0.05f;

    transform->SetGlobalPosition(newPos);
    rendering_camera->GetGameObject()->Update();
}

void Hachiko::ModuleCamera::Orbit(float motion_x, float motion_y) const
{
    auto* transform = rendering_camera->GetGameObject()->GetTransform();
    float3 vector = transform->GetGlobalPosition() - rendering_camera->reference_point;

    vector = Quat(transform->GetUp(), motion_x * 0.003f).Transform(vector);
    vector = Quat(transform->GetRight().Neg(), motion_y * 0.003f).Transform(vector);

    transform->SetGlobalPosition(vector + rendering_camera->reference_point);

    transform->LookAtTarget(rendering_camera->reference_point);
    rendering_camera->GetGameObject()->Update();
}

void Hachiko::ModuleCamera::MovementController(const float delta) const
{
    static const float move_speed = 15.0f;
    static const float speed_modifier = 2.0f;

    float effective_speed = move_speed;
    if (App->input->IsModifierPressed(KMOD_SHIFT))
    {
        effective_speed *= speed_modifier;
    }

    auto* transform = rendering_camera->GetGameObject()->GetTransform();
    float3 deltaRight = float3::zero, deltaUp = float3::zero, deltaFwd = float3::zero;

    if (App->input->IsKeyPressed(SDL_SCANCODE_W))
    {
        deltaFwd += transform->GetFront() * delta * effective_speed;
    }
    if (App->input->IsKeyPressed(SDL_SCANCODE_S))
    {
        deltaFwd -= transform->GetFront() * delta * effective_speed;
    }
    if (App->input->IsKeyPressed(SDL_SCANCODE_A))
    {
        deltaRight += transform->GetRight() * delta * effective_speed;
    }
    if (App->input->IsKeyPressed(SDL_SCANCODE_D))
    {
        deltaRight -= transform->GetRight() * delta * effective_speed;
    }
    if (App->input->IsKeyPressed(SDL_SCANCODE_Q))
    {
        deltaUp += float3::unitY * delta * effective_speed;
    }
    if (App->input->IsKeyPressed(SDL_SCANCODE_E))
    {
        deltaUp -= float3::unitY * delta * effective_speed;
    }

    transform->SetGlobalPosition(transform->GetGlobalPosition() + deltaFwd + deltaRight + deltaUp);
    rendering_camera->GetGameObject()->Update();
    rendering_camera->reference_point += deltaRight + deltaUp;
}

void Hachiko::ModuleCamera::FocusOnModel(const float3& target, float distance) const
{
    auto* transform = rendering_camera->GetGameObject()->GetTransform();
    const float3 v = transform->GetFront().Neg();

    transform->SetGlobalPosition(target + (v * distance));
    rendering_camera->GetGameObject()->Update();
    rendering_camera->reference_point = target;
}

void Hachiko::ModuleCamera::Rotate(float motion_x, float motion_y) const
{
    auto* transform = rendering_camera->GetGameObject()->GetTransform();

    const Quat yaw_quat = Quat::RotateY(motion_x);
    const float3 newRight = yaw_quat * transform->GetRight();

    const Quat pitch_quat = Quat::RotateAxisAngle(newRight, motion_y);
    const float3 newUp = pitch_quat * yaw_quat * transform->GetUp();
    const float3 newFwd = pitch_quat * yaw_quat * transform->GetFront();

    transform->SetGlobalRotationAxis(newRight, newUp, newFwd);
    rendering_camera->GetGameObject()->Update();

    const float distancetoReference = (rendering_camera->reference_point - transform->GetLocalPosition()).Length();
    rendering_camera->reference_point = transform->GetGlobalPosition() + newFwd * distancetoReference;
}

void Hachiko::ModuleCamera::PerpendicularMovement(float motion_x, float motion_y) const
{
    static const float move_speed = 15.0f;

    ComponentTransform* transform = rendering_camera->GetGameObject()->GetTransform();
    float3 deltaMovement = transform->GetRight() * move_speed * motion_x + transform->GetUp() * move_speed * motion_y;

    transform->SetGlobalPosition(transform->GetGlobalPosition() + deltaMovement);
    rendering_camera->GetGameObject()->Update();
    rendering_camera->reference_point += deltaMovement;
}

void Hachiko::ModuleCamera::CheckImGuizmoViewManipulateUsed() 
{
    float3 frustum_front = rendering_camera->GetFrustum().Front();
    float3 transform_front = rendering_camera->GetGameObject()->GetTransform()->GetFront();
    if (frustum_front.x != transform_front.x || frustum_front.y != transform_front.y || frustum_front.z != transform_front.z)
    {
        ComponentTransform* transform = rendering_camera->GetGameObject()->GetTransform();
        transform->SetGlobalRotationAxis(math::Cross(rendering_camera->GetFrustum().Up(), rendering_camera->GetFrustum().Front()),
                                         rendering_camera->GetFrustum().Up(),
                                         rendering_camera->GetFrustum().Front());
        rendering_camera->GetGameObject()->Update();
    }
}
