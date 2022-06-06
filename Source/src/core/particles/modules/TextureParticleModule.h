#pragma once

#include "core/particles/ParticleModule.h"

namespace Hachiko
{
    class ResourceTexture;

    class TextureParticleModule : public ParticleModule
    {
    public:
        TextureParticleModule(const std::string& name);
        ~TextureParticleModule() override = default;

        void Update(std::vector<Particle>&) override;
        void DrawGui() override;

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

    private:
        Widgets::DragFloatConfig cfg;
        bool flip_texture[2] = {false, false};
        int has_texture = 0;
        ResourceTexture* texture = nullptr;
        UID textureID = 0;
                
        // Animation
        bool animation_loop = true;
        bool play_animation = false;
        bool has_flip_x = false;
        bool has_flip_y = false;
        int x_tiles = 1;
        int y_tiles = 1;
        float x_factor = 1.0f;
        float y_factor = 1.0f;
        float current_frame = 0.0f;
        float2 animation_index = {0.0f, 0.0f};

        void AddTexture();
        void RemoveTexture();
        void UpdateTextureOverTime(Particle& particle);
    };
}