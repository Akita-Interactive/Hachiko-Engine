#include "core/hepch.h"
#include "AnimationController.h"

#include "resources/ResourceAnimation.h"
#include "modules/ModuleResources.h"

Hachiko::AnimationController::AnimationController() {}

Hachiko::AnimationController::~AnimationController() {
    if (current != nullptr)
    {
        Stop();
    }
}

void Hachiko::AnimationController::Play(ResourceAnimation* current_animation, bool loop, unsigned fade_time) 
{
    Instance* new_instance = new Instance();

    SetCurrentState(AnimationController::State::PLAYING);

    new_instance->current_animation = current_animation;
    new_instance->loop = loop;
    new_instance->fade_duration = fade_time;
    new_instance->previous = current;

    current = new_instance;
}

void Hachiko::AnimationController::Update(unsigned elapsed, bool reverse)
{
    if (current != nullptr)
    {
        UpdateInstance(current, elapsed, reverse);
    }
}

void Hachiko::AnimationController::Stop() 
{
    if (current != nullptr)
    {
        SetCurrentState(AnimationController::State::PAUSED);
        ReleaseInstance(current);
        current = nullptr;
    }
}

void Hachiko::AnimationController::UpdateInstance(Instance* instance, unsigned elapsed, bool reverse)
{
    if (reverse)
    {
        if (current->current_animation != nullptr && current->current_animation->GetDuration() > 0)
        {
            unsigned me_elapsed = unsigned(elapsed * instance->speed);
            unsigned to_end = instance->time;

            if (me_elapsed <= to_end)
            {
                instance->time -= me_elapsed;
            }
            else if (instance->loop)
            {
                instance->time = current->current_animation->GetDuration() + (to_end - me_elapsed);
            }
            else
            {
                instance->time = 0;
            }

            if (instance->time == 0)
            {
                SetCurrentState(AnimationController::State::STOPPED);
            }
        }

        /*
        if (instance->previous != nullptr)
        {
            unsigned to_end = instance->fade_duration - instance->fade_time;
            if (elapsed <= to_end)
            {
                instance->fade_time += elapsed;
                UpdateInstance(instance->previous, elapsed, reverse);
            }
            else
            {
                ReleaseInstance(instance->previous);
                instance->previous = nullptr;
                instance->fade_time = instance->fade_duration = 0;
            }
        }
        */
    }
    else
    {
        if (current->current_animation != nullptr && current->current_animation->GetDuration() > 0)
        {
            unsigned me_elapsed = unsigned(elapsed * instance->speed);
            //me_elapsed = me_elapsed % current->current_animation->GetDuration(); // REMOVE
            unsigned to_end = current->current_animation->GetDuration() - instance->time;

            if (me_elapsed <= to_end)
            {
                instance->time += me_elapsed;
            }
            else if (instance->loop)
            {
                instance->time = (me_elapsed - to_end);
            }
            else
            {
                instance->time = current->current_animation->GetDuration();
            }

            //instance->time = instance->time > current->current_animation->GetDuration() ? current->current_animation->GetDuration() : instance->time;

            if (to_end == 0)
            {
                SetCurrentState(AnimationController::State::STOPPED);
            }
        }

        if (instance->previous != nullptr)
        {
            unsigned to_end = instance->fade_duration - instance->fade_time;
            if (elapsed <= to_end)
            {
                instance->fade_time += elapsed;
                UpdateInstance(instance->previous, elapsed, reverse);
            }
            else
            {
                ReleaseInstance(instance->previous);
                instance->previous = nullptr;
                instance->fade_time = instance->fade_duration = 0;
            }
        }
    }
}

void Hachiko::AnimationController::ReleaseInstance(Instance* instance)
{
    do
    {
        Instance* next = instance->previous;
        delete instance;
        instance = next;
    } 
    while (instance != nullptr);
}

bool Hachiko::AnimationController::GetTransform(Instance* instance, const std::string& channel_name, math::float3& position, Quat& rotation) const
{
    if (instance != nullptr && instance->current_animation != nullptr)
    {
        const ResourceAnimation::Channel* channel = instance->current_animation->GetChannel(channel_name);

        if (channel != nullptr)
        {

            instance->time = instance->time > instance->current_animation->GetDuration() ? instance->current_animation->GetDuration() : instance->time;

            //assert(instance->time <= instance->current_animation->GetDuration());

            float pos_key = float(instance->time * (channel->num_positions - 1)) / float(instance->current_animation->GetDuration());
            float rot_key = float(instance->time * (channel->num_rotations - 1)) / float(instance->current_animation->GetDuration());

            unsigned pos_index = unsigned(pos_key);
            unsigned rot_index = unsigned(rot_key);

            float pos_lambda = pos_key - float(pos_index);
            float rot_lambda = rot_key - float(rot_index);

            if (pos_lambda > 0.0f)
            {
                position = Interpolate(channel->positions[pos_index], channel->positions[pos_index + 1], pos_lambda);
            }
            else
            {
                position = channel->positions[pos_index];
            }

            if (rot_lambda > 0.0f)
            {
                rotation = Interpolate(channel->rotations[rot_index], channel->rotations[rot_index + 1], rot_lambda);
            }
            else
            {
                rotation = channel->rotations[rot_index];
            }

            if (instance->previous != nullptr)
            {
                assert(instance->fade_duration >= 0.0f);

                float3 next_position;
                Quat next_rotation;

                if (GetTransform(instance->previous, channel_name, next_position, next_rotation))
                {
                    float blend_lambda = float(instance->fade_time) / float(instance->fade_duration);

                    position = Interpolate(next_position, position, blend_lambda);
                    rotation = Interpolate(next_rotation, rotation, blend_lambda);
                }
            }

            return true;
        }
    }

    return false;
}

float Hachiko::AnimationController::Interpolate(float first, float second, float lambda)
{
    return first * (1.0f - lambda) + second * lambda;
}

float3 Hachiko::AnimationController::Interpolate(float3& first, float3& second, float lambda) const
{
    return float3::Lerp(first, second, lambda);
}

Quat Hachiko::AnimationController::Interpolate(Quat& first, Quat& second, float lambda) const
{
    if (first.Dot(second) >= 0.0f)
    {
        return Quat::Lerp(first, second, lambda).Normalized();
    }
    return Quat::Lerp(first, second.Neg(), lambda).Normalized();
}