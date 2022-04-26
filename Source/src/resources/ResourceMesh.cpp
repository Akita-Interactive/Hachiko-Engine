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

    // Indices (1 value)
    glGenBuffers(1, &buffer_ids[static_cast<int>(Buffers::INDICES)]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_ids[static_cast<int>(Buffers::INDICES)]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer_sizes[static_cast<int>(Buffers::INDICES)] * sizeof(unsigned), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Hachiko::ResourceMesh::GenerateAABB()
{
    bounding_box.SetFrom((float3*)vertices, buffer_sizes[static_cast<int>(Buffers::VERTICES)] / 3);
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
    }
    loaded = false;
}
