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
        
        void Import(const char* path) override;
        void Save(const Resource* resource) override;
        Resource* Load(const char* model_path) override;
        [[nodiscard]] bool IsImported(const char* path) override;
        Resource* CherryImport(int mesh_index, const UID uid, const char* model_path);
    private:
        void ImportModel(const aiScene* scene, YAML::Node& ticket);
        void ImportNode(const aiNode* assimp_node, YAML::Node& node);
        void LoadChildren(YAML::Node& node, YAML::Node& meshes, YAML::Node& materials, std::vector<ResourceNode*>& children);
    };
}