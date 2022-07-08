#include "core/hepch.h"
#include "GeometryBatch.h"

#include "modules/ModuleProgram.h"

#include "core/GameObject.h"
#include "components/ComponentMeshRenderer.h"
#include "components/ComponentTransform.h"

#include "batching/TextureBatch.h"

Hachiko::GeometryBatch::GeometryBatch(ResourceMesh::Layout batch_layout) {
    batch = new ResourceMesh(0);
    batch->layout = batch_layout;

    texture_batch = new TextureBatch();

    GenerateBuffers();
}

Hachiko::GeometryBatch::~GeometryBatch() {
    for (auto& resource : resources)
    {
        delete resource.second;
    }

    if (batch->loaded)
    {
        glDeleteBuffers(1, &instance_indices_vbo);
        glDeleteBuffers(1, &transform_buffer);
        glDeleteBuffers(1, &palettes_buffer);
        glDeleteBuffers(1, &palettes_per_instances_buffer);
    }

    RELEASE(batch);
    RELEASE(texture_batch);
}

void Hachiko::GeometryBatch::AddMesh(const ComponentMeshRenderer* mesh_renderer)
{
    const ResourceMesh* resource = mesh_renderer->GetResourceMesh();
    auto it = resources.find(resource);

    if (it == resources.end())
    {
        // It will be formatted when generating batches
        resources[resource] = new DrawCommand();
    }

    texture_batch->AddMaterial(mesh_renderer->GetResourceMaterial());

    component_count += 1;
    if (resource->num_bones > 0)
    {
        component_palette_count += resource->num_bones;
    }
}

void Hachiko::GeometryBatch::AddDrawComponent(const ComponentMeshRenderer* mesh)
{
    components.push_back(mesh);
}

void Hachiko::GeometryBatch::BuildBatch() {

    BatchMeshes();
    UpdateBuffers();

    texture_batch->BuildBatch(component_count);
}

void Hachiko::GeometryBatch::BatchMeshes()
{
    unsigned resource_index = 0;

    static const int buffer_types[] = {static_cast<int>(ResourceMesh::Buffers::INDICES),
                                       static_cast<int>(ResourceMesh::Buffers::VERTICES),
                                       static_cast<int>(ResourceMesh::Buffers::NORMALS),
                                       static_cast<int>(ResourceMesh::Buffers::TEX_COORDS),
                                       static_cast<int>(ResourceMesh::Buffers::TANGENTS),
                                       static_cast<int>(ResourceMesh::Buffers::BONES_INDICES),
                                       static_cast<int>(ResourceMesh::Buffers::BONES_WEIGHTS)};

    // Index all resources and compute total size to generate batch buffers;
    for (auto& resource : resources)
    {
        const ResourceMesh* r = resource.first;

        for (int buffer_type : buffer_types)
        {
            batch->buffer_sizes[buffer_type] += r->buffer_sizes[buffer_type];
        }    
    }

    batch->indices = new unsigned[batch->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::INDICES)]];
    batch->vertices = new float[batch->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::VERTICES)]];
    batch->normals = new float[batch->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::NORMALS)]];
    batch->tex_coords = new float[batch->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::TEX_COORDS)]];
    batch->tangents = new float[batch->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::TANGENTS)]];
    batch->src_bone_indices = std::make_unique<unsigned[]>(batch->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::BONES_INDICES)]);
    batch->src_bone_weights = std::make_unique<float4[]>(batch->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::BONES_WEIGHTS)]);

    unsigned indices_offset = 0;
    unsigned vertices_offset = 0;
    unsigned normals_offset = 0;
    unsigned tex_coords_offset = 0;
    unsigned tangents_offset = 0;
    unsigned bones_indices_offset = 0;
    unsigned bones_weights_offset = 0;

    for (auto& resource : resources)
    {
        const ResourceMesh* r = resource.first;
        DrawCommand* command = resource.second;

        command->count = r->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::INDICES)];
        command->first_index = indices_offset;
        command->base_vertex = static_cast<int>(vertices_offset / 3);

        unsigned size = r->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::INDICES)];
        unsigned size_bytes = sizeof(unsigned) * size;
        memcpy(&batch->indices[indices_offset], r->indices, size_bytes);
        indices_offset += size;

        size = r->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::VERTICES)];
        size_bytes = sizeof(float) * size;
        memcpy(&batch->vertices[vertices_offset], r->vertices, size_bytes);
        vertices_offset += size;

        size = r->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::NORMALS)];
        size_bytes = sizeof(float) * size;
        memcpy(&batch->normals[normals_offset], r->normals, size_bytes);
        normals_offset += size;

        size = r->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::TEX_COORDS)];
        size_bytes = sizeof(float) * size;
        memcpy(&batch->tex_coords[tex_coords_offset], r->tex_coords, size_bytes);
        tex_coords_offset += size;

        size = r->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::TANGENTS)];
        size_bytes = sizeof(float) * size;
        memcpy(&batch->tangents[tangents_offset], r->tangents, size_bytes);
        tangents_offset += size;

        if (batch->layout.bones)
        {
            size = r->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::BONES_INDICES)];
            size_bytes = sizeof(unsigned) * size;
            memcpy(&batch->src_bone_indices[bones_indices_offset], r->src_bone_indices.get(), size_bytes);
            bones_indices_offset += size;

            size = r->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::BONES_WEIGHTS)];
            size_bytes = sizeof(float4) * size;
            memcpy(batch->src_bone_weights[bones_weights_offset].ptr(), r->src_bone_weights[0].ptr(), size_bytes);
            bones_weights_offset += size;
        }
    }
}

