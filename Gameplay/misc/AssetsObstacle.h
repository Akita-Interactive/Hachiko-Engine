#pragma once

#include <scripting/Script.h>

namespace Hachiko
{
    class GameObject;
    namespace Scripting
    {
        class LevelManager;
        class AssetsObstacle : public Script
        {
            SERIALIZATION_METHODS(false)

        public:
            AssetsObstacle(GameObject* game_object);
            ~AssetsObstacle() override = default;

            void OnAwake() override;

        private:
            ComponentObstacle* _obstacle = nullptr;
        };
    } // namespace Scripting
} // namespace Hachiko*/
