#include "core/hepch.h"
#include "AnimationParticleModule.h"
#include "resources/ResourceTexture.h"
#include "modules/ModuleResources.h"

Hachiko::AnimationParticleModule::AnimationParticleModule(const std::string& name) 
    : ParticleModule(name, false) 
{
    cfg.min = 0.0f;
    cfg.max = 10.0f;
    cfg.speed = 0.01f;
    cfg.format = "%.2f";
}

void Hachiko::AnimationParticleModule::Update(std::vector<Particle>& particles)
{
    if (!IsActive())
    {
        return;
    }

    time += EngineTimer::delta_time;
    if (time <= skip_frames)
    {
        return;
    }

    time = 0.0f;
    for (auto& particle : particles)
    {
        if (!particle.IsActive())
        {
            continue;
        }

        UpdateAnimation(particle);
    }
}

void Hachiko::AnimationParticleModule::DrawGui()
{
    ImGui::Checkbox("Fit to lifetime", &fit_to_lifetime);
    cfg.enabled = !fit_to_lifetime;
    DragFloat("Skip frames", skip_frames, &cfg);
}

void Hachiko::AnimationParticleModule::Save(YAML::Node& node) const 
{
    YAML::Node velocity_module = node[MODULE_ANIMATION];
    ParticleModule::Save(velocity_module);
    velocity_module[SKIP_FRAMES] = skip_frames;
}

void Hachiko::AnimationParticleModule::Load(const YAML::Node& node) 
{
    ParticleModule::Load(node[MODULE_ANIMATION]);
    skip_frames = node[MODULE_ANIMATION][SKIP_FRAMES].IsDefined() ? node[MODULE_ANIMATION][SKIP_FRAMES].as<float>() : skip_frames;
}

void Hachiko::AnimationParticleModule::UpdateAnimation(Particle& particle) 
{
    if (!particle.HasTexture())
    {
        return;
    }

    unsigned animation_frame = particle.GetCurrentAnimationFrame();
    float2 animation_idx = particle.GetAnimationIndex();

    if (animation_frame >= particle.GetTextureTotalTiles())
    {
        particle.SetCurrentAnimationFrame(1);
        particle.SetAnimationIndex(float2::zero);
        return;
    }
    else if (animation_idx.x < particle.GetTextureTiles().x - 1)
    {
        animation_idx.x += 1.0f;
        particle.SetCurrentAnimationFrame(++animation_frame);
        particle.SetAnimationIndex(animation_idx);
        return;
    }
    else if (particle.GetAnimationIndex().y < particle.GetTextureTiles().y - 1)
    {
        animation_idx.x = 0.0f;
        animation_idx.y += 1.0f;
        particle.SetCurrentAnimationFrame(++animation_frame);
        particle.SetAnimationIndex(animation_idx);
        return;
    }

    particle.SetCurrentAnimationFrame(1);
    particle.SetAnimationIndex(float2::zero);
}