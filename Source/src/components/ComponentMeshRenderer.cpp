#include "core/hepch.h"

#include "modules/ModuleProgram.h"
#include "modules/ModuleResources.h"
#include "modules/ModuleSceneManager.h"

#include "resources/ResourceMesh.h"

#include "Application.h"

#include "ComponentAnimation.h"
#include "ComponentMeshRenderer.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"

#include "modules/ModuleEvent.h"
#include "modules/ModuleResources.h"

#include <debugdraw.h>

Hachiko::ComponentMeshRenderer::ComponentMeshRenderer(GameObject* container, UID id, ResourceMesh* res) 
    : Component(Type::MESH_RENDERER, container)
{
    SetResourceMesh(res);
}

Hachiko::ComponentMeshRenderer::~ComponentMeshRenderer() 
{
    App->resources->ReleaseResource(mesh);
    App->resources->ReleaseResource(material);
    if (game_object->scene_owner)
    {
        game_object->scene_owner->GetQuadtree()->Remove(this);
    }
    delete[] node_cache;
}

void Hachiko::ComponentMeshRenderer::Update()
{
    if (mesh == nullptr)
    {
        return;

    }

    // Material override
    if (override_material)
    {
        override_timer -= GameTimer::delta_time;
        if (override_timer <= 0)
        {
            override_material = false;
            override_timer = 0;
        }
    }

    if (palette.empty())
    {
        palette.resize(mesh->num_bones);
        for (unsigned i = 0; i < mesh->num_bones; ++i)
        {
            palette[i] = float4x4::identity;
        }
    }

    // SKINNIG PER FRAME

    UpdateSkinPalette(palette);
}

void Hachiko::ComponentMeshRenderer::Draw(ComponentCamera* camera, Program* program)
{
    OPTICK_CATEGORY("Draw", Optick::Category::Rendering);

    if (mesh == nullptr || !visible)
    {
        return;
    }
    
    program->BindUniformFloat4x4("model", game_object->GetTransform()->GetGlobalMatrix().ptr());

    // SKINING

    if (!palette.empty())
    {
        glUniformMatrix4fv(glGetUniformLocation(program->GetId(), "palette"), palette.size(), true, palette[0].ptr());
    }
    program->BindUniformBool("has_bones", mesh->num_bones > 0);

    App->program->UpdateMaterial(this);
    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::INDICES)], GL_UNSIGNED_INT, nullptr);
}

void Hachiko::ComponentMeshRenderer::DrawStencil(ComponentCamera* camera, Program* program) const
{
    if (mesh == nullptr)
    {
        return;
    }
    
    program->BindUniformFloat4x4("model", game_object->GetTransform()->GetGlobalMatrix().ptr());

    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::INDICES)], GL_UNSIGNED_INT, nullptr);
}

void Hachiko::ComponentMeshRenderer::DebugDraw()
{
    ddVec3 p[8];
    // This order was pure trial and error, i dont know how to really do it
    // Using center and points does not show the rotation
    static const int order[8] = {0, 1, 5, 4, 2, 3, 7, 6};
    for (int i = 0; i < 8; ++i)
    {
        p[i] = obb.CornerPoint(order[i]);
    }
    dd::box(p, dd::colors::White);
}

void Hachiko::ComponentMeshRenderer::OnTransformUpdated()
{
    UpdateBoundingBoxes();
}

void Hachiko::ComponentMeshRenderer::SetResourceMesh(ResourceMesh* res)
{
    App->resources->ReleaseResource(mesh);
    mesh = res;

    if (!mesh)
    {
        return;
    }

    if (mesh->num_bones > 0)
    {
        node_cache = new const GameObject*[mesh->num_bones];

        for (unsigned int i = 0; i < mesh->num_bones; ++i)
        {
            node_cache[i] = nullptr;
        }
    }
}

void Hachiko::ComponentMeshRenderer::LoadMesh(UID mesh_id)
{
    App->resources->ReleaseResource(mesh);
    SetResourceMesh(static_cast<ResourceMesh*> (App->resources->GetResource(Resource::Type::MESH, mesh_id)));
}

void Hachiko::ComponentMeshRenderer::LoadMaterial(UID material_id)
{
    App->resources->ReleaseResource(material);
    material = static_cast<ResourceMaterial*>(App->resources->GetResource(Resource::Type::MATERIAL, material_id));
}

void Hachiko::ComponentMeshRenderer::DrawGui()
{
    static const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
    ImGui::PushID(this);
    if (ImGui::CollapsingHeader("Mesh renderer", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::TreeNodeEx((void*)&mesh, flags, "Mesh"))
        {
            if (mesh != nullptr)
            {
                ImGui::Text("%d Triangles\n%d vertices\n%d indices\n%d bones binded",
                            mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::INDICES)] / 3,
                            mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::VERTICES)],
                            mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::INDICES)],
                            mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::BONES)]);
                if(ImGui::Checkbox("Visible", &visible))
                {
                    App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
                }
                if(ImGui::Checkbox("Navigable", &navigable))
                {
                    App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
                }
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx((void*)&material, flags, "Material"))
        {
            ChangeMaterial();
            if (material != nullptr)
            {
                material->DrawGui();
            }
            else
            {
                ImGui::Text("No material resource");
            }
            ImGui::TreePop();
        }
    }
    ImGui::PopID();
}

