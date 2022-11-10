#include "core/hepch.h"

#include "MeshImporter.h"
#include "resources/ResourceMesh.h"

void Hachiko::MeshImporter::Save(UID id, const Resource* res)
{
    const ResourceMesh* mesh = static_cast<const ResourceMesh*>(res);
    const std::string file_path = GetResourcePath(Resource::Type::MESH, id);

    const unsigned* sizes = mesh->buffer_sizes;
    unsigned header[static_cast<int>(ResourceMesh::Buffers::COUNT)] = {
        sizes[static_cast<int>(ResourceMesh::Buffers::INDICES)], 
        sizes[static_cast<int>(ResourceMesh::Buffers::VERTICES)],
        sizes[static_cast<int>(ResourceMesh::Buffers::NORMALS)], 
        sizes[static_cast<int>(ResourceMesh::Buffers::TEX_COORDS)],
        sizes[static_cast<int>(ResourceMesh::Buffers::TANGENTS)],
        sizes[static_cast<int>(ResourceMesh::Buffers::BONES_INDICES)],
        sizes[static_cast<int>(ResourceMesh::Buffers::BONES_WEIGHTS)],
        sizes[static_cast<int>(ResourceMesh::Buffers::BONES)]
    };

    unsigned file_size = 0;
    file_size += sizeof(header);
    file_size += sizeof(unsigned) * sizes[static_cast<int>(ResourceMesh::Buffers::INDICES)];
    file_size += sizeof(float) * sizes[static_cast<int>(ResourceMesh::Buffers::VERTICES)];
    file_size += sizeof(float) * sizes[static_cast<int>(ResourceMesh::Buffers::NORMALS)];
    file_size += sizeof(float) * sizes[static_cast<int>(ResourceMesh::Buffers::TEX_COORDS)];
    file_size += sizeof(float) * sizes[static_cast<int>(ResourceMesh::Buffers::TANGENTS)];
    file_size += sizeof(unsigned) * sizes[static_cast<int>(ResourceMesh::Buffers::BONES_INDICES)];
    file_size += sizeof(float4) * sizes[static_cast<int>(ResourceMesh::Buffers::BONES_WEIGHTS)];
    file_size += sizeof(Hachiko::ResourceMesh::Bone) * sizes[static_cast<int>(ResourceMesh::Buffers::BONES)];

    const auto file_buffer = new char[file_size];
    char* cursor = file_buffer;
    unsigned size_bytes = 0;

    size_bytes = sizeof(header);
    memcpy(cursor, header, size_bytes);
    cursor += size_bytes;

    size_bytes = sizeof(unsigned) * sizes[static_cast<int>(ResourceMesh::Buffers::INDICES)];
    memcpy(cursor, mesh->indices, size_bytes);
    cursor += size_bytes;

    size_bytes = sizeof(float) * sizes[static_cast<int>(ResourceMesh::Buffers::VERTICES)];
    memcpy(cursor, mesh->vertices, size_bytes);
    cursor += size_bytes;

    size_bytes = sizeof(float) * sizes[static_cast<int>(ResourceMesh::Buffers::NORMALS)];
    memcpy(cursor, mesh->normals, size_bytes);
    cursor += size_bytes;

    size_bytes = sizeof(float) * sizes[static_cast<int>(ResourceMesh::Buffers::TEX_COORDS)];
    memcpy(cursor, mesh->tex_coords, size_bytes);
    cursor += size_bytes;

    size_bytes = sizeof(float) * sizes[static_cast<int>(ResourceMesh::Buffers::TANGENTS)];
    memcpy(cursor, mesh->tangents, size_bytes);
    cursor += size_bytes;

    size_bytes = sizeof(unsigned) * sizes[static_cast<int>(ResourceMesh::Buffers::BONES_INDICES)];
    memcpy(cursor, mesh->src_bone_indices.get(), size_bytes);
    cursor += size_bytes;

    size_bytes = sizeof(float4) * sizes[static_cast<int>(ResourceMesh::Buffers::BONES_WEIGHTS)];
    memcpy(cursor, mesh->src_bone_weights.get(), size_bytes);
    cursor += size_bytes;

    size_bytes = sizeof(Hachiko::ResourceMesh::Bone) * sizes[static_cast<int>(ResourceMesh::Buffers::BONES)];
    memcpy(cursor, mesh->bones.get(), size_bytes);
    cursor += size_bytes;

    FileSystem::Save(file_path.c_str(), file_buffer, file_size);
    delete[] file_buffer;
}

Hachiko::Resource* Hachiko::MeshImporter::Load(UID id)
{
    assert(id && "Unable to load mesh. Given an empty id");

    const std::string file_path = GetResourcePath(Resource::Type::MESH, id);
    
    char* file_buffer = FileSystem::Load(file_path.c_str());
    char* cursor = file_buffer;
    unsigned size_bytes = 0;

    const auto mesh = new ResourceMesh(id);

    unsigned header[static_cast<int>(ResourceMesh::Buffers::COUNT)];
    size_bytes = sizeof(header);
    memcpy(header, cursor, size_bytes);
    cursor += size_bytes;

    unsigned* sizes = mesh->buffer_sizes;
    sizes[static_cast<int>(ResourceMesh::Buffers::INDICES)] = header[0];
    sizes[static_cast<int>(ResourceMesh::Buffers::VERTICES)] = header[1];
    sizes[static_cast<int>(ResourceMesh::Buffers::NORMALS)] = header[2];
    sizes[static_cast<int>(ResourceMesh::Buffers::TEX_COORDS)] = header[3];
    sizes[static_cast<int>(ResourceMesh::Buffers::TANGENTS)] = header[4];
    sizes[static_cast<int>(ResourceMesh::Buffers::BONES_INDICES)] = header[5];
    sizes[static_cast<int>(ResourceMesh::Buffers::BONES_WEIGHTS)] = header[6];
    sizes[static_cast<int>(ResourceMesh::Buffers::BONES)] = header[7];

    size_bytes = sizeof(unsigned) * sizes[static_cast<int>(ResourceMesh::Buffers::INDICES)];
    mesh->indices = new unsigned[sizes[static_cast<int>(ResourceMesh::Buffers::INDICES)]];
    memcpy(mesh->indices, cursor, size_bytes);
    cursor += size_bytes;

    mesh->vertices = new float[sizes[static_cast<int>(ResourceMesh::Buffers::VERTICES)]];
    size_bytes = sizeof(float) * sizes[static_cast<int>(ResourceMesh::Buffers::VERTICES)];
    memcpy(mesh->vertices, cursor, size_bytes);
    cursor += size_bytes;

    if (sizes[static_cast<int>(ResourceMesh::Buffers::NORMALS)] > 0)
    {
        mesh->normals = new float[sizes[static_cast<int>(ResourceMesh::Buffers::NORMALS)]];
        size_bytes = sizeof(float) * sizes[static_cast<int>(ResourceMesh::Buffers::NORMALS)];
        memcpy(mesh->normals, cursor, size_bytes);
        cursor += size_bytes;

        mesh->layout.normals = true;
    }

    if (sizes[static_cast<int>(ResourceMesh::Buffers::TEX_COORDS)] > 0)
    {
        mesh->tex_coords = new float[sizes[static_cast<int>(ResourceMesh::Buffers::TEX_COORDS)]];
        size_bytes = sizeof(float) * sizes[static_cast<int>(ResourceMesh::Buffers::TEX_COORDS)];
        memcpy(mesh->tex_coords, cursor, size_bytes);
        cursor += size_bytes;

        mesh->layout.text_coords = true;
    }
    
    if (sizes[static_cast<int>(ResourceMesh::Buffers::TANGENTS)] > 0)
    {
        mesh->tangents = new float[sizes[static_cast<int>(ResourceMesh::Buffers::TANGENTS)]];
        size_bytes = sizeof(float) * sizes[static_cast<int>(ResourceMesh::Buffers::TANGENTS)];
        memcpy(mesh->tangents, cursor, size_bytes);
        cursor += size_bytes;
    }

    if (sizes[static_cast<int>(ResourceMesh::Buffers::BONES_INDICES)] > 0)
    {
        mesh->src_bone_indices = std::make_unique<unsigned[]>(sizes[static_cast<int>(ResourceMesh::Buffers::BONES_INDICES)]);
        size_bytes = sizeof(unsigned) * sizes[static_cast<int>(ResourceMesh::Buffers::BONES_INDICES)];
        memcpy(mesh->src_bone_indices.get(), cursor, size_bytes);
        cursor += size_bytes;
    }

    if (sizes[static_cast<int>(ResourceMesh::Buffers::BONES_WEIGHTS)] > 0)
    {
        mesh->src_bone_weights = std::make_unique<float4[]>(sizes[static_cast<int>(ResourceMesh::Buffers::BONES_WEIGHTS)]);
        size_bytes = sizeof(float4) * sizes[static_cast<int>(ResourceMesh::Buffers::BONES_WEIGHTS)];
        memcpy(mesh->src_bone_weights.get(), cursor, size_bytes);
        cursor += size_bytes;
    }

    if (sizes[static_cast<int>(ResourceMesh::Buffers::BONES)] > 0)
    {
        mesh->bones = std::make_unique<Hachiko::ResourceMesh::Bone[]>(sizes[static_cast<int>(ResourceMesh::Buffers::BONES)]);
        size_bytes = sizeof(Hachiko::ResourceMesh::Bone) * sizes[static_cast<int>(ResourceMesh::Buffers::BONES)];
        memcpy(mesh->bones.get(), cursor, size_bytes);
        cursor += size_bytes;

        mesh->layout.bones = true;
        mesh->num_bones = sizes[static_cast<int>(ResourceMesh::Buffers::BONES)];
    }

    mesh->GenerateAABB();
    mesh->loaded = true;

    delete[] file_buffer;

    return mesh;
}

