#pragma once

class aiMesh;

namespace Hachiko
{
    class ResourceMesh;

    class MeshImporter : public Importer
    {
        friend class ModelImporter;
    public:
        MeshImporter() = default;
        ~MeshImporter() override = default;

        void Import(const char* path, YAML::Node& meta) override
        { 
            assert(false && "This should not be called since it is not an asset");
        };
        void Save(UID id, const Resource* mesh) override;
        Resource* Load(UID id) override;
    private:
        void ImportFromAssimp(UID uid, const aiMesh* ai_mesh);
    };
}
