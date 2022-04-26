#pragma once

#include <assimp/scene.h>

namespace Hachiko
{
    class ResourceAnimation;

    class AnimationImporter final : public Importer
    {
    public:
        AnimationImporter();
        ~AnimationImporter() override = default;

        void Import(const char* path) override;
        void Save(const Resource* resource) override;
        Resource* Load(const char* model_path) override;
        [[nodiscard]] bool IsImported(const char* path) override;

        ResourceAnimation* Import(const aiAnimation* animation);
    };
} // namespace Hachiko
