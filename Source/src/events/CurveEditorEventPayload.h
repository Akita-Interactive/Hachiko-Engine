#pragma once
#include "core/particles/ParticleSystem.h"

namespace Hachiko
{
    struct CurveEditorEventPayload
    {
        CurveEditorEventPayload(ParticleSystem::VariableTypeProperty* value, std::string label):
            title(std::move(label)),
            value(value)
        {
        }

        [[nodiscard]] ParticleSystem::VariableTypeProperty* GetValue() const
        {
            return value;
        }

        [[nodiscard]] const std::string& GetTitle() const
        {
            return title;
        }

    private:
        const std::string title;
        ParticleSystem::VariableTypeProperty* value;
    };
}
