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

        struct Layout
        {
            bool normals = false;
            bool text_coords = false;
            bool bones = false;

            bool Equal(Layout layout) 
            {
                return normals == layout.normals && text_coords == layout.text_coords && bones == layout.bones;
            }
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

        Layout layout;
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

        // UID of the model
        UID model_id; // TODO: USE IT

        // BONES
        std::unique_ptr<unsigned[]> src_bone_indices = nullptr;
        std::unique_ptr<float4[]> src_bone_weights = nullptr;

        unsigned int num_bones = 0;
        std::unique_ptr<Bone[]> bones = nullptr;
    };

    bool operator==(const ResourceMesh::Layout& l, const ResourceMesh::Layout& r);
}
