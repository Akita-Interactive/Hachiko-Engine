#include "core/hepch.h"
#include "ResourceMesh.h"

Hachiko::ResourceMesh::ResourceMesh(UID id) :
    Resource(id, Type::MESH) {}

Hachiko::ResourceMesh::~ResourceMesh()
{
    CleanUp();
}

void Hachiko::ResourceMesh::GenerateBuffers()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Positions (3 values per coord)
    glGenBuffers(1, &buffer_ids[static_cast<int>(Buffers::VERTICES)]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_ids[static_cast<int>(Buffers::VERTICES)]);
    glBufferData(GL_ARRAY_BUFFER, buffer_sizes[static_cast<int>(Buffers::VERTICES)] * sizeof(float), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);

    // Normals (3 values per coord)
    if (buffer_sizes[static_cast<int>(Buffers::NORMALS)] > 0)
    {
        glGenBuffers(1, &buffer_ids[static_cast<int>(Buffers::NORMALS)]);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_ids[static_cast<int>(Buffers::NORMALS)]);
        glBufferData(GL_ARRAY_BUFFER, buffer_sizes[static_cast<int>(Buffers::NORMALS)] * sizeof(float), normals, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
        glEnableVertexAttribArray(1);
    }

    // Texture Coords (2 values per coord)
    if (buffer_sizes[static_cast<int>(Buffers::TEX_COORDS)] > 0)
    {
        glGenBuffers(1, &buffer_ids[static_cast<int>(Buffers::TEX_COORDS)]);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_ids[static_cast<int>(Buffers::TEX_COORDS)]);
        glBufferData(GL_ARRAY_BUFFER, buffer_sizes[static_cast<int>(Buffers::TEX_COORDS)] * sizeof(float), tex_coords, GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(nullptr));
        glEnableVertexAttribArray(2);
    }

    // Tangents (3 values per coord)
    if (buffer_sizes[static_cast<int>(Buffers::TANGENTS)] > 0)
    {
        glGenBuffers(1, &buffer_ids[static_cast<int>(Buffers::TANGENTS)]);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_ids[static_cast<int>(Buffers::TANGENTS)]);
        glBufferData(GL_ARRAY_BUFFER, buffer_sizes[static_cast<int>(Buffers::TANGENTS)] * sizeof(float), tangents, GL_STATIC_DRAW);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
        glEnableVertexAttribArray(3);
    }

    if (buffer_sizes[static_cast<int>(Buffers::BONES)] > 0)
    {
        // SRC_BONES_WEIGHTS
        glGenBuffers(1, &buffer_ids[static_cast<int>(Buffers::BONES_INDICES)]);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_ids[static_cast<int>(Buffers::BONES_INDICES)]);
        glBufferData(GL_ARRAY_BUFFER, buffer_sizes[static_cast<int>(Buffers::BONES_INDICES)] * sizeof(unsigned), src_bone_indices.get(), GL_STATIC_DRAW);
        glVertexAttribIPointer(4, 4, GL_UNSIGNED_INT, sizeof(unsigned) * 4, static_cast<void*>(nullptr));
        glEnableVertexAttribArray(4);

        // SRC_BONES_INEX
        glGenBuffers(1, &buffer_ids[static_cast<int>(Buffers::BONES_WEIGHTS)]);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_ids[static_cast<int>(Buffers::BONES_WEIGHTS)]);
        glBufferData(GL_ARRAY_BUFFER, buffer_sizes[static_cast<int>(Buffers::BONES_WEIGHTS)] * sizeof(float4), src_bone_weights.get(), GL_STATIC_DRAW);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, static_cast<void*>(nullptr));
        glEnableVertexAttribArray(5);
    }

    // Indices (1 value)
    glGenBuffers(1, &buffer_ids[static_cast<int>(Buffers::INDICES)]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_ids[static_cast<int>(Buffers::INDICES)]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer_sizes[static_cast<int>(Buffers::INDICES)] * sizeof(unsigned), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    loaded = true;
}

void Hachiko::ResourceMesh::GenerateAABB()
{
    bounding_box.SetFrom((float3*)vertices, buffer_sizes[static_cast<int>(Buffers::VERTICES)] / 3);
}

