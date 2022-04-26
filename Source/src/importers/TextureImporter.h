#pragma once

#include "importers/Importer.h"

namespace Hachiko
{

    class TextureImporter final : public Importer
    {
        friend class MaterialImporter;
        friend class ModuleResources;

    public:
        TextureImporter();
        ~TextureImporter() override = default;

        void Import(const char* path) override;
        Resource* Load(const char* path) override;
        void Save(const Resource* resource) override;

        [[nodiscard]] bool IsImported(const char* path) override
        {
            return false;
        }

    private:
        Resource* ImportTexture(const char* path, UID id = 0);
    };
}