void Hachiko::MeshImporter::ImportFromAssimp(UID uid, const aiMesh* ai_mesh)
{
    const auto mesh = new ResourceMesh(uid);
    mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::VERTICES)] = ai_mesh->mNumVertices * 3;
    mesh->vertices = new float[mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::VERTICES)]];
    memcpy(mesh->vertices, ai_mesh->mVertices, mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::VERTICES)] * sizeof(float));

    if (ai_mesh->HasBones())
    {
        mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::BONES)] = ai_mesh->mNumBones;
        mesh->GenerateBoneData(ai_mesh, 1);
        mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::BONES_INDICES)] = 4 * ai_mesh->mNumVertices;
        mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::BONES_WEIGHTS)] = ai_mesh->mNumVertices;

        mesh->layout.bones = true;
    }
    else
    {
        mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::BONES)] = 0;
        mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::BONES_INDICES)] = 0;
        mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::BONES_WEIGHTS)] = 0;
    }

    if (ai_mesh->HasNormals())
    {
        mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::NORMALS)] = ai_mesh->mNumVertices * 3;
        mesh->normals = new float[mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::NORMALS)]];
        memcpy(mesh->normals, ai_mesh->mNormals, mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::NORMALS)] * sizeof(float));

        mesh->layout.normals = true;
    }
    else
    {
        mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::NORMALS)] = 0;
    }

    if (ai_mesh->HasTextureCoords(0))
    {
        mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::TEX_COORDS)] = ai_mesh->mNumVertices * 2;
        mesh->tex_coords = new float[mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::TEX_COORDS)]];
        for (unsigned i = 0; i < ai_mesh->mNumVertices; i++)
        {
            memcpy(&mesh->tex_coords[i * 2], &ai_mesh->mTextureCoords[0][i], 2 * sizeof(unsigned));
        }

        mesh->layout.text_coords = true;
    }
    else
    {
        mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::TEX_COORDS)] = 0;
    }

    if (ai_mesh->HasTangentsAndBitangents())
    {
        mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::TANGENTS)] = ai_mesh->mNumVertices * 3;
        mesh->tangents = new float[mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::TANGENTS)]];
        memcpy(mesh->tangents, ai_mesh->mTangents, mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::TANGENTS)] * sizeof(float));
    }
    else
    {
        mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::TANGENTS)] = 0;
    }

    mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::INDICES)] = ai_mesh->mNumFaces * 3;
    mesh->indices = new unsigned[mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::INDICES)]];
    for (unsigned i = 0; i < ai_mesh->mNumFaces; ++i)
    {
        memcpy(&mesh->indices[i * 3], ai_mesh->mFaces[i].mIndices, 3 * sizeof(unsigned));
    }

    mesh->GenerateAABB();
    mesh->loaded = true;

    Save(uid, mesh);

    delete mesh;
}