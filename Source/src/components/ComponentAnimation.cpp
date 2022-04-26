#include "core/hepch.h"

#include "ComponentAnimation.h"
#include "ComponentTransform.h"

#include "animation/AnimationController.h"
#include "importers/AnimationImporter.h"

Hachiko::ComponentAnimation::ComponentAnimation(GameObject* container) : Component(Type::ANIMATION, container)
{
    controller = new AnimationController();
}

Hachiko::ComponentAnimation::~ComponentAnimation()
{
    delete controller;
}

void Hachiko::ComponentAnimation::Start()
{
    controller->Play();
}

void Hachiko::ComponentAnimation::Stop()
{
    controller->Stop();
}

void Hachiko::ComponentAnimation::Update()
{
    controller->Update();

    if (game_object != nullptr)
    {
        UpdatedGameObject(game_object);
    }
}

void Hachiko::ComponentAnimation::UpdatedGameObject(GameObject* go)
{
    float3 position;
    Quat rotation;

    if (controller->GetTransform(go->name.c_str(), position, rotation))
    {
        go->GetTransform()->SetLocalPosition(position);
        go->GetTransform()->SetLocalRotation(rotation);
    }

    for (GameObject* child : go->children)
    {
        UpdatedGameObject(child);
    }
}

void Hachiko::ComponentAnimation::Import(const aiScene* scene)
{
    assert(scene->mNumAnimations == 1);
    // resource = Hachiko::AnimationImporter::Import(scene->mAnimations[0]);
}

void Hachiko::ComponentAnimation::DrawGui()
{
    ImGui::PushID(this);
    if (ImGuiUtils::CollapsingHeader(game_object, this, "Animation"))
    {
        if (ImGui::Checkbox("Active Debug", &active))
        {
            /* TODO */
        }
    }
    ImGui::PopID();
}

void Hachiko::ComponentAnimation::Save(YAML::Node& node) const {}

void Hachiko::ComponentAnimation::Load(const YAML::Node& node) {}
