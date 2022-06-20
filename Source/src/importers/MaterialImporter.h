#pragma once

#include "Importer.h"

class aiMaterial;

namespace Hachiko
{
    class ResourceMaterial;
    class ResourceTexture;

    class MaterialImporter : public Importer
    {
        friend class ModelImporter;

    public:
        MaterialImporter() = default;
        ~MaterialImporter() override = default;

        // Copies the material file to lib
        void Import(const char* path, YAML::Node& meta) override;
        // Stores current material resource state to assets and lib
        void Save(UID id, const Resource* material) override;
        Resource* Load(UID id) override;
        // Create new material
        UID CreateAsset(const std::string& name);        
        UID CreateMaterialAssetFromAssimp(const std::string& model_path, aiMaterial* ai_material);
        void GenerateMaterialAssetFile(const ResourceMaterial* material);
    private:
        
        
    };
}