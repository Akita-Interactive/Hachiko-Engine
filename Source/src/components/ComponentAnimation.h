#pragma once

#include "Component.h"

#include "animation/AnimationController.h"

namespace Hachiko
{
    class GameObject;
    class ResourceAnimation;
    class ResourceStateMachine;

    class WindowStateMachine;
  
    class ComponentAnimation : public Component
    {
    public:
        ComponentAnimation(GameObject* container);
        ~ComponentAnimation() override;

        HACHIKO_API void StartAnimating();
        HACHIKO_API void StopAnimating();
        HACHIKO_API void SendTrigger(const std::string& trigger);
        HACHIKO_API void ResetState();

        void Update() override;

        void DrawGui() override;

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

        bool IsAnimationStopped()
        {
            return controller->GetCurrentState() == AnimationController::State::STOPPED;
        }


        // TEST
        bool reverse = false;
        //

    private:
        void LoadStateMachine();
        void AnimationSelector(unsigned clip_idx);

        void UpdatedGameObject(GameObject* go);
        void PlayNode(const std::string& node, unsigned int blend);
        void PlayNode(unsigned int node_idx, unsigned int blend);
        std::string GetActiveNode() const;

    private:
        AnimationController* controller = nullptr;
        unsigned int active_node = 0;

        // SM CONTROL
        WindowStateMachine* windowStateMachine = nullptr;
        ResourceStateMachine* state_machine = nullptr;
        std::string auxiliary_name;
        unsigned editing_clip_idx = 0;
        YAML::Node selected_model_meta;
        std::vector<std::string> selected_asset_clip_names;
    };
} // namespace Hachiko
