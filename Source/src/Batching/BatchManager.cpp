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
        CollectMeshes(child);
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

            // Find matching batch to include the mesh
            for (GeometryBatch* geometry_batch : geometry_batches)
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
                geometry_batches.push_back(new_batch);
            }
        }
    }
}

void Hachiko::BatchManager::AddDrawComponent(const ComponentMeshRenderer* mesh_renderer)
{
    const ResourceMesh* resource = mesh_renderer->GetResourceMesh();

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
    for (GeometryBatch* geometry_batch : geometry_batches)
    {
        geometry_batch->BuildBatch();
    }
}

void Hachiko::BatchManager::DrawBatches()
{
    static float4x4 identity = float4x4::identity;
    for (GeometryBatch* geometry_batch : geometry_batches)
    {
        // Binds meshes and transforms
        geometry_batch->UpdateWithTextureBatch(use_first_segment);
        geometry_batch->BindBuffers(use_first_segment);
        //App->program->GetMainProgram()->BindUniformFloat4x4("model", identity.ptr());
        // Bind texture batch
        // bind materials array
        auto& commands = geometry_batch->GetCommands();

        App->program->GetMainProgram()->BindUniformBool("has_bones", geometry_batch->batch->layout.bones);
        int persistent_offset = (use_first_segment) ? 0 : geometry_batch->component_count;
        App->program->GetMainProgram()->BindUniformInts("persistent_offset", 1, &persistent_offset);

        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0, commands.size(), 0);
    }

    use_first_segment = !use_first_segment;
}

void Hachiko::BatchManager::ClearBatchesDrawList()
{
    for (GeometryBatch* geometry_batch : geometry_batches)
    {
        geometry_batch->ClearDrawList();
    }
}

void Hachiko::BatchManager::CleanUp()
{
    for (GeometryBatch* geometry_batch : geometry_batches)
    {
        delete geometry_batch;
    }
    geometry_batches.clear();
}

void Hachiko::BatchManager::DebugMenu() 
{
    for (GeometryBatch* geometry_batch : geometry_batches)
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
