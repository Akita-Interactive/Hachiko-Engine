#pragma once

#include "Component.h"
#include "resources/ResourceAnimation.h"

namespace Hachiko
{
    class GameObject;
    class AnimationController;
    class ResourceAnimation;

    class ComponentAnimation : public Component
    {
    public:
        ComponentAnimation(GameObject* container);
        ~ComponentAnimation() override;

        static Type GetType()
        {
            return Type::ANIMATION;
        }

        void Start() override;
        void Stop() override;
        void Update() override;

        void UpdatedGameObject(GameObject* go);

        void DrawGui() override;

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

        std::vector<ResourceAnimation*> animations;

    private:

        AnimationController* controller = nullptr;
        ResourceAnimation* current_animation = nullptr;
    };
} // namespace Hachiko
