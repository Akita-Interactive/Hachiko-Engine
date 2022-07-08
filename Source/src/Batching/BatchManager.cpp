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
                    if (geometry_batch->batch->layout.Equal(resource->layout))
                    {
                        geometry_batch->AddMesh(mesh_renderer);
                        batch_found = true;
                        break;
                    }
                }
                if (!batch_found)
                {
                    // Create new batch if there are no matching ones
                    GeometryBatch* new_batch = new GeometryBatch(resource->layout);
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

void Hachiko::BatchManager::DrawOpaqueBatches(const Program* program)
{
    for (GeometryBatch* geometry_batch : geometry_batches_opaque)
    {
        DrawSingleBatch(geometry_batch, program, use_first_segment_opaque);
    }

    use_first_segment_opaque = !use_first_segment_opaque;
}

void Hachiko::BatchManager::DrawTransparentBatches(const Program* program) 
{
    for (GeometryBatch* geometry_batch : geometry_batches_transparent)
    {
        DrawSingleBatch(geometry_batch, program, use_first_segment_transparent);
    }

    use_first_segment_transparent = !use_first_segment_transparent;
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
    ImGui::Text("Opaque Batches");
    ImGui::Separator();
    ShowDebugMenuForBatches(geometry_batches_opaque);

    ImGui::NewLine();

    ImGui::Text("Transparent Batches"); 
    ImGui::Separator();
    ShowDebugMenuForBatches(geometry_batches_transparent);
}

void Hachiko::BatchManager::ShowDebugMenuForBatches(
    const std::vector<GeometryBatch*>& batches) const 
{
    for (GeometryBatch* geometry_batch : batches)
    {
        ImGui::Text(geometry_batch->batch->layout.bones ? "Bones = true; " : "Bones = false; ");
        ImGui::SameLine();
        ImGui::Text(geometry_batch->batch->layout.normals ? "Normals = true; " : "Normals = false; ");
        ImGui::SameLine();
        ImGui::Text(geometry_batch->batch->layout.text_coords ? "TexCoords = true; " : "TexCoords = false; ");

        geometry_batch->ImGuiWindow();
        ImGui::Separator();
    }
}

void Hachiko::BatchManager::DrawSingleBatch(GeometryBatch* geometry_batch, const Program* program, bool use_first_segment) const 
{
    // Binds meshes and transforms
    geometry_batch->UpdateWithTextureBatch(program, use_first_segment);
    geometry_batch->BindBuffers(use_first_segment);

    // Bind texture batch
    // bind materials array
    auto& commands = geometry_batch->GetCommands();

    program->BindUniformBool("has_bones", geometry_batch->batch->layout.bones);
    int persistent_offset = (use_first_segment) ? 0 : geometry_batch->component_count;
    program->BindUniformInts("persistent_offset", 1, &persistent_offset);

    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0, commands.size(), 0);
}