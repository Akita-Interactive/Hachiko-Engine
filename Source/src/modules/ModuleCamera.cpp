#include "core/hepch.h"

#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"

#include "components/ComponentTransform.h"
#include "components/ComponentCamera.h"

#include "ui/WindowScene.h"


Hachiko::ModuleCamera::ModuleCamera() = default;

Hachiko::ModuleCamera::~ModuleCamera() = default;

bool Hachiko::ModuleCamera::Init()
{
    HE_LOG("Creating Editor Camera");

    editor_camera_game_object = new GameObject("Editor Camera");
    rendering_camera = static_cast<ComponentCamera*>(
        editor_camera_game_object->CreateComponent(Component::Type::CAMERA));

    camera_buffer.clear();
    camera_buffer.push_back(rendering_camera);
    
    rendering_camera->SetCameraType(ComponentCamera::CameraType::GOD);
    editor_camera_game_object->GetTransform()->SetGlobalPosition(float3(0.0f, 8.0f, 10.0f));
    editor_camera_game_object->GetTransform()->LookAtTarget(float3::zero);
    editor_camera_game_object->Update();

    ImGuizmo::Enable(true);
    return true;
}

UpdateStatus Hachiko::ModuleCamera::Update(const float delta)
{
    if (App->input->GetKey(SDL_SCANCODE_C) == KeyState::KEY_DOWN)
    {
        int buffer_size = camera_buffer.size();
        if (last_it < buffer_size)
        {
            rendering_camera = static_cast<ComponentCamera*>(camera_buffer[last_it]);
            editor_camera_game_object = camera_buffer[last_it]->GetGameObject();
            last_it++;
        }
        else
        {
            last_it = 0;
            rendering_camera = static_cast<ComponentCamera*>(camera_buffer[last_it]);
            editor_camera_game_object = camera_buffer[last_it]->GetGameObject();
            last_it++;
        }
    }
    RunDynamicScript(GameTimer::delta_time); //Wokr around function until scripting operative
    Controller(delta);
    return UpdateStatus::UPDATE_CONTINUE;
}

bool Hachiko::ModuleCamera::CleanUp()
{
    camera_buffer.erase(camera_buffer.begin());
    delete editor_camera_game_object;
    camera_buffer.clear();
    return true;
}

void Hachiko::ModuleCamera::OnResize(unsigned int screen_width, unsigned int screen_height) const
{
    rendering_camera->SetResolution(screen_width, screen_height);
    if (rendering_camera != nullptr)
    {
        rendering_camera->SetResolution(screen_width, screen_height);
    }
}

void Hachiko::ModuleCamera::Controller(const float delta) const
{
    static const float zoom_speed = 3.0f;
    static const float rot_speed = 2.0f;
    static const float perpendicular_movement_speed = 2.0f;

#ifndef PLAY_BUILD
    if (!App->editor->GetSceneWindow()->IsHovering())
    {
        return;
    }
#endif  
    if (rendering_camera->GetCameraType() != ComponentCamera::CameraType::GOD)
    {
        return;
    }
    // Keyboard movement ---------------
    if (App->input->IsMouseButtonPressed(SDL_BUTTON_RIGHT))
    {
        const float2 moved = App->input->GetMousePixelsMotion();

        Rotate(-moved.x * delta * rot_speed, moved.y * delta * rot_speed);
        MovementController(delta);
    }

    // Mouse ----------------------------

    const int scrolled_y = App->input->GetScrollDelta();
    if (scrolled_y != 0)
    {
        Zoom(static_cast<float>(scrolled_y) * zoom_speed);
    }

    if (App->input->GetKey(SDL_SCANCODE_LALT) == KeyState::KEY_REPEAT)
    {
        const float2 moved = App->input->GetMousePixelsMotion();
        Orbit(moved.x * 1.5f, moved.y * 1.5f);
    }
    if (App->input->GetKey(SDL_SCANCODE_F) == KeyState::KEY_DOWN)
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
    }
}

void Hachiko::ModuleCamera::AddCameraComponent(ComponentCamera* camera)
{
    camera_buffer.push_back(camera);
}

void Hachiko::ModuleCamera::RemoveCameraComponent(ComponentCamera* camera)
{
    camera_buffer.erase(std::remove(camera_buffer.begin(), camera_buffer.end(), camera), camera_buffer.end());
}

void Hachiko::ModuleCamera::SetRenderingCamera(ComponentCamera* camera)
{
    rendering_camera = static_cast<ComponentCamera*>(camera);
    editor_camera_game_object = camera->GetGameObject();
}

void Hachiko::ModuleCamera::RestoreOriginCamera()
{
    if (rendering_camera->GetCameraType() == ComponentCamera::CameraType::DYNAMIC) //Return Dynamic camera to pinned position
    {
        rendering_camera->GetGameObject()->GetTransform()->SetLocalPosition(rendering_camera->camera_pinned_pos);
    }
    rendering_camera = static_cast<ComponentCamera*>(camera_buffer[0]);
    editor_camera_game_object = camera_buffer[0]->GetGameObject();
}

void Hachiko::ModuleCamera::ReturnPlayerCamera()
{
    for (auto it = camera_buffer.rbegin(); it != camera_buffer.rend(); ++it)
    {
        if (static_cast<ComponentCamera*>(*it)->GetCameraType() == ComponentCamera::CameraType::PLAYER)
        {
            SetRenderingCamera(*it);
            break;
        }
    }
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

    if (App->input->GetKey(SDL_SCANCODE_W) == KeyState::KEY_REPEAT)
    {
        deltaFwd += transform->GetFront() * delta * effective_speed;
    }
    if (App->input->GetKey(SDL_SCANCODE_S) == KeyState::KEY_REPEAT)
    {
        deltaFwd -= transform->GetFront() * delta * effective_speed;
    }
    if (App->input->GetKey(SDL_SCANCODE_A) == KeyState::KEY_REPEAT)
    {
        deltaRight += transform->GetRight() * delta * effective_speed;
    }
    if (App->input->GetKey(SDL_SCANCODE_D) == KeyState::KEY_REPEAT)
    {
        deltaRight -= transform->GetRight() * delta * effective_speed;
    }
    if (App->input->GetKey(SDL_SCANCODE_Q) == KeyState::KEY_REPEAT)
    {
        deltaUp += float3::unitY * delta * effective_speed;
    }
    if (App->input->GetKey(SDL_SCANCODE_E) == KeyState::KEY_REPEAT)
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
    auto* transform = editor_camera_game_object->GetTransform();

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

void Hachiko::ModuleCamera::RunDynamicScript(const float delta)
{
    if (rendering_camera->GetCameraType() == ComponentCamera::CameraType::DYNAMIC)
    {
        static const float move_speed = 10.0f;
        static const float max_distance = 25.0f;
        float effective_speed = move_speed;

        auto* transform = rendering_camera->GetGameObject()->GetTransform();

        float3 deltaRight = float3::zero, deltaUp = float3::zero, deltaFwd = float3::zero;
        const float distanceFromReference = rendering_camera->camera_pinned_pos.Distance(transform->GetLocalPosition());
        if (distanceFromReference < max_distance)
        {
            deltaFwd += transform->GetFront() * delta * effective_speed;
            transform->SetLocalPosition(transform->GetGlobalPosition() + deltaFwd + deltaRight + deltaUp);
        }
        else
        {
            transform->SetLocalPosition(rendering_camera->camera_pinned_pos);
        }
    }
}
