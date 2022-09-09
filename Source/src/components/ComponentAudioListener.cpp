#include "core/hepch.h"
#include "ComponentAudioListener.h"
#include "components/ComponentTransform.h"

/**     CONSTRUCTORS    **/

Hachiko::ComponentAudioListener::ComponentAudioListener(GameObject* container) 
    : Component(Type::AUDIO_LISTENER, container)
{
    MY_DEFAULT_LISTENER = GetID();
    AK::SoundEngine::RegisterGameObj(MY_DEFAULT_LISTENER, game_object->GetName().c_str());
    AK::SoundEngine::SetDefaultListeners(&MY_DEFAULT_LISTENER, 1);
    OnTransformUpdated();
}

Hachiko::ComponentAudioListener::~ComponentAudioListener()
{
    AK::SoundEngine::UnregisterGameObj(MY_DEFAULT_LISTENER);
}


void Hachiko::ComponentAudioListener::OnTransformUpdated()
{
    ComponentTransform* transform = game_object->GetTransform();

    const float3& pos = transform->GetGlobalPosition();
    const float3& front = transform->GetFront();
    const float3& up = transform->GetUp();

    const float3 reverse = (front * -1).Normalized();

    listener_transform.Set(pos.x, pos.y, pos.z, 
                           reverse.x, reverse.y, reverse.z,
                           up.x, up.y, up.z
    );

    AK::SoundEngine::SetPosition(MY_DEFAULT_LISTENER, listener_transform);

}

/**     GUI     **/

void Hachiko::ComponentAudioListener::DrawGui() 
{
    ImGui::PushID(this);

    if (ImGuiUtils::CollapsingHeader(this, "Audio listener"))
    {
        ImGui::TextWrapped("Now you can listen your game ^-^");
    }
    ImGui::PopID();
}
