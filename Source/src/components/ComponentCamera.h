#pragma once

#include "components/Component.h"
#include <MathGeoLib.h>

#define DEFAULT_CAMERA_WIDTH 1920
#define DEFAULT_CAMERA_HEIGHT 1080

#if defined(HACHIKO_API)
// Do Nothing
#elif defined(_MSC_VER)
#define HACHIKO_API __declspec(dllexport)
#endif

// TODO: Only expose some methods of ComponentCamera to Scripting. Not all of 
// them.

namespace Hachiko
{
    class GameObject;

    class HACHIKO_API ComponentCamera : public Component
    {
    public:
        enum class CameraType
        {
            STATIC,
            DYNAMIC,
            GOD,
            PLAYER
        };

        ComponentCamera(GameObject* conatiner);
        ~ComponentCamera() override;

        void OnTransformUpdated() override;

        [[nodiscard]] float4x4 GetViewMatrix(bool transpose = false) const;
        [[nodiscard]] float4x4 GetProjectionMatrix(bool transpose = false) const;

        Frustum* GetFrustum()
        {
            return &frustum;
        }

        void SetCameraType(CameraType cam_type);
        void SetCameraInitialPos();
        CameraType GetCameraType() const;
        std::string GetCameraTypeString(CameraType cam_type);

        void SetNearPlane(float distance);
        void SetFarPlane(float distance);
        void SetPlaneDistances(float near_distance, float far_distance);
        void SetHorizontalFov(float fov_deg);

        void SetResolution(unsigned width, unsigned height);
        void GetResolution(unsigned& width, unsigned& height) const;

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

        void DrawGui() override;

        void DebugDraw() override;

        LineSegment Raycast(float x, float y) const;
        LineSegment Raycast(const float2& from_position) const;

        static float2 ScreenPositionToView(float x, float y);
        static float2 ScreenPositionToView(const float2& screen_position);

        Plane planes[6];
        float3 reference_point = float3::zero;
        bool draw_frustum = false;
        bool preview_cam = false;
        float3 camera_pinned_pos = float3::zero;

    private:
        float horizontal_fov;
        Frustum frustum;
        CameraType camera_type;
        unsigned resolution_x = DEFAULT_CAMERA_WIDTH;
        unsigned resolution_y = DEFAULT_CAMERA_HEIGHT;
    };
}
