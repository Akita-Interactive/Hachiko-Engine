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

#include "utils/ComponentUtility.h"

Hachiko::ComponentMeshRenderer::ComponentMeshRenderer(GameObject* container) :
    Component(Type::MESH_RENDERER, container)
{
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
    if (!mesh || ! material)
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

    if (!visible || !mesh)
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

    App->program->UpdateMaterial(this, program);
    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::INDICES)], GL_UNSIGNED_INT, nullptr);
}

void Hachiko::ComponentMeshRenderer::DrawStencil(ComponentCamera* camera, Program* program) const
{
    if (!mesh)
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

void Hachiko::ComponentMeshRenderer::SetMeshResource(ResourceMesh* res)
{
    // Component mesh renderer needs a mesh
    assert(res);

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

void Hachiko::ComponentMeshRenderer::SetMaterialResource(ResourceMaterial* res)
{
    // Component mesh renderer needs a material
    assert(res);
    App->resources->ReleaseResource(material);
    material = res;
}

void Hachiko::ComponentMeshRenderer::LoadMesh(UID mesh_id)
{
    App->resources->ReleaseResource(mesh);
    SetMeshResource(static_cast<ResourceMesh*>(App->resources->GetResource(Resource::Type::MESH, mesh_id)));
}

void Hachiko::ComponentMeshRenderer::LoadMaterial(UID material_id)
{
    App->resources->ReleaseResource(material);
    SetMaterialResource(static_cast<ResourceMaterial*>(App->resources->GetResource(Resource::Type::MATERIAL, material_id)));
}

void Hachiko::ComponentMeshRenderer::SetCastingShadow(const bool value)
{
    is_casting_shadow = value;
}

void Hachiko::ComponentMeshRenderer::DrawGui()
{
    static const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
    ImGui::PushID(this);
    if (ImGuiUtils::CollapsingHeader(this, "Mesh renderer"))
    {
        if (ImGui::TreeNodeEx(&mesh, flags, "Mesh"))
        {
            if (mesh != nullptr)
            {
                ImGui::TextWrapped("%d Triangles\n%d vertices\n%d indices\n%d bones binded",
                            mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::INDICES)] / 3,
                            mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::VERTICES)],
                            mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::INDICES)],
                            mesh->buffer_sizes[static_cast<int>(ResourceMesh::Buffers::BONES)]);
                Widgets::Checkbox("Visible", &visible);
                Widgets::Checkbox("Navigable", &navigable);
                Widgets::Checkbox("Casts shadows", &is_casting_shadow);

                int current_index = Outline::TypeToInt(outline_type);
                const bool outline_type_changed = Widgets::Combo(
                    "Outline type", 
                    &current_index, 
                    Outline::all_names, 
                    Outline::GetAllTypeCount());

                if (outline_type_changed)
                {
                    outline_type = Outline::IntToType(current_index);
                }
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNodeEx((void*)&material, flags, "Material"))
        {
            Widgets::DragFloatConfig cfg;
            cfg.speed = 0.01f;
            cfg.min = 0.0f;
            cfg.max = 1.0f;
            
            Widgets::ColorEdit4("Tint color", tint_color);
            Widgets::DragFloat("Dissolve progress", dissolve_progress, &cfg);

            ChangeMaterial();
            if (material != nullptr)
            {
                material->DrawGui();
            }
            else
            {
                ImGui::TextWrapped("No material resource");
            }
            ImGui::TreePop();
        }
    }
    ImGui::PopID();
}

void Hachiko::ComponentMeshRenderer::Save(YAML::Node& node) const
{
    node.SetTag("mesh_renderer");
    if (mesh != nullptr)
    {
        node[RENDERER_MESH_ID] = mesh->GetID();
        node[MESH_NAVIGABLE] = navigable;
        node[MESH_VISIBLE] = visible;
        node[MESH_CASTING_SHADOW] = is_casting_shadow;
        node[MESH_OUTLINE_TYPE] = Outline::TypeToInt(outline_type);
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
    node[RENDERER_TINT_COLOR] = tint_color;
}

void Hachiko::ComponentMeshRenderer::Load(const YAML::Node& node)
{
    UID mesh_id = node[RENDERER_MESH_ID].as<UID>();
    UID material_id = node[RENDERER_MATERIAL_ID].as<UID>();
    if (mesh_id)
    {
        navigable = node[MESH_NAVIGABLE].IsDefined()
            ? node[MESH_NAVIGABLE].as<bool>()
            : false;
        visible = node[MESH_VISIBLE].IsDefined()
            ? node[MESH_VISIBLE].as<bool>()
            : true;
        is_casting_shadow = node[MESH_CASTING_SHADOW].IsDefined()
            ? node[MESH_CASTING_SHADOW].as<bool>()
            : true;

        outline_type = node[MESH_OUTLINE_TYPE].IsDefined()
            ? Outline::IntToType(node[MESH_OUTLINE_TYPE].as<int>())
            : Outline::Type::NONE;

        LoadMesh(mesh_id);
    }
    if (material_id)
    {
        LoadMaterial(material_id);
    }
    tint_color = node[RENDERER_TINT_COLOR].IsDefined() ? node[RENDERER_TINT_COLOR].as<float4>() : float4::one;
}

void Hachiko::ComponentMeshRenderer::CollectResources(const YAML::Node& node, std::map<Resource::Type, std::set<UID>>& resources)
{
    ComponentUtility::CollectResource(
        Resource::Type::MESH, 
        node[RENDERER_MESH_ID], 
        resources);

    ComponentUtility::CollectResource(
        Resource::Type::MATERIAL,
        node[RENDERER_MATERIAL_ID],
        resources);
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
    const std::string title = StringUtils::Concat("Select material#", std::to_string(uid));

    if (ImGui::Button("Select material", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
    {
        ImGuiFileDialog::Instance()->OpenDialog(title,
                                                "Select material",
                                                MATERIAL_EXTENSION,
                                                ASSETS_FOLDER_MATERIAL,
                                                1,
                                                nullptr,
                                                ImGuiFileDialogFlags_DontShowHiddenFiles | ImGuiFileDialogFlags_DisableCreateDirectoryButton | ImGuiFileDialogFlags_HideColumnType
                                                | ImGuiFileDialogFlags_HideColumnDate);
    }

    if (ImGuiFileDialog::Instance()->Display(title))
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

                // TODO: Do this only when the material is actually changed.
                App->scene_manager->RebuildBatches();
            }
        }

        ImGuiFileDialog::Instance()->Close();
    }
}

void Hachiko::ComponentMeshRenderer::OverrideEmissive(const float4& color, bool override_flag)
{
    override_emissive = color;
    override_material = true;
    override_emissive_flag_override = override_flag;
}
void Hachiko::ComponentMeshRenderer::LiftOverrideEmissive() {
    override_emissive = float4::zero;
    override_material = false;
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
        scene->GetQuadtree()->Reposition(this);
    }
}
