#pragma once

#include "core/serialization/ISerializable.h"
#include "utils/UUID.h"

#if defined(HACHIKO_API)
// Do Nothing
#elif defined(_MSC_VER)
#define HACHIKO_API __declspec(dllexport)
#endif

namespace Hachiko
{
    class GameObject;
    class ComponentCamera;
    class Program;

    class HACHIKO_API Component : public ISerializable
    {
    public:
        enum class Type
        {
            NONE = 0,
            TRANSFORM = 1,
            MESH = 2,
            MATERIAL = 3,
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
            UNKNOWN
        };

        Component(const Type type, GameObject* container, UID id = 0);

        virtual ~Component() = default;

        // --- COMPONENT EVENTS --- //

        virtual void Start() { }
        virtual void Stop() {};
        virtual void Update() { }
        virtual void OnTransformUpdated() {}

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

        [[nodiscard]] bool IsActive() const
        {
            return active;
        }

        [[nodiscard]] const GameObject* GetGameObject() const
        {
            return game_object;
        }

        GameObject* GetGameObject()
        {
            return game_object;
        }

        void SetGameObject(GameObject* container)
        {
            game_object = container;
        }

        virtual void DrawGui() {}

        virtual void Draw(ComponentCamera* camera, Program* program) {}

        virtual void DebugDraw() {}

        virtual void Save(YAML::Node& node) const {}

        virtual void Load(const YAML::Node& node) {}

        virtual bool CanBeRemoved() const;
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

    private:
        UID uid = 0;
    };
}