#pragma once

namespace Hachiko
{
    struct GameStateEventPayload
    {
        enum class State
        {
            STARTED,
            RESUMED,
            PAUSED,
            STOPPED            
        };

        GameStateEventPayload(State state) : state(state) {}

        [[nodiscard]] State GetState() const
        {
            return state;
        }
    private:
        State state;
    };
} // namespace Hachiko