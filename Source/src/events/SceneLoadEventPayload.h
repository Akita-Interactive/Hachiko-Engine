#pragma once

namespace Hachiko
{
    struct SceneLoadEventPayload
    {
        enum State
        {
            LOADED,
            NOT_LOADED,
        };

        SceneLoadEventPayload(State state) : state(state) {}

        [[nodiscard]] State GetState() const
        {
            return state;
        }

    private:
        State state;
    };
} // namespace Hachiko