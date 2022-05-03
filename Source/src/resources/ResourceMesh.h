#pragma once
#include "Resource.h"

namespace Hachiko
{
    class ResourceMesh : public Resource
    {
    public:
        struct Bone
        {
            char name[PATH_MAX];
            float4x4 bind = float4x4::identity;
        };

        enum class Buffers
        {
            INDICES = 0,
            VERTICES,
            NORMALS,
            TEX_COORDS,
            TANGENTS,
            BONES_INDICES,
            BONES_WEIGHTS,
            BONES,
            COUNT,
        };

        ResourceMesh(UID id);
        ~ResourceMesh() override;

        void GenerateBuffers();
        void GenerateAABB();

        void GenerateBoneData(const aiMesh* mesh, float scale);

        void CleanUp();

        [[nodiscard]] unsigned int GetNumBones() const
        {
            return num_bones;
        }

        bool loaded = false;

        AABB bounding_box;

        // Store the ids of buffers in opengl
        unsigned vbo{}; // TODO: remove this
        unsigned ebo{};
        unsigned vao{};
        unsigned buffer_ids[static_cast<int>(Buffers::COUNT)]{};

        // Track buffer sizes
        unsigned buffer_sizes[static_cast<int>(Buffers::COUNT)]{};

        // In memory buffers
        unsigned* indices{};
        float* vertices{};
        float* normals{};
        float* tex_coords{};
        float* tangents{};

        // BONES
        std::unique_ptr<unsigned[]> src_bone_indices = nullptr;
        std::unique_ptr<float4[]> src_bone_weights = nullptr;

        unsigned int num_bones = 0;
        std::unique_ptr<Bone[]> bones = nullptr;

    public:
        CLONE_RESOURCE(ResourceMesh)

        ResourceMesh(const ResourceMesh& other) :
            Resource(other),
            loaded(other.loaded),
            bounding_box(other.bounding_box),
            vbo(other.vbo),
            ebo(other.ebo),
            vao(other.vao),
            num_bones(other.num_bones)
        {
            // *indices = *other.indices;
            // *vertices = *other.vertices;
            // *normals = *other.normals;
            // *tex_coords = *other.tex_coords;
            // *tangents = *other.tangents;

            bones = std::make_unique<Bone[]>(other.num_bones);
            for (int i = 0; i < other.num_bones; ++i)
            {
                bones[i] = other.bones[i];
            }
            //src_bone_indices
            // src_bone_weights
            //TODO include missing fields
        }

        ResourceMesh& operator=(const ResourceMesh& other)
        {
            if (this == &other)
            {
                return *this;
            }
            Resource::operator =(other);
            loaded = other.loaded;
            bounding_box = other.bounding_box;
            vbo = other.vbo;
            ebo = other.ebo;
            vao = other.vao;
            indices = other.indices;
            vertices = other.vertices;
            normals = other.normals;
            tex_coords = other.tex_coords;
            tangents = other.tangents;
            num_bones = other.num_bones;
            return *this;
            //TODO ensure this works
        }
    };
}
