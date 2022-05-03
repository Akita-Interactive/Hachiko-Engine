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
#define CLONE_COMPONENT(type)           \
    Component* Clone() override   \
    {                                   \
        return new type(*this);         \
    }

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

        virtual void DrawGui()
        {
        }

        virtual void Draw(ComponentCamera* camera, Program* program)
        {
        }

        virtual void DebugDraw()
        {
        }

        void Save(YAML::Node& node) const override
        {
        }

        void Load(const YAML::Node& node) override
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

    public:
        virtual Component* Clone() = 0;

        Component(const Component& other) :
            active(other.active),
            type(other.type),
            uid(other.uid)
        {
        }

        Component& operator=(const Component& other)
        {
            if (this == &other)
            {
                return *this;
            }
            // game_object = other.game_object;
            active = other.active;
            type = other.type;
            uid = other.uid;
            return *this;
        }
    };
}
