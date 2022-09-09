#pragma once

namespace Hachiko
{
    struct MouseEventPayload
    {
        enum class Action
        {
            CLICK,
            RELEASE,
        };

        explicit MouseEventPayload(Action action) : action(action) {}

        [[nodiscard]] Action GetAction() const
        {
            return action;
        }

    private:
        Action action;
    };
}
