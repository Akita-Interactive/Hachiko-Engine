#pragma once

#include "components/Component.h"
#include "Globals.h"

namespace Hachiko
{
    class GameObject;

    class ComponentCanvas : public Component
    {
    public:
        // This component will contain canvas and canvas renderer unity functionality
        // Canvas will be in charge of rendering all children
        ComponentCanvas(GameObject* container);
        ~ComponentCanvas() override;

        static Type GetType();

        void Update() override;

        void DrawGui() override;
        void DebugDraw() override;

        void UpdateSize(bool force = false);

        virtual bool HasDependentComponents(GameObject* game_object) const override;

    private:
        unsigned size_x = 0;
        unsigned size_y = 0;

    public:
        CLONE_COMPONENT(ComponentCanvas)

        ComponentCanvas(const ComponentCanvas& other) = default;

        ComponentCanvas& operator=(const ComponentCanvas& other)
        {
            if (this == &other)
            {
                return *this;
            }
            Component::operator =(other);
            size_x = other.size_x;
            size_y = other.size_y;
            return *this;
        }
    };
} // namespace Hachiko

inline Hachiko::Component::Type Hachiko::ComponentCanvas::GetType()
{
    return Type::CANVAS;
}
