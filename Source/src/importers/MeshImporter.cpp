#include "core/hepch.h"

#include "MeshImporter.h"
#include "resources/ResourceMesh.h"
#include "core/preferences/src/ResourcesPreferences.h"

Hachiko::MeshImporter::MeshImporter() : Importer(Importer::Type::MESH)
{
}

void Hachiko::MeshImporter::Import(const char* path)
{
}

void Hachiko::MeshImporter::Save(const Resource* res)
{
    const ResourceMesh* mesh = static_cast<const ResourceMesh*>(res);
    const std::string file_path = GetResourcesPreferences()->GetLibraryPath(Resource::Type::MESH) + std::to_string(mesh->GetID());

    const unsigned* sizes = mesh->buffer_sizes;
    unsigned header[static_cast<int>(ResourceMesh::Buffers::COUNT)] = {
        sizes[static_cast<int>(ResourceMesh::Buffers::INDICES)], 
        sizes[static_cast<int>(ResourceMesh::Buffers::VERTICES)],
        sizes[static_cast<int>(ResourceMesh::Buffers::NORMALS)], 
        sizes[static_cast<int>(ResourceMesh::Buffers::TEX_COORDS)],
        sizes[static_cast<int>(ResourceMesh::Buffers::TANGENTS)]
    };

    unsigned file_size = 0;
    file_size += sizeof(header);
    file_size += sizeof(unsigned) * sizes[static_cast<int>(ResourceMesh::Buffers::INDICES)];
    file_size += sizeof(float) * sizes[static_cast<int>(ResourceMesh::Buffers::VERTICES)];
    file_size += sizeof(float) * sizes[static_cast<int>(ResourceMesh::Buffers::NORMALS)];
    file_size += sizeof(float) * sizes[static_cast<int>(ResourceMesh::Buffers::TEX_COORDS)];
    file_size += sizeof(float) * sizes[static_cast<int>(ResourceMesh::Buffers::TANGENTS)];

    const auto file_buffer = new char[file_size];
    char* cursor = file_buffer;
    unsigned size_bytes = 0;

    size_bytes = sizeof(header);
    memcpy(cursor, sizes, size_bytes);
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

    FileSystem::Save(file_path.c_str(), file_buffer, file_size);
    delete[] file_buffer;
}

Hachiko::Resource* Hachiko::MeshImporter::Load(const char* file_path)
{
    if (!std::filesystem::exists(file_path))
    {
        return nullptr;
    }

    char* file_buffer = FileSystem::Load(file_path);
    char* cursor = file_buffer;
    unsigned size_bytes = 0;

    std::string mesh_id = FileSystem::GetFileNameAndExtension(file_path);

    const auto mesh = new ResourceMesh(UUID::StringToUID(mesh_id));

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
    }
    else
    {
        mesh->normals = nullptr;
    }

    if (sizes[static_cast<int>(ResourceMesh::Buffers::TEX_COORDS)] > 0)
    {
        mesh->tex_coords = new float[sizes[static_cast<int>(ResourceMesh::Buffers::TEX_COORDS)]];
        size_bytes = sizeof(float) * sizes[static_cast<int>(ResourceMesh::Buffers::TEX_COORDS)];
        memcpy(mesh->tex_coords, cursor, size_bytes);
        cursor += size_bytes;
    }
    else
    {
        mesh->tex_coords = nullptr;
    }
    
    if (sizes[static_cast<int>(ResourceMesh::Buffers::TANGENTS)] > 0)
    {
        mesh->tangents = new float[sizes[static_cast<int>(ResourceMesh::Buffers::TANGENTS)]];
        size_bytes = sizeof(float) * sizes[static_cast<int>(ResourceMesh::Buffers::TANGENTS)];
        memcpy(mesh->tangents, cursor, size_bytes);
        cursor += size_bytes;
    }
    else
    {
        mesh->tangents = nullptr;
    }    

    mesh->GenerateBuffers();
    mesh->GenerateAABB();
    mesh->loaded = true;

    delete[] file_buffer;

    return mesh;
}

void Hachiko::MeshImporter::Import(const aiMesh* ai_mesh, const UID& id)
{
    if (!id)
    {
        const_cast<UID&>(id) = UUID::GenerateUID();
    }

    const auto mesh = new ResourceMesh(id);
    mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::VERTICES)] = ai_mesh->mNumVertices * 3;
    mesh->vertices = new float[mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::VERTICES)]];
    memcpy(mesh->vertices, ai_mesh->mVertices, mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::VERTICES)] * sizeof(float));

    if (ai_mesh->HasNormals())
    {
        mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::NORMALS)] = ai_mesh->mNumVertices * 3;
        mesh->normals = new float[mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::NORMALS)]];
        memcpy(mesh->normals, ai_mesh->mNormals, mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::NORMALS)] * sizeof(float));
    }
    else
    {
        mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::NORMALS)] = 0;
        mesh->normals = nullptr;
    }

    if (ai_mesh->HasTextureCoords(0))
    {
        mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::TEX_COORDS)] = ai_mesh->mNumVertices * 2;
        mesh->tex_coords = new float[mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::TEX_COORDS)]];
        for (unsigned i = 0; i < ai_mesh->mNumVertices; i++)
        {
            memcpy(&mesh->tex_coords[i * 2], &ai_mesh->mTextureCoords[0][i], 2 * sizeof(unsigned));
        }
    }
    else
    {
        mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::TEX_COORDS)] = 0;
        mesh->tex_coords = nullptr;
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
        mesh->tangents = nullptr;
    }

    mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::INDICES)] = ai_mesh->mNumFaces * 3;
    mesh->indices = new unsigned[mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::INDICES)]];
    for (unsigned i = 0; i < ai_mesh->mNumFaces; ++i)
    {
        memcpy(&mesh->indices[i * 3], ai_mesh->mFaces[i].mIndices, 3 * sizeof(unsigned));
    }

    mesh->GenerateBuffers();
    mesh->GenerateAABB();
    mesh->loaded = true;

    Save(mesh);

    delete mesh;
}