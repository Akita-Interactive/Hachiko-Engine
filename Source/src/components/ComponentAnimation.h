#pragma once

#include "Component.h"
#include "resources/ResourceAnimation.h"

namespace Hachiko
{
    class GameObject;
    class AnimationController;

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

        void Import(const aiScene* scene);

        void DrawGui() override;

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

    private:
        AnimationController* controller = nullptr;
        ResourceAnimation* resource = nullptr;
    };
} // namespace Hachiko
