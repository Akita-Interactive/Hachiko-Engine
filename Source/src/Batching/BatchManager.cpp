#include "core/hepch.h"
#include "BatchManager.h"

#include "Application.h"
#include "core/GameObject.h"
#include "components/ComponentTransform.h"
#include "components/ComponentMeshRenderer.h"
#include "resources/ResourceMesh.h"
#include "modules/ModuleProgram.h"

Hachiko::BatchManager::BatchManager() 
{
}

Hachiko::BatchManager::~BatchManager()
{
    CleanUp();
}

void Hachiko::BatchManager::CollectMeshes(const GameObject* game_object)
{
    CollectMesh(game_object);

    for (GameObject* child : game_object->children)
    {
        if (child)
        {
            CollectMeshes(child);
        }
    }    
}

void Hachiko::BatchManager::CollectMesh(const GameObject* game_object)
{
    const std::vector<Component*> components = game_object->GetComponents();
    
    for (unsigned i = 0; i < components.size(); ++i)
    {
        if (components[i]->GetType() == Component::Type::MESH_RENDERER)
        {
            const ComponentMeshRenderer* mesh_renderer = static_cast<ComponentMeshRenderer*>(components[i]);
            const ResourceMesh* resource = mesh_renderer->GetResourceMesh();
            const ResourceMaterial* resource_material = mesh_renderer->GetResourceMaterial();
            bool batch_found = false;

            if (mesh_renderer->GetResourceMaterial())
            {
                // Switch between correct batches vector based on transparency property of the material:
                std::vector<GeometryBatch*>* geometry_batches = 
                    mesh_renderer->GetResourceMaterial()->is_transparent 
                        ? &geometry_batches_transparent 
                        : &geometry_batches_opaque;      
                // Find matching batch to include the mesh
                for (GeometryBatch* geometry_batch : (*geometry_batches))
                {
                    if (geometry_batch->layout.mesh_layout.Equal(resource->layout) && 
                        geometry_batch->layout.shader == resource_material->shader)
                    {
                        geometry_batch->AddMesh(mesh_renderer);
                        batch_found = true;
                        break;
                    }
                }
                if (!batch_found)
                {
                    GeometryBatch::Layout layout;
                    layout.mesh_layout = resource->layout;
                    layout.shader = resource_material->shader;

                    // Create new batch if there are no matching ones
                    GeometryBatch* new_batch = new GeometryBatch(layout);
                    new_batch->AddMesh(mesh_renderer);
                    geometry_batches->push_back(new_batch);
                }
            }


        }
    }
}

void Hachiko::BatchManager::AddDrawComponent(const ComponentMeshRenderer* mesh_renderer)
{
    const ResourceMesh* resource = mesh_renderer->GetResourceMesh();

    std::vector<GeometryBatch*>& geometry_batches = 
                mesh_renderer->GetResourceMaterial()->is_transparent 
                    ? geometry_batches_transparent 
                    : geometry_batches_opaque;

    for (GeometryBatch* geometry_batch : geometry_batches)
    {
        if (geometry_batch->batch->layout.Equal(resource->layout))
        {
            // We assume that the resource is already contained in the batch
            geometry_batch->AddDrawComponent(mesh_renderer);
            return;
        }
    }
}

void Hachiko::BatchManager::BuildBatches()
{
    for (GeometryBatch* geometry_batch : geometry_batches_opaque)
    {
        geometry_batch->BuildBatch();
    }

    for (GeometryBatch* geometry_batch : geometry_batches_transparent)
    {
        geometry_batch->BuildBatch();
    }
}

void Hachiko::BatchManager::DrawOpaqueBatches(
    const Program* program, 
    bool override_segment_skip)
{
    for (GeometryBatch* geometry_batch : geometry_batches_opaque)
    {
        DrawSingleBatch(geometry_batch, program, opaque_buffers_segment);
    }

    // We can set override segment skip to true if we are basically gonna draw
    // same things twice:
    if (!override_segment_skip)
    {
        opaque_buffers_segment = 
            (opaque_buffers_segment + 1) % BatchingProperties::MAX_SEGMENTS;
    }
}

void Hachiko::BatchManager::DrawTransparentBatches(
    const Program* program, 
    bool override_segment_skip)
{
    for (GeometryBatch* geometry_batch : geometry_batches_transparent)
    {
        DrawSingleBatch(geometry_batch, program, transparent_buffers_segment);
    }

    // We can set override segment skip to true if we are basically gonna draw
    // same things twice:
    if (!override_segment_skip)
    {
        transparent_buffers_segment = 
            (transparent_buffers_segment + 1) % BatchingProperties::MAX_SEGMENTS;
    }
}

void Hachiko::BatchManager::ClearOpaqueBatchesDrawList()
{
    for (GeometryBatch* geometry_batch : geometry_batches_opaque)
    {
        geometry_batch->ClearDrawList();
    }
}

void Hachiko::BatchManager::ClearTransparentBatchesDrawList() 
{
    for (GeometryBatch* geometry_batch : geometry_batches_transparent)
    {
        geometry_batch->ClearDrawList();
    }
}

void Hachiko::BatchManager::CleanUp()
{
    for (GeometryBatch* geometry_batch : geometry_batches_opaque)
    {
        delete geometry_batch;
    }

    for (GeometryBatch* geometry_batch : geometry_batches_transparent)
    {
        delete geometry_batch;
    }

    geometry_batches_opaque.clear();

    geometry_batches_transparent.clear();
}

void Hachiko::BatchManager::DebugMenu() 
{
    ImGui::TextWrapped("Opaque batches");
    ImGui::Separator();
    ShowDebugMenuForBatches(geometry_batches_opaque);

    ImGui::Separator();

    ImGui::TextWrapped("Transparent batches"); 
    ImGui::Separator();
    ShowDebugMenuForBatches(geometry_batches_transparent);
}

void Hachiko::BatchManager::ShowDebugMenuForBatches(
    const std::vector<GeometryBatch*>& batches) const 
{
    for (int i = 0; i < batches.size(); ++i)
    {
        ImGui::TextWrapped("Geometry batch %i:", i);
        ImGui::BulletText("Bones %d", batches[i]->batch->layout.bones);
        ImGui::BulletText("Normals %d", batches[i]->batch->layout.normals);
        ImGui::BulletText("TexCoords %d", batches[i]->batch->layout.text_coords);
        ImGui::NewLine();

        batches[i]->ImGuiWindow();

        ImGui::NewLine();
    }
}

void Hachiko::BatchManager::DrawSingleBatch(GeometryBatch* geometry_batch, const Program* program, int segment) const 
{
    // Update and bind batch
    if (geometry_batch->dirty_draw_components)
    {
        geometry_batch->UpdateCommands();
        geometry_batch->UpdateBatch(segment);
        geometry_batch->dirty_draw_components = false;
    }

    geometry_batch->BindBatch(segment, program);


    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0, geometry_batch->GetCommandAmount(), 0);
    glBindVertexArray(0);

    // Fence sync if manual sync waiting is enabled:
    geometry_batch->FenceSync(segment);
}