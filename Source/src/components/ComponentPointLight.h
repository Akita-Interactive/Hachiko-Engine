#pragma once

#include "components/Component.h"
#include "core/HachikoApiDefine.h"

namespace Hachiko
{
    class HACHIKO_API ComponentPointLight : public Component
    {
    public:
        // TODO: add more light types
        ComponentPointLight(GameObject* conatiner);
        ~ComponentPointLight() override;

        void DebugDraw() override;

        [[nodiscard]] bool IsActive() const
        {
            return active;
        }

        void SetActive(bool is_active)
        {
            active = is_active;
        }

        float3 GetPosition() const;

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

        void DrawGui() override;

        float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
        float intensity = 1.0f;
        float radius = 250.0f;

    private:
        bool active = true;
        bool draw_sphere = false;
    };
}
