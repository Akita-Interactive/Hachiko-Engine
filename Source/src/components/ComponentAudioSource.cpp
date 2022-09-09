#include "core/hepch.h"
#include "ComponentAudioSource.h"
#include "components/ComponentTransform.h"
#include "modules/ModuleAudio.h"

Hachiko::ComponentAudioSource::ComponentAudioSource(GameObject* container)
    : Component(Type::AUDIO_SOURCE, container)
    , source_transform(new AkTransform())
{
    source_id = GetID();
    
    AK::SoundEngine::RegisterGameObj(source_id, game_object->GetName().c_str());
    
    OnTransformUpdated();
}

Hachiko::ComponentAudioSource::~ComponentAudioSource()
{
    AK::SoundEngine::StopAll(source_id);
    AK::SoundEngine::UnregisterGameObj(source_id);
    
    delete source_transform;
}

void Hachiko::ComponentAudioSource::OnTransformUpdated()
{
    ComponentTransform* transform = game_object->GetTransform();

    const float3& pos = transform->GetGlobalPosition();
    const float3& front = transform->GetFront();
    const float3& up = transform->GetUp();

    const float3 reverse = (front * -1).Normalized();

    source_transform->Set(pos.x, pos.y, pos.z,
                          reverse.x, reverse.y, reverse.z,
                          up.x, up.y, up.z);

    AK::SoundEngine::SetPosition(source_id, *source_transform);
}

void Hachiko::ComponentAudioSource::PostEvent(const wchar_t* name_event) const
{
    AK::SoundEngine::PostEvent(name_event, source_id);
}

void Hachiko::ComponentAudioSource::DrawGui()
{
    ImGui::PushID(this);

    if (ImGuiUtils::CollapsingHeader(this, "Audio source"))
    {
        ImGui::TextWrapped("Now you can call events to be played ^-^");
    }

    ImGui::PopID();
}