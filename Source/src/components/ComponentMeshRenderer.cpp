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

Hachiko::ComponentMeshRenderer::ComponentMeshRenderer(GameObject* container, UID id, ResourceMesh* res) 
    : Component(Type::MESH_RENDERER, container)
{
    if (res != nullptr)
    {
        AddResourceMesh(res);
    }
}

Hachiko::ComponentMeshRenderer::~ComponentMeshRenderer() 
{
    delete[] node_cache;
}

void Hachiko::ComponentMeshRenderer::Update() {
    if (mesh == nullptr)
    {
        return;

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

void Hachiko::ComponentMeshRenderer::LoadMesh(UID mesh_id)
{
    AddResourceMesh(static_cast<ResourceMesh*> (App->resources->GetResource(Resource::Type::MESH, mesh_id)));
}

void Hachiko::ComponentMeshRenderer::LoadMaterial(UID material_id)
{
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
                ImGui::Checkbox("Visible", &visible);
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
    }
    else
    {
        node[RENDERER_MESH_ID] = 0;
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
    SetID(node[COMPONENT_ID].as<UID>());

    UID mesh_id = node[RENDERER_MESH_ID].as<UID>();
    UID material_id = node[RENDERER_MATERIAL_ID].as<UID>();
    if (mesh_id)
    {
        LoadMesh(mesh_id);
    }
    if (material_id)
    {
        LoadMaterial(material_id);
    }
}

Hachiko::GameObject* GetRoot(Hachiko::GameObject* posible_root) 
{
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
            return;

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
                                                ".mat",
                                                "./assets/materials/",
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

            ResourceMaterial* res = static_cast<ResourceMaterial*>(App->resources->GetResource(Resource::Type::MATERIAL, material_node[GENERAL_NODE][GENERAL_ID].as<UID>()));
            if (res != nullptr)
            {
                // Unload material
                material = res;
            }
        }

        ImGuiFileDialog::Instance()->Close();
    }
}
