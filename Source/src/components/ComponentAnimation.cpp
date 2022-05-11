#include "core/hepch.h"

#include "Application.h"

#include "modules/ModuleEditor.h"
#include "modules/ModuleResources.h"

#include "ComponentAnimation.h"
#include "ComponentTransform.h"

#include "resources/ResourceAnimation.h"
#include "animation/AnimationController.h"
#include "importers/AnimationImporter.h"

Hachiko::ComponentAnimation::ComponentAnimation(GameObject* container) 
    : Component(Type::ANIMATION, container)
{
    controller = new AnimationController();
}

Hachiko::ComponentAnimation::~ComponentAnimation()
{
    delete controller;
    animations.clear();
}

void Hachiko::ComponentAnimation::StartAnimating(unsigned int animation_index, bool on_loop, unsigned int fade_in_time_ms) 
{
    if (animation_index >= animations.size())
    {
        return;
    }

    current_animation = animations[animation_index];

    StartAnimating(on_loop, fade_in_time_ms);
}

void Hachiko::ComponentAnimation::StartAnimating(bool on_loop, unsigned int fade_in_time_ms)
{
    if (!animations.empty())
    {
        controller->Play(current_animation, on_loop, fade_in_time_ms);
    }
}

void Hachiko::ComponentAnimation::StopAnimating()
{
    controller->Stop();
}

void Hachiko::ComponentAnimation::Update()
{
    if (current_animation)
    {
        controller->Update(EngineTimer::delta_time * 1000); // TODO: change for GameTimer::delta_time

        if (game_object != nullptr)
        {
            UpdatedGameObject(game_object);
        }
    }
}

void Hachiko::ComponentAnimation::UpdatedGameObject(GameObject* go)
{
    float3 position;
    Quat rotation;

    if (controller->GetTransform(controller->current, go->name.c_str(), position, rotation))
    {
        go->GetTransform()->SetLocalPosition(position);
        go->GetTransform()->SetLocalRotation(rotation);
    }

    for (GameObject* child : go->children)
    {
        UpdatedGameObject(child);
    }
}

void Hachiko::ComponentAnimation::DrawGui()
{
    ImGui::PushID(this);
    
    if (ImGuiUtils::CollapsingHeader(game_object, this, "Animation"))
    {
        for (unsigned i = 0; i < animations.size(); ++i)
        {
            char animation_name[50];
            strcpy_s(animation_name, 50, animations[i]->GetName().c_str());
            if (ImGui::Button(StringUtils::Concat(ICON_FA_PLAY, " ", animation_name, std::to_string(i)).c_str()))
            {
                current_animation = animations[i];
                this->StartAnimating();
            }
        }

        const std::string title = StringUtils::Concat("Select Animation#", std::to_string(uid));

        if (ImGui::Button("Select animation"))
        {
            ImGuiFileDialog::Instance()->OpenDialog(title.c_str(),
                                                    "Select Animation",
                                                    ".*",
                                                    "./library/animations/",
                                                    1,
                                                    nullptr,
                                                    ImGuiFileDialogFlags_DisableCreateDirectoryButton | ImGuiFileDialogFlags_HideColumnType
                                                        | ImGuiFileDialogFlags_HideColumnDate);
        }

        if (ImGuiFileDialog::Instance()->Display(title.c_str()))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filename = ImGuiFileDialog::Instance()->GetCurrentFileName();
                const std::string uid = filename.substr(0, filename.find(".*"));

                ResourceAnimation* res = static_cast<ResourceAnimation*>(App->resources->GetResource(Resource::Type::ANIMATION, Hachiko::UUID::StringToUID(uid)));
                if (res != nullptr) // TODO: ADD CHECK FOR ADDED
                {
                    HE_LOG(res->GetName().c_str());
                    animations.push_back(res);
                }
            }

            ImGuiFileDialog::Instance()->Close();
        }

        if (current_animation != nullptr && ImGui::Button(StringUtils::Concat(ICON_FA_STOP, " ", current_animation->GetName().c_str()).c_str()))
        {
            StopAnimating();
        }
    }
   
    ImGui::PopID();
}

void Hachiko::ComponentAnimation::Save(YAML::Node& node) const 
{
    for (unsigned i = 0; i < animations.size(); ++i)
    {
        node[ANIMATIONS][i] = animations[i]->GetID();
    }
}

void Hachiko::ComponentAnimation::Load(const YAML::Node& node) 
{
    if (node[ANIMATIONS].IsDefined())
    {
        for (unsigned i = 0; i < node[ANIMATIONS].size(); ++i)
        {
            ResourceAnimation* r_animation = static_cast<ResourceAnimation*>(App->resources->GetResource(Resource::Type::ANIMATION, node[ANIMATIONS][i].as<UID>()));
            animations.push_back(r_animation);
        }
    }
}