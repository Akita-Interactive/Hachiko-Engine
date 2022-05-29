#pragma once

#include <MathGeolib.h>

namespace Hachiko
{
    struct MouseEventPayload
    {
        enum Action
        {
            CLICK,
            RELEASE,
        };

        MouseEventPayload(Action action, const float2& coords) :
            action(action),
            coords(coords)
        {
        }

        [[nodiscard]] Action GetAction() const
        {
            return action;
        }

        [[nodiscard]] const float2& GetCoords() const
        {
            return coords;
        }

    private:
        Action action;
        float2 coords;
    };
}
