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
        void Save(YAML::Node& node) const override;

        void UpdateSize(bool force = false);

        bool HasDependentComponents(GameObject* game_object) const override;

    private:
        unsigned size_x = 0;
        unsigned size_y = 0;
    };
} // namespace Hachiko

inline Hachiko::Component::Type Hachiko::ComponentCanvas::GetType()
{
    return Type::CANVAS;
}
