#pragma once
#include "Module.h"
#include "Globals.h"

#include "core/GameObject.h"

namespace Hachiko
{
    class ComponentCamera;
    class CameraPreferences;

    class ModuleCamera : public Module
    {
    public:
        ModuleCamera();
        ~ModuleCamera() override;

        bool Init() override;
        UpdateStatus Update(const float delta) override;
        bool CleanUp() override;

        void OnResize(unsigned int screen_width, unsigned int screen_height);

        void Controller(float delta);
        void MovementController(float delta) const;

        [[nodiscard]] ComponentCamera* GetRenderingCamera() 
        {
            return rendering_camera;
        }

        [[nodiscard]] ComponentCamera* GetEditorCamera()
        {
            return editor_camera;
        }

        void AddCamera(ComponentCamera* camera);
        void RemoveCamera(ComponentCamera* camera);
        void ToggleCamera();
        void SetRenderingCamera(ComponentCamera* camera);
        void RestoreEditorCamera();

        // Camera actions
        void Zoom(float zoom) const;
        void Orbit(float motion_x, float motion_y) const;
        void FocusOnModel(const float3& target, float distance) const;
        void Rotate(float motion_x, float motion_y) const;
        void PerpendicularMovement(float motion_x, float motion_y) const;

        void CheckImGuizmoViewManipulateUsed();

    private:
        
        ComponentCamera* editor_camera = nullptr;
        ComponentCamera* rendering_camera = nullptr;
        CameraPreferences* camera_prefs = nullptr;

        std::vector<ComponentCamera*> camera_buffer;
        unsigned int camera_idx = 0;

        bool moving_camera = false;
    };
}
