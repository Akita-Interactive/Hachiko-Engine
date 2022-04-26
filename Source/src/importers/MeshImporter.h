#pragma once

class aiMesh;

namespace Hachiko
{
    class ResourceMesh;

    class MeshImporter : public Importer
    {
        friend class ModelImporter;
    public:
        MeshImporter();
        void Import(const char* path) override;
        void Save(const Resource* mesh) override;
        Resource* Load(const char* path) override;
        bool IsImported(const char* path) override
        {
            return false;
        }

    private:
        void Import(const aiMesh* ai_mesh, const UID& id = 0);
    };
}