void Hachiko::GeometryBatch::UpdateWithTextureBatch(const Program* program, bool use_first_segment)
{
    BatchData();
    GenerateCommands();

    glBindVertexArray(batch->vao);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirect_buffer_id);

    if (use_first_segment)
    {
        memcpy(transform_buffer_data, transforms.data(), transforms.size() * sizeof(float4x4));
        if (batch->layout.bones)
        {
            memcpy(palettes_buffer_data, palettes.data(), palettes.size() * sizeof(float4x4));
            memcpy(palettes_per_instances_buffer_data, palettes_per_instance.data(), palettes_per_instance.size() * sizeof(PalettePerInstance));
        }
    }
    else
    {
        memcpy(&transform_buffer_data[component_count], transforms.data(), transforms.size() * sizeof(float4x4));
        if (batch->layout.bones)
        {
            memcpy(&palettes_buffer_data[component_palette_count], palettes.data(), palettes.size() * sizeof(float4x4));
            memcpy(&palettes_per_instances_buffer_data[component_count], palettes_per_instance.data(), palettes_per_instance.size() * sizeof(PalettePerInstance));
        }
    }

    texture_batch->Draw(program, components, use_first_segment, component_count);
}

void Hachiko::GeometryBatch::BatchData()
{
    transforms.reserve(components.size());
    if (batch->layout.bones)
    {
        palettes_per_instance.reserve(components.size());

        PalettePerInstance palette_per_instance;
        int palette_offset = 0;
        for (const ComponentMeshRenderer* component : components)
        {
            if (component->GetPalette().size() <= 0)
            {
                continue;
            }
            transforms.push_back(component->GetGameObject()->GetTransform()->GetGlobalMatrix());

            palette_per_instance.numBones = component->GetResourceMesh()->num_bones;
            palette_per_instance.paletteOffset = palette_offset;
            palettes_per_instance.push_back(palette_per_instance);

            palettes.resize(palette_offset + palette_per_instance.numBones);
            memcpy(&palettes[palette_offset], component->GetPalette().data(), sizeof(float4x4) * palette_per_instance.numBones);

            palette_offset += palette_per_instance.numBones;
        }
    }
    else
    {
        for (const ComponentMeshRenderer* component : components)
        {
            transforms.push_back(component->GetGameObject()->GetTransform()->GetGlobalMatrix());
        }
    }
}

void Hachiko::GeometryBatch::GenerateCommands()
{
    commands.reserve(components.size());

    unsigned instance = 0;
    for (const ComponentMeshRenderer* component : components)
    {
        const ResourceMesh* r = component->GetResourceMesh();
        DrawCommand command = (*resources[r]);
        command.base_instance = instance;
        commands.emplace_back(command);
        ++instance;
    }

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirect_buffer_id);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, commands.size() * sizeof(DrawCommand), commands.data(), GL_DYNAMIC_DRAW);
}

void Hachiko::GeometryBatch::ClearDrawList()
{
    commands.clear();
    components.clear();
    transforms.clear();
    //TODO: clear texture batch
    texture_batch->Clear();
    palettes.clear();
    palettes_per_instance.clear();
}