void Hachiko::ResourceMesh::GenerateBoneData(const aiMesh* mesh, float scale) {
    assert(mesh->HasBones());

    num_bones = mesh->mNumBones;
    bones = std::make_unique<Bone[]>(num_bones);

    for (unsigned i = 0; i < num_bones; ++i)
    {
        const aiBone* bone = mesh->mBones[i];

        strcpy_s(bones[i].name, bone->mName.C_Str());
        bones[i].bind = float4x4(float4(bone->mOffsetMatrix.a1, bone->mOffsetMatrix.b1, bone->mOffsetMatrix.c1, bone->mOffsetMatrix.d1),
                                 float4(bone->mOffsetMatrix.a2, bone->mOffsetMatrix.b2, bone->mOffsetMatrix.c2, bone->mOffsetMatrix.d2),
                                 float4(bone->mOffsetMatrix.a3, bone->mOffsetMatrix.b3, bone->mOffsetMatrix.c3, bone->mOffsetMatrix.d3),
                                 float4(bone->mOffsetMatrix.a4, bone->mOffsetMatrix.b4, bone->mOffsetMatrix.c4, bone->mOffsetMatrix.d4));

        bones[i].bind.SetTranslatePart(bones[i].bind.TranslatePart() * scale);
    }

    std::unique_ptr<unsigned[]> bone_indices = std::make_unique<unsigned[]>(4 * mesh->mNumVertices);
    std::unique_ptr<float[]> bone_weights = std::make_unique<float[]>(4 * mesh->mNumVertices);

    for (unsigned i = 0; i < mesh->mNumVertices * 4; ++i)
    {
        bone_indices[i] = 0;
        bone_weights[i] = 0.0f;
    }

    for (unsigned i = 0; i < num_bones; ++i)
    {
        const aiBone* bone = mesh->mBones[i];

        for (unsigned j = 0; j < bone->mNumWeights; ++j)
        {
            unsigned index = bone->mWeights[j].mVertexId;
            float weight = bone->mWeights[j].mWeight;

            unsigned* bone_idx = &bone_indices[index * 4];
            float* bone_weight = &bone_weights[index * 4];

            // TODO: Ask Eric if this should stay.
            //assert(bone_weight[3] == 0.0f);

            for (unsigned l = 0; l < 4; ++l)
            {
                if (bone_weight[l] == 0.0f)
                {
                    bone_idx[l] = i;
                    bone_weight[l] = weight;

                    break;
                }
            }
        }
    }

    src_bone_indices = std::move(bone_indices);
    src_bone_weights.reset((float4*)bone_weights.release());

    for (unsigned i = 0; i < mesh->mNumVertices; ++i)
    {
        float length = 0.0f;
        for (unsigned j = 0; j < 4; ++j)
        {
            length += src_bone_weights[i][j];
        }

        if (length > 0.0f)
        {
            src_bone_weights[i] = src_bone_weights[i] / length;
        }
    }
}

void Hachiko::ResourceMesh::CleanUp()
{
    if (loaded)
    {
        glDeleteBuffers(1, &buffer_ids[static_cast<int>(Buffers::INDICES)]);
        glDeleteBuffers(1, &buffer_ids[static_cast<int>(Buffers::VERTICES)]);
        glDeleteBuffers(1, &buffer_ids[static_cast<int>(Buffers::NORMALS)]);
        glDeleteBuffers(1, &buffer_ids[static_cast<int>(Buffers::TEX_COORDS)]);
        glDeleteBuffers(1, &buffer_ids[static_cast<int>(Buffers::TANGENTS)]);
        glDeleteBuffers(1, &buffer_ids[static_cast<int>(Buffers::BONES_INDICES)]);
        glDeleteBuffers(1, &buffer_ids[static_cast<int>(Buffers::BONES_WEIGHTS)]);

        delete[] indices;
        delete[] vertices;
        delete[] normals;
        delete[] tex_coords;
        delete[] tangents;

        buffer_sizes[static_cast<int>(Buffers::INDICES)] = 0;
        buffer_sizes[static_cast<int>(Buffers::VERTICES)] = 0;
        buffer_sizes[static_cast<int>(Buffers::NORMALS)] = 0;
        buffer_sizes[static_cast<int>(Buffers::TEX_COORDS)] = 0;
        buffer_sizes[static_cast<int>(Buffers::TANGENTS)] = 0;
        buffer_sizes[static_cast<int>(Buffers::BONES)] = 0;
        buffer_sizes[static_cast<int>(Buffers::BONES_INDICES)] = 0;
        buffer_sizes[static_cast<int>(Buffers::BONES_WEIGHTS)] = 0;
    }
    loaded = false;
}

bool Hachiko::operator==(const ResourceMesh::Layout& l, const ResourceMesh::Layout& r)
{
    return (l.normals == r.normals && l.text_coords == r.text_coords);
}