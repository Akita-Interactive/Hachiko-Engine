#pragma once

#include <assimp/scene.h>

namespace Hachiko
{
    class ResourceAnimation;

    class AnimationImporter final : public Importer
    {
        friend class ModelImporter;
    public:
        AnimationImporter();
        ~AnimationImporter() override = default;

        void Import(const char* path, YAML::Node& meta) override;
        void ImportWithMeta(const char* path, YAML::Node& meta) override;
        void Save(const Resource* resource) override;
        Resource* Load(UID id) override;

        [[nodiscard]] bool IsImported(const char* path) override
        {
            return false;
        }

    private:
        void Import(const aiAnimation* animation, UID id);
    };
} // namespace Hachiko