void Hachiko::GeometryBatch::BindBuffers(bool use_first_segment)
{
    if (use_first_segment)
    {
        glBindBuffer(GL_ARRAY_BUFFER, transform_buffer);
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, static_cast<int>(ModuleProgram::BINDING::MODEL), transform_buffer, 0, component_count * sizeof(float4x4));

        if (batch->layout.bones)
        {
            glBindBuffer(GL_ARRAY_BUFFER, palettes_buffer);
            glBindBufferRange(GL_SHADER_STORAGE_BUFFER, static_cast<int>(ModuleProgram::BINDING::PALETTE), palettes_buffer, 0, component_palette_count * sizeof(float4x4));

            glBindBuffer(GL_ARRAY_BUFFER, palettes_per_instances_buffer);
            glBindBufferRange(GL_SHADER_STORAGE_BUFFER, static_cast<int>(ModuleProgram::BINDING::PALETTE_PER_INSTANCE), palettes_per_instances_buffer, 0, component_count * sizeof(PalettePerInstance));
        }
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, transform_buffer);
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, static_cast<int>(ModuleProgram::BINDING::MODEL), transform_buffer, component_count * sizeof(float4x4), component_count * sizeof(float4x4));

        if (batch->layout.bones)
        {
            glBindBuffer(GL_ARRAY_BUFFER, palettes_buffer);
            glBindBufferRange(GL_SHADER_STORAGE_BUFFER,
                              static_cast<int>(ModuleProgram::BINDING::PALETTE),
                              palettes_buffer,
                              component_palette_count * sizeof(float4x4),
                              component_palette_count * sizeof(float4x4));

            glBindBuffer(GL_ARRAY_BUFFER, palettes_per_instances_buffer);
            glBindBufferRange(GL_SHADER_STORAGE_BUFFER,
                              static_cast<int>(ModuleProgram::BINDING::PALETTE_PER_INSTANCE),
                              palettes_per_instances_buffer,
                              component_count * sizeof(PalettePerInstance),
                              component_count * sizeof(PalettePerInstance));
        }
    }
}

void Hachiko::GeometryBatch::GenerateBuffers()
{
    glGenVertexArrays(1, &batch->vao);
    glBindVertexArray(batch->vao);

    // Positions (3 values per coord)
    glGenBuffers(1, &batch->buffer_ids[static_cast<int>(ResourceMesh::Buffers::VERTICES)]);
    glBindBuffer(GL_ARRAY_BUFFER, batch->buffer_ids[static_cast<int>(ResourceMesh::Buffers::VERTICES)]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(0);

    // Normals (3 values per coord)
    glGenBuffers(1, &batch->buffer_ids[static_cast<int>(ResourceMesh::Buffers::NORMALS)]);
    glBindBuffer(GL_ARRAY_BUFFER, batch->buffer_ids[static_cast<int>(ResourceMesh::Buffers::NORMALS)]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(1);

    // Texture Coords (2 values per coord)
    glGenBuffers(1, &batch->buffer_ids[static_cast<int>(ResourceMesh::Buffers::TEX_COORDS)]);
    glBindBuffer(GL_ARRAY_BUFFER, batch->buffer_ids[static_cast<int>(ResourceMesh::Buffers::TEX_COORDS)]);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(2);

    // Tangents (3 values per coord)
    glGenBuffers(1, &batch->buffer_ids[static_cast<int>(ResourceMesh::Buffers::TANGENTS)]);
    glBindBuffer(GL_ARRAY_BUFFER, batch->buffer_ids[static_cast<int>(ResourceMesh::Buffers::TANGENTS)]);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(3);

    if (batch->layout.bones)
    {
        // Bones index (4 values per coord)
        glGenBuffers(1, &batch->buffer_ids[static_cast<int>(ResourceMesh::Buffers::BONES_INDICES)]);
        glBindBuffer(GL_ARRAY_BUFFER, batch->buffer_ids[static_cast<int>(ResourceMesh::Buffers::BONES_INDICES)]);
        glVertexAttribIPointer(4, 4, GL_UNSIGNED_INT, 4 * sizeof(unsigned), static_cast<void*>(nullptr));
        glEnableVertexAttribArray(4);

        // Bones weights (4 values per coord)
        glGenBuffers(1, &batch->buffer_ids[static_cast<int>(ResourceMesh::Buffers::BONES_WEIGHTS)]);
        glBindBuffer(GL_ARRAY_BUFFER, batch->buffer_ids[static_cast<int>(ResourceMesh::Buffers::BONES_WEIGHTS)]);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), static_cast<void*>(nullptr));
        glEnableVertexAttribArray(5);
    }

    // Instance indices (one per component to draw)
    glGenBuffers(1, &instance_indices_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, instance_indices_vbo);
    glVertexAttribPointer(6, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(unsigned), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(6);
    glVertexAttribDivisor(6, 1); // advances divisor times per instance/draw command

    // Indices (1 value)
    glGenBuffers(1, &batch->buffer_ids[static_cast<int>(ResourceMesh::Buffers::INDICES)]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch->buffer_ids[static_cast<int>(ResourceMesh::Buffers::INDICES)]);
    
    glGenBuffers(1, &indirect_buffer_id);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirect_buffer_id);
    
    glBindVertexArray(0);
    
    batch->loaded = true;
}

