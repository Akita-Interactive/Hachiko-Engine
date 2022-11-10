#pragma once

#include <scripting/Script.h>

namespace Hachiko
{
    class GameObject;
    class ComponentTransform;
    class ComponentImage;
    namespace Scripting
    {
        // Creates a way to set the time scale mode of the game object
        // (and it's descendants) during the game play.
        class TimeScaleModeManager : public Script
        {
            SERIALIZATION_METHODS(false)


        public:
            TimeScaleModeManager(GameObject* game_object);
            ~TimeScaleModeManager() override = default;

            void OnAwake() override;
            void SetScaled(bool is_scaled, bool is_recursive);
            [[nodiscard]] bool IsScaled() const;

        private:
            SERIALIZE_FIELD(bool, _is_scaled);
            SERIALIZE_FIELD(bool, _is_recursive);
        };
} // namespace Scripting
} // namespace Hachiko
