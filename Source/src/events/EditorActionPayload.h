#pragma once

namespace Hachiko
{
    struct EditorActionPayload
    {
        enum class Action
        {
            PLAY,
            STOP,
            RESUME
        };

        EditorActionPayload(Action action) :
            action(action)
        {
        }

        [[nodiscard]] Action GetAction() const
        {
            return action;
        }

    private:
        Action action;
    };
} // namespace Hachiko