void Hachiko::GeometryBatch::UpdateBuffers()
{
    glBindVertexArray(batch->vao);
    glBindBuffer(GL_ARRAY_BUFFER, batch->buffer_ids[static_cast<int>(ResourceMesh::Buffers::VERTICES)]);
    glBufferData(GL_ARRAY_BUFFER, batch->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::VERTICES)] * sizeof(float), batch->vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, batch->buffer_ids[static_cast<int>(ResourceMesh::Buffers::NORMALS)]);
    glBufferData(GL_ARRAY_BUFFER, batch->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::NORMALS)] * sizeof(float), batch->normals, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, batch->buffer_ids[static_cast<int>(ResourceMesh::Buffers::TEX_COORDS)]);
    glBufferData(GL_ARRAY_BUFFER, batch->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::TEX_COORDS)] * sizeof(float), batch->tex_coords, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, batch->buffer_ids[static_cast<int>(ResourceMesh::Buffers::TANGENTS)]);
    glBufferData(GL_ARRAY_BUFFER, batch->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::TANGENTS)] * sizeof(float), batch->tangents, GL_DYNAMIC_DRAW);

    if (batch->layout.bones)
    {
        glBindBuffer(GL_ARRAY_BUFFER, batch->buffer_ids[static_cast<int>(ResourceMesh::Buffers::BONES_INDICES)]);
        glBufferData(GL_ARRAY_BUFFER, batch->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::BONES_INDICES)] * sizeof(unsigned), batch->src_bone_indices.get(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, batch->buffer_ids[static_cast<int>(ResourceMesh::Buffers::BONES_WEIGHTS)]);
        glBufferData(GL_ARRAY_BUFFER, batch->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::BONES_WEIGHTS)] * sizeof(float4), batch->src_bone_weights.get(), GL_DYNAMIC_DRAW);
    }

    std::vector<unsigned> indices_vbo(components.size());
    std::iota(std::begin(indices_vbo), std::end(indices_vbo), 0);

    glBindBuffer(GL_ARRAY_BUFFER, instance_indices_vbo);
    glBufferData(GL_ARRAY_BUFFER, components.size() * sizeof(unsigned), indices_vbo.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch->buffer_ids[static_cast<int>(ResourceMesh::Buffers::INDICES)]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, batch->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::INDICES)] * sizeof(unsigned), batch->indices, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);

    transform_buffer_data = static_cast<float4x4*>(
        App->program->CreatePersistentBuffers(transform_buffer, static_cast<int>(ModuleProgram::BINDING::MODEL), 2 * component_count * sizeof(float4x4))
    );
    if (batch->layout.bones)
    {
        palettes_buffer_data = static_cast<float4x4*>(
            App->program->CreatePersistentBuffers(palettes_buffer, static_cast<int>(ModuleProgram::BINDING::PALETTE), 2 * component_palette_count * sizeof(float4x4))
        );
        palettes_per_instances_buffer_data = static_cast<PalettePerInstance*>(
            App->program->CreatePersistentBuffers(palettes_per_instances_buffer, static_cast<int>(ModuleProgram::BINDING::PALETTE_PER_INSTANCE), 2 * component_count * sizeof(PalettePerInstance))
        );
    }
}

void Hachiko::GeometryBatch::ImGuiWindow() 
{
    for (unsigned i = 0; i < components.size(); ++i)
    {
        ImGui::Text(components[i]->GetGameObject()->name.c_str());
    }
    texture_batch->ImGuiWindow();
}