void Hachiko::ComponentMeshRenderer::Save(YAML::Node& node) const
{
    if (mesh != nullptr)
    {
        node[RENDERER_MESH_ID] = mesh->GetID();
        node[MESH_NAVIGABLE] = navigable;
        node[MESH_VISIBLE] = visible;
    }
    else
    {
        node[RENDERER_MESH_ID] = 0;
        node[MESH_NAVIGABLE] = false;
        node[MESH_VISIBLE] = true;
    }

    if (material != nullptr)
    {
        node[RENDERER_MATERIAL_ID] = material->GetID();
    }
    else
    {
        node[RENDERER_MATERIAL_ID] = 0;
    }
}

void Hachiko::ComponentMeshRenderer::Load(const YAML::Node& node)
{
    UID mesh_id = node[RENDERER_MESH_ID].as<UID>();
    UID material_id = node[RENDERER_MATERIAL_ID].as<UID>();
    if (mesh_id)
    {
        navigable = node[MESH_NAVIGABLE].IsDefined() ? node[MESH_NAVIGABLE].as<bool>() : false;
        visible = node[MESH_VISIBLE].IsDefined() ? node[MESH_VISIBLE].as<bool>() : true;

        LoadMesh(mesh_id);
    }
    if (material_id)
    {
        LoadMaterial(material_id);
    }
}

Hachiko::GameObject* GetRoot(Hachiko::GameObject* posible_root) 
{
    if (!posible_root)
    {
        return nullptr;
    }

    if (posible_root->GetComponent<Hachiko::ComponentAnimation>())
    {
        return posible_root;
    }

    GetRoot(posible_root->parent);
}

void Hachiko::ComponentMeshRenderer::UpdateSkinPalette(std::vector<float4x4>& palette) const
{
    if (mesh && mesh->num_bones > 0)
    {
        const GameObject* root = GetRoot(game_object);

        if (!root)
        {
            HE_LOG("Root not found");
            return;
        }     

        float4x4 root_transform = root->GetTransform()->GetGlobalMatrix().Inverted();

        for (unsigned i = 0; i < mesh->num_bones; ++i)
        {
            const ResourceMesh::Bone& bone = mesh->bones[i];
            const GameObject* bone_node = node_cache[i];

            if (bone_node == nullptr)
            {
                bone_node = node_cache[i] = root ? root->FindDescendantWithName(bone.name) : nullptr;
            }

            if (bone_node)
            {
                palette[i] = root_transform * bone_node->GetTransform()->GetGlobalMatrix() * bone.bind;
            }
            else
            {
                palette[i] = float4x4::identity;
            }
        }
    }
}

void Hachiko::ComponentMeshRenderer::ChangeMaterial() 
{
    const std::string title = StringUtils::Concat("Select Material#", std::to_string(uid));
    
    if (ImGui::Button("Select material"))
    {
        ImGuiFileDialog::Instance()->OpenDialog(title.c_str(),
                                                "Select Material",
                                                MATERIAL_EXTENSION,
                                                ASSETS_FOLDER_MATERIAL,
                                                1,
                                                nullptr,
                                                ImGuiFileDialogFlags_DontShowHiddenFiles | ImGuiFileDialogFlags_DisableCreateDirectoryButton | ImGuiFileDialogFlags_HideColumnType
                                                    | ImGuiFileDialogFlags_HideColumnDate);
    }

    if (ImGuiFileDialog::Instance()->Display(title.c_str()))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string material_path = ImGuiFileDialog::Instance()->GetFilePathName();
            material_path.append(META_EXTENSION);
            YAML::Node material_node = YAML::LoadFile(material_path);

            ResourceMaterial* res = static_cast<ResourceMaterial*>(App->resources->GetResource(Resource::Type::MATERIAL, material_node[RESOURCES][0][RESOURCE_ID].as<UID>()));
            if (res != nullptr)
            {
                App->resources->ReleaseResource(material);
                material = res;
            }
        }

        ImGuiFileDialog::Instance()->Close();
    }
}

void Hachiko::ComponentMeshRenderer::OverrideEmissive(float4 color, float time) 
{
    override_material = true;
    override_timer = time;
    override_emissive = color;
}

void Hachiko::ComponentMeshRenderer::UpdateBoundingBoxes()
{
    obb = GetMeshAABB();
    obb.Transform(game_object->GetTransform()->GetGlobalMatrix());
    // Enclose is accumulative, reset the box
    aabb.SetNegativeInfinity();
    aabb.Enclose(obb);

    // Without the check main camera crashes bcs there is no quadtree
    Scene* scene = game_object->scene_owner;
    if (scene)
    {
        const Quadtree* quadtree = scene->GetQuadtree();
        quadtree->Remove(this);
        quadtree->Insert(this);
    }

}
