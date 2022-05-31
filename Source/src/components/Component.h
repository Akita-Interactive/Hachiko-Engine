#pragma once

#if defined(HACHIKO_API)
// Do Nothing
#elif defined(_MSC_VER)
#define HACHIKO_API __declspec(dllexport)
#endif

#include "utils/UUID.h"
#include "yaml-cpp/yaml.h"

namespace Hachiko
{
    class GameObject;
    class ComponentCamera;
    class Program;

    class HACHIKO_API Component
    {
    public:
        enum class Type
        {
            NONE = 0,
            TRANSFORM = 1,
            MESH_RENDERER = 2,
            //MATERIAL = 3,
            CAMERA = 4,
            DIRLIGHT = 5,
            POINTLIGHT = 6,
            SPOTLIGHT = 7,
            CANVAS = 8,
            CANVAS_RENDERER = 9,
            TRANSFORM_2D = 10,
            IMAGE = 11,
            BUTTON = 12,
            PROGRESS_BAR = 13,
            ANIMATION = 14,
            SCRIPT = 15,
            TEXT = 16,
            AGENT = 17,
            OBSTACLE = 18,
            AUDIO_LISTENER = 19,
            AUDIO_SOURCE = 20,
            BILLBOARD = 21,
            PARTICLE_SYSTEM = 22,
            UNKNOWN
        };

        Component(Type type, GameObject* container, UID id = 0);

        virtual ~Component() = default;

        // --- COMPONENT EVENTS --- //

        virtual void Start()
        {
        }

        virtual void Stop()
        {
        }

        virtual void Update()
        {
        }

        virtual void OnTransformUpdated()
        {
        }

        [[nodiscard]] Type GetType() const
        {
            return type;
        }

        [[nodiscard]] UID GetID() const
        {
            return uid;
        }

        void SetID(const UID new_uid)
        {
            uid = new_uid;
        }

        void Enable()
        {
            active = true;
        }

        void Disable()
        {
            active = false;
        }

        [[nodiscard]] bool IsActive() const;
        [[nodiscard]] const GameObject* GetGameObject() const
        {
            return game_object;
        }

        [[nodiscard]] GameObject* GetGameObject()
        {
            return game_object;
        }

        void SetGameObject(GameObject* container)
        {
            game_object = container;
        }

        virtual void DrawGui()
        {
        }

        virtual void Draw(ComponentCamera* camera, Program* program)
        {
        }

        virtual void DebugDraw()
        {
        }

        virtual void Save(YAML::Node& node) const
        {
        }

        virtual void Load(const YAML::Node& node)
        {
        }

        [[nodiscard]] virtual bool CanBeRemoved() const;
        virtual bool HasDependentComponents(GameObject* game_object) const;

    protected:
        void OverrideUID(UID new_uid)
        {
            uid = new_uid;
        }

    protected:
        GameObject* game_object = nullptr;
        bool active = true;
        Type type = Type::UNKNOWN;
        UID uid = 0;
    };
}
