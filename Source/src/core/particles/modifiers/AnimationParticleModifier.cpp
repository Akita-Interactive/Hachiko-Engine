#include "core/hepch.h"
#include "AnimationParticleModifier.h"
#include "resources/ResourceTexture.h"
#include "modules/ModuleResources.h"

Hachiko::AnimationParticleModifier::AnimationParticleModifier(const std::string& name) 
    : ParticleModifier(name, false) 
{
    cfg.min = 0.0f;
    cfg.max = 1.0f;
    cfg.format = "%.2f";
}

void Hachiko::AnimationParticleModifier::Update(std::vector<Particle>& particles)
{
    if (!IsActive())
    {
        return;
    }

    time += EngineTimer::delta_time;
    if (!fit_to_lifetime && time <= animation_speed)
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

void Hachiko::AnimationParticleModifier::DrawGui()
{
    Widgets::Checkbox("Fit to lifetime", &fit_to_lifetime);
    cfg.enabled = !fit_to_lifetime;
    cfg.speed = 0.01f;
    Widgets::DragFloat("Animation speed", animation_speed, &cfg);
    cfg.enabled = true;
    cfg.speed = 0.05f;
    DragFloat("Blend factor", blend_factor, &cfg);
}

void Hachiko::AnimationParticleModifier::Save(YAML::Node& node) const 
{
    YAML::Node animation_modifier = node[MODIFIER_ANIMATION];
    ParticleModifier::Save(animation_modifier);
    animation_modifier[ANIMATION_SPEED] = animation_speed;
    animation_modifier[BLEND_FACTOR] = blend_factor;
    animation_modifier[FIT_TO_LIFETIME] = fit_to_lifetime;
}

void Hachiko::AnimationParticleModifier::Load(const YAML::Node& node) 
{
    ParticleModifier::Load(node[MODIFIER_ANIMATION]);
    animation_speed = node[MODIFIER_ANIMATION][ANIMATION_SPEED].IsDefined() ?
        node[MODIFIER_ANIMATION][ANIMATION_SPEED].as<float>() : animation_speed;
    blend_factor = node[MODIFIER_ANIMATION][BLEND_FACTOR].IsDefined() ?
        node[MODIFIER_ANIMATION][BLEND_FACTOR].as<float>() : blend_factor;
    fit_to_lifetime = node[MODIFIER_ANIMATION][FIT_TO_LIFETIME].IsDefined() ?
        node[MODIFIER_ANIMATION][FIT_TO_LIFETIME].as<bool>() : fit_to_lifetime;
}

void Hachiko::AnimationParticleModifier::UpdateAnimation(Particle& particle) 
{
    if (!particle.HasTexture())
    {
        return;
    }

    particle.SetAnimationBlend(blend_factor);
    unsigned animation_frame = particle.GetCurrentAnimationFrame();
    
    if (fit_to_lifetime)
    {
        unsigned fit_to_frame = static_cast<unsigned>(std::trunc((particle.GetCurrentLifeNormalized() * particle.GetTextureTotalTiles())));

        if (animation_frame == fit_to_frame)
        {
            return;
        }
    }

    float2 animation_idx = particle.GetAnimationIndex();
    
    if (animation_frame >= particle.GetTextureTotalTiles() - 1)
    {
        particle.SetCurrentAnimationFrame(0);
        particle.SetAnimationIndex(float2::zero);
        return;
    }

    if (animation_idx.x < particle.GetTextureTiles().x - 1)
    {
        animation_idx.x += 1.0f;
        particle.SetCurrentAnimationFrame(++animation_frame);
        particle.SetAnimationIndex(animation_idx);
        return;
    }

    if (particle.GetAnimationIndex().y < particle.GetTextureTiles().y - 1)
    {
        animation_idx.x = 0.0f;
        animation_idx.y += 1.0f;
        particle.SetCurrentAnimationFrame(++animation_frame);
        particle.SetAnimationIndex(animation_idx);
        return;
    }

    particle.SetCurrentAnimationFrame(0);
    particle.SetAnimationIndex(float2::zero);
}