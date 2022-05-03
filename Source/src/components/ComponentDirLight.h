#pragma once

#include "components/Component.h"

namespace Hachiko
{
    class ComponentDirLight : public Component
    {
    public:
        // TODO: add more light types
        ComponentDirLight(GameObject* conatiner);
        ~ComponentDirLight() override;

        void DebugDraw() override;

        [[nodiscard]] bool IsActive() const
        {
            return active;
        }

        void SetActive(bool is_active)
        {
            active = is_active;
        }

        [[nodiscard]] float3 GetDirection() const;

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

        void DrawGui() override;

        float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
        float intensity = 1.0f;

    private:
        bool active = true;
        bool draw_direction = false;

    public:
        CLONE_COMPONENT(ComponentDirLight)

        ComponentDirLight(const ComponentDirLight& other) = default;

        ComponentDirLight& operator=(const ComponentDirLight& other)
        {
            if (this == &other)
            {
                return *this;
            }
            Component::operator =(other);
            color = other.color;
            intensity = other.intensity;
            active = other.active;
            draw_direction = other.draw_direction;
            return *this;
        }
    };
}
