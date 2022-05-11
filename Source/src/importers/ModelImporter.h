#pragma once

class aiScene;

namespace Hachiko
{
    class Importer;
    struct ResourceNode;

    class ModelImporter final : public Importer
    {
    public:
        ModelImporter();
        ~ModelImporter() override = default;
        
        void Import(const char* path, YAML::Node& meta) override;
        void ImportWithMeta(const char* path, YAML::Node& meta) override;
        void Save(const Resource* resource) override;
        Resource* Load(UID id) override;
        void Delete(const YAML::Node& meta) override;

        [[nodiscard]] bool IsImported(const char* path) override;
    private:
        void ImportModel(const aiScene* scene, YAML::Node& ticket);
        void ImportNode(const aiNode* assimp_node, YAML::Node& node, bool load_auxiliar);
        void LoadChildren(YAML::Node& node, YAML::Node& meshes, YAML::Node& materials, std::vector<ResourceNode*>& children);
    };
}