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
    };
}
