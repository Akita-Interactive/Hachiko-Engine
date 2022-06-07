#include "core/hepch.h"

#include "modules/ModuleSceneManager.h"
#include "modules/ModuleCamera.h"
#include "modules/ModuleRender.h"
#include "modules/ModuleEvent.h"

#include "ComponentCamera.h"
#include "ComponentTransform.h"

#include <debugdraw.h>

Hachiko::ComponentCamera::ComponentCamera(GameObject* container) :
    Component(Type::CAMERA, container),
    horizontal_fov(65.0f)
{
    frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
    frustum.SetViewPlaneDistances(0.1f, 1000.0f);
    
    float2 frame_buffer_size = App->renderer->GetFrameBufferSize();    
    SetResolution(frame_buffer_size.x, frame_buffer_size.y);

    frustum.SetPos(float3(0.0f, 0.0f, 0.0f));
    frustum.SetFront(float3x3::identity.WorldZ());
    frustum.SetUp(float3x3::identity.WorldY());

    frustum.GetPlanes(planes);
    ComponentCamera::OnTransformUpdated();
    App->camera->AddCamera(this);
}

Hachiko::ComponentCamera::~ComponentCamera()
{
    if (game_object->scene_owner)
    {
        App->camera->RemoveCamera(this);
        if (game_object->scene_owner->GetCullingCamera() == this)
        {
            game_object->scene_owner->SetCullingCamera(App->camera->GetRenderingCamera());
        }
    }
}

void Hachiko::ComponentCamera::DebugDraw()
{
    if (draw_frustum)
    {
        float4x4 inverse_clip;
        float4x4 matrix = GetProjectionMatrix() * GetViewMatrix();
        matrix.Inverse();
        dd::frustum(matrix, dd::colors::Yellow);
    }
}

void Hachiko::ComponentCamera::SetCameraInitialPos()
{
    camera_pinned_pos = GetGameObject()->GetTransform()->GetGlobalPosition();
}


void Hachiko::ComponentCamera::SetNearPlane(float distance)
{
    SetPlaneDistances(distance, frustum.FarPlaneDistance());
}

void Hachiko::ComponentCamera::SetFarPlane(float distance)
{
    SetPlaneDistances(frustum.NearPlaneDistance(), distance);
}

void Hachiko::ComponentCamera::SetPlaneDistances(const float near_distance, const float far_distance)
{
    frustum.SetViewPlaneDistances(near_distance, far_distance);
    frustum.GetPlanes(planes);
}

void Hachiko::ComponentCamera::SetHorizontalFov(float fov_deg)
{
    horizontal_fov = fov_deg;
    frustum.SetHorizontalFovAndAspectRatio(DegToRad(horizontal_fov), frustum.AspectRatio());
    frustum.GetPlanes(planes);
}


float4x4 Hachiko::ComponentCamera::GetViewMatrix(const bool transpose) const
{
    if (transpose)
    {
        return float4x4(frustum.ViewMatrix()).Transposed();
    }
    return float4x4(frustum.ViewMatrix());
}

float4x4 Hachiko::ComponentCamera::GetProjectionMatrix(const bool transpose) const
{
    if (transpose)
    {
        return frustum.ProjectionMatrix().Transposed();
    }
    return frustum.ProjectionMatrix();
}

void Hachiko::ComponentCamera::OnTransformUpdated()
{
    ComponentTransform* transform = game_object->GetTransform();

    frustum.SetPos(transform->GetGlobalPosition());
    frustum.SetFront(transform->GetFront());
    frustum.SetUp(transform->GetUp());

    frustum.GetPlanes(planes);
}

void Hachiko::ComponentCamera::SetResolution(unsigned width, unsigned height)
{
    if (resolution_x != width || resolution_y != height)
    {
        resolution_x = width;
        resolution_y = height;
        frustum.SetHorizontalFovAndAspectRatio(DegToRad(horizontal_fov), (static_cast<float>(width) / static_cast<float>(height)));
    }
}

void Hachiko::ComponentCamera::GetResolution(unsigned& width, unsigned& height) const
{
    width = resolution_x;
    height = resolution_y;
}

