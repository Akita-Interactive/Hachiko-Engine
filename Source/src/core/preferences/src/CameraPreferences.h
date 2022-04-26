#pragma once

#include "core/preferences/Preferences.h"

namespace Hachiko
{
    class CameraPreferences final : public Preferences
    {
    public:
        CameraPreferences();
        ~CameraPreferences() override = default;
        void LoadConfigurationData(const YAML::Node& node) override;
        void SaveConfigurationData(YAML::Node& node) override;

        [[nodiscard]] unsigned int GetMoveSpeed() const { return move_speed; }
        [[nodiscard]] unsigned int GetRotationSpeed() const { return rotation_speed; }
        [[nodiscard]] unsigned int GetOrbitSpeed() const { return orbit_speed; }
        [[nodiscard]] unsigned int GetZoomSpeed() const { return zoom_speed; }
        [[nodiscard]] float GetFOV() const { return fov; }
        [[nodiscard]] float GetNearDistance() const { return near_distance; }
        [[nodiscard]] float GetFarDistance() const { return far_distance; }

        void SetMoveSpeed(unsigned int moveSpeed) { move_speed = moveSpeed; }
        void SetRotationSpeed(unsigned int rotationSpeed) { rotation_speed = rotationSpeed; }
        void SetOrbitSpeed(unsigned int orbitSpeed) { orbit_speed = orbitSpeed; }
        void SetZoomSpeed(unsigned int zoomSpeed) { zoom_speed = zoomSpeed; }
        void SetFOV(float fov) { this->fov = fov; }
        void SetNearDistance(float nearDistance) { near_distance = nearDistance; }
        void SetFarDistance(float farDistance) { far_distance = farDistance; }

    private:
        unsigned int move_speed = 15;
        unsigned int rotation_speed = 15;
        unsigned int orbit_speed = 25;
        unsigned int zoom_speed = 50;
        float fov = 90.0;
        float near_distance = 0.1;
        float far_distance = 500.0;
    };
}
