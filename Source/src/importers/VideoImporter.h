#pragma once

#include "importers/Importer.h"

namespace Hachiko
{
    class VideoImporter final : public Importer
    {
        friend class ModuleResources;

    public:
        VideoImporter() = default;
        ~VideoImporter() override = default;

        void Import(const char* path, YAML::Node& meta) override;
        Resource* Load(UID id) override;

        void Save(UID id, const Resource* resource) override;
    };
} // namespace Hachiko