LineSegment Hachiko::ComponentCamera::Raycast(float x, float y) const
{
    return frustum.UnProjectLineSegment(x, y);
}

float2 Hachiko::ComponentCamera::ScreenPositionToView(float x, float y)
{
    return float2(x - 0.5f, (y - 0.5f) * -1.0f);
}

LineSegment Hachiko::ComponentCamera::Raycast(const float2& from_position) const
{
    return Raycast(from_position.x, from_position.y);
}

float2 Hachiko::ComponentCamera::ScreenPositionToView(const float2& screen_position)
{
    return ScreenPositionToView(screen_position.x, screen_position.y);
}

void Hachiko::ComponentCamera::Save(YAML::Node& node) const
{
    node.SetTag("camera");
    YAML::Node node_frustum = node[FRUSTUM];
    node_frustum[NEAR_DISTANCE] = frustum.NearPlaneDistance();
    node_frustum[FAR_DISTANCE] = frustum.FarPlaneDistance();
    node_frustum[FOV] = horizontal_fov;
    node_frustum[CAMERA_POSITION] = frustum.Pos();
    node_frustum[CAMERA_FRONT] = frustum.Front();
    node_frustum[CAMERA_UP] = frustum.Up();
    node_frustum[PINNED_CAMERA] = camera_pinned_pos;
}

void Hachiko::ComponentCamera::Load(const YAML::Node& node)
{
    const YAML::Node node_frustum = node[FRUSTUM];

    SetNearPlane(node_frustum[NEAR_DISTANCE].as<float>());
    SetFarPlane(node_frustum[FAR_DISTANCE].as<float>());
    SetHorizontalFov(node_frustum[FOV].as<float>());
    frustum.SetPos(node_frustum[CAMERA_POSITION].as<float3>());
    frustum.SetFront(node_frustum[CAMERA_FRONT].as<float3>());
    frustum.SetUp(node_frustum[CAMERA_UP].as<float3>());
    camera_pinned_pos = node_frustum[PINNED_CAMERA].as<float3>();
}

void Hachiko::ComponentCamera::DrawGui()
{
    ImGui::PushID(this);
    static bool debug_data = false;

    if (ImGuiUtils::CollapsingHeader(game_object, this, "Camera"))
    {
        ImGui::Checkbox("Draw Frustum", &draw_frustum);

        ImGui::SameLine();
        #ifndef PLAY_BUILD
            if(ImGui::Checkbox("Preview Camera", &preview_cam))
            {
                if (preview_cam)
                {
                    App->camera->SetRenderingCamera(this);
                }
                else
                {
                    App->camera->RestoreEditorCamera();
                }
            }
            
        #endif
        float planes[2] = {frustum.NearPlaneDistance(), frustum.FarPlaneDistance()};
        if (ImGui::InputFloat2("N & F", &planes[0]))
        {
            SetPlaneDistances(planes[0], planes[1]);
        }
        CREATE_HISTORY_ENTRY_AFTER_EDIT()

        if (ImGui::SliderFloat("H. Fov", &horizontal_fov, 30.f, 178.f))
        {
            SetHorizontalFov(horizontal_fov);
        }
        CREATE_HISTORY_ENTRY_AFTER_EDIT()

        if (ImGui::Button("Culling Camera"))
        {
            App->scene_manager->GetActiveScene()->SetCullingCamera(this);
        }

        ImGui::Checkbox("Debug", &debug_data);
        if (debug_data)
        {
            ImGui::Separator();
            ImGui::Text("Fov (H, V): %.2f, %.2f", RadToDeg(frustum.HorizontalFov()), RadToDeg(frustum.VerticalFov()));
            ImGui::Text("Aspect Ratio: %.2f", frustum.AspectRatio());
            ImGui::Text("Distance from initial point: %f", App->camera->GetRenderingCamera()->camera_pinned_pos.Distance(App->camera->GetRenderingCamera()->GetGameObject()->GetTransform()->GetGlobalPosition()));
        }
        
    }
    ImGui::PopID();
}
