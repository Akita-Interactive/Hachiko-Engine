#pragma once

#include "components/Component.h"

namespace Hachiko
{
    class ComponentSpotLight : public Component
    {
    public:
        // TODO: add more light types
        ComponentSpotLight(GameObject* conatiner);
        ~ComponentSpotLight() override;

        void DebugDraw() override;

        [[nodiscard]] bool IsActive() const
        {
            return active;
        }

        void SetActive(bool is_active)
        {
            active = is_active;
        }

        [[nodiscard]] float3 GetPosition() const;
        [[nodiscard]] float3 GetDirection() const;

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

        void DrawGui() override;

        float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
        float inner = 15.f;
        float outer = 30.f;
        float intensity = 1.0f;
        float radius = 250.0f;

    private:
        bool active = true;
        bool draw_cone = false;
    };
}
