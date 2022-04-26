#include "core/hepch.h"

#include "modules/ModuleSceneManager.h"
#include "modules/ModuleCamera.h"

#include "ComponentCamera.h"
#include "ComponentTransform.h"

#include <debugdraw.h>

Hachiko::ComponentCamera::ComponentCamera(GameObject* container) :
    Component(Type::CAMERA, container),
    horizontal_fov(65.0f)
{
    frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
    frustum.SetViewPlaneDistances(0.1f, 1000000000000.0f);
    SetCameraType(CameraType::STATIC);
    HE_LOG("TEST LOG: INIT COMPONENT CAMERA");
    frustum.SetHorizontalFovAndAspectRatio(DegToRad(horizontal_fov), static_cast<float>(DEFAULT_CAMERA_WIDTH) / static_cast<float>(DEFAULT_CAMERA_HEIGHT));

    frustum.SetPos(float3(0.0f, 0.0f, 0.0f));
    frustum.SetFront(float3x3::identity.WorldZ());
    frustum.SetUp(float3x3::identity.WorldY());

    frustum.GetPlanes(planes);
    ComponentCamera::OnTransformUpdated();
    App->camera->AddCameraComponent(this);
}

Hachiko::ComponentCamera::~ComponentCamera()
{
    
    if (game_object->scene_owner)
    {
        App->camera->RemoveCameraComponent(this);
        App->camera->RestoreOriginCamera();
        if (game_object->scene_owner->GetCullingCamera() == this)
        {
            game_object->scene_owner->SetCullingCamera(App->camera->GetMainCamera());
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

void Hachiko::ComponentCamera::SetCameraType(CameraType cam_type) 
{
    camera_type = cam_type;
}
void Hachiko::ComponentCamera::SetCameraInitialPos()
{
    camera_pinned_pos = GetGameObject()->GetTransform()->GetGlobalPosition();
}
Hachiko::ComponentCamera::CameraType Hachiko::ComponentCamera::GetCameraType() const 
{
    return camera_type;
}

std::string Hachiko::ComponentCamera::GetCameraTypeString(CameraType cam_type)
{
    switch (cam_type)
    {
    case CameraType::STATIC:
        return std::string("STATIC");
        break;
    case CameraType::DYNAMIC:
        return std::string("DYNAMIC");
        break;
    case CameraType::GOD:
        return std::string("GOD");
        break;
    case CameraType::PLAYER:
        return std::string("PLAYER");
        break;

    default:
        return std::string("UNKNOWN");
    }
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

    frustum.SetFrame(transform->GetGlobalPosition(), transform->GetFront(), transform->GetUp());
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

LineSegment Hachiko::ComponentCamera::RayCast(float x, float y) const
{
    return frustum.UnProjectLineSegment(x, y);
}

void Hachiko::ComponentCamera::Save(JsonFormatterValue j_component) const
{
    const JsonFormatterValue j_frustum = j_component["Frustrum"];
    j_frustum["NearDistance"] = frustum.NearPlaneDistance();
    j_frustum["FarDistance"] = frustum.FarPlaneDistance();
    j_frustum["Fov"] = horizontal_fov;
    j_frustum["CameraType"] = static_cast<int>(camera_type);
    

    const JsonFormatterValue j_pos = j_frustum["Pos"];
    j_pos[0] = frustum.Pos().x;
    j_pos[1] = frustum.Pos().y;
    j_pos[2] = frustum.Pos().z;

    const JsonFormatterValue j_front = j_frustum["Front"];
    j_front[0] = frustum.Front().x;
    j_front[1] = frustum.Front().y;
    j_front[2] = frustum.Front().z;

    const JsonFormatterValue j_up = j_frustum["Up"];
    j_up[0] = frustum.Up().x;
    j_up[1] = frustum.Up().y;
    j_up[2] = frustum.Up().z;

    const JsonFormatterValue j_pinned_camera = j_frustum["PinnedCamera"];
    j_pinned_camera[0] = camera_pinned_pos.x;
    j_pinned_camera[1] = camera_pinned_pos.y;
    j_pinned_camera[2] = camera_pinned_pos.z;
}

void Hachiko::ComponentCamera::Load(JsonFormatterValue j_component)
{
    const JsonFormatterValue j_frustum = j_component["Frustrum"];

    SetNearPlane(j_frustum["NearDistance"]);
    SetFarPlane(j_frustum["FarDistance"]);
    SetHorizontalFov(j_frustum["Fov"]);
    SetCameraType(static_cast<CameraType>(static_cast<int>(j_frustum["CameraType"])));

    const JsonFormatterValue j_pos = j_frustum["Pos"];
    const JsonFormatterValue j_front = j_frustum["Front"];
    const JsonFormatterValue j_up = j_frustum["Up"];
    const JsonFormatterValue j_pinned_camera = j_frustum["PinnedCamera"];

    frustum.SetPos(float3(j_pos[0], j_pos[1], j_pos[2]));
    frustum.SetFront(float3(j_front[0], j_front[1], j_front[2]));
    frustum.SetUp(float3(j_up[0], j_up[1], j_up[2]));
    camera_pinned_pos = float3(j_pinned_camera[0], j_pinned_camera[1], j_pinned_camera[2]);
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
            ImGui::Checkbox("Preview Camera", &preview_cam);
            if (preview_cam)
            {
                App->camera->SetMainCamera(this);
            }
            else
                App->camera->RestoreOriginCamera();
        #endif
        float planes[2] = {frustum.NearPlaneDistance(), frustum.FarPlaneDistance()};
        if (ImGui::InputFloat2("N & F", &planes[0]))
        {
            SetPlaneDistances(planes[0], planes[1]);
        }
        if (ImGui::SliderFloat("H. Fov", &horizontal_fov, 30.f, 180.f))
        {
            SetHorizontalFov(horizontal_fov);
        }
        if (ImGui::Button("Culling Camera"))
        {
            App->scene_manager->GetActiveScene()->SetCullingCamera(this);
        }

        #ifndef PLAY_BUILD
            int cam_type_selector = static_cast<int>(camera_type);
            if (ImGui::RadioButton("Static", &cam_type_selector, static_cast<int>(CameraType::STATIC)))
            {
                SetCameraType(CameraType::STATIC);
            };
            ImGui::SameLine();
            if (ImGui::RadioButton("Dynamic", &cam_type_selector, static_cast<int>(CameraType::DYNAMIC)))
            {
                SetCameraType(CameraType::DYNAMIC);
            };
            ImGui::SameLine();
            if (ImGui::RadioButton("God", &cam_type_selector, static_cast<int>(CameraType::GOD)))
            {
                SetCameraType(CameraType::GOD);
            };
            ImGui::SameLine();
            if (ImGui::RadioButton("Player", &cam_type_selector, static_cast<int>(CameraType::PLAYER)))
            {
                SetCameraType(CameraType::PLAYER);
            };

            if (ImGui::Button("Set Initial Position"))
            {
                SetCameraInitialPos();
            }
        #endif
        ImGui::Checkbox("Debug", &debug_data);
        if (debug_data)
        {
            ImGui::Separator();
            ImGui::Text("Fov (H, V): %.2f, %.2f", RadToDeg(frustum.HorizontalFov()), RadToDeg(frustum.VerticalFov()));
            ImGui::Text("Aspect Ratio: %.2f", frustum.AspectRatio());
            ImGui::Text("Camera Type: %s", GetCameraTypeString(camera_type).c_str());
            ImGui::Text("Distance from initial point: %f", App->camera->GetMainCamera()->camera_pinned_pos.Distance(App->camera->GetMainCamera()->GetGameObject()->GetTransform()->GetGlobalPosition()));
        }
        
    }
    ImGui::PopID();
}
