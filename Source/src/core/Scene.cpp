#include "core/hepch.h"
#include "Scene.h"

#include "components/ComponentTransform.h"
#include "components/ComponentCamera.h"
#include "components/ComponentMeshRenderer.h"
#include "components/ComponentImage.h"
#include "components/ComponentAnimation.h"

#include "modules/ModuleTexture.h"
#include "modules/ModuleEditor.h"
#include "modules/ModuleCamera.h"
#include "modules/ModuleDebugDraw.h"
#include "modules/ModuleResources.h"
#include "modules/ModuleNavigation.h"

#include "resources/ResourceMaterial.h"
#include "resources/ResourceMaterial.h"
#include "resources/ResourceAnimation.h"
#include <debugdraw.h>
#include <algorithm>

Hachiko::Scene::Scene()
    : root(new GameObject(nullptr, float4x4::identity, "Root"))
    , culling_camera(App->camera->GetRenderingCamera())
    , skybox(new Skybox())
    , quadtree(new Quadtree())
    , loaded(false)
    , name(UNNAMED_SCENE)
{
    // Root's scene_owner should always be this scene:
    root->scene_owner = this;

    // TODO: Send hardcoded values to preferences
    quadtree->SetBox(AABB(float3(-500, -100, -500), float3(500, 250, 500)));
    App->navigation->BuildNavmesh(this);
}

Hachiko::Scene::~Scene()
{
    CleanScene();
}

void Hachiko::Scene::CleanScene()
{
    App->editor->SetSelectedGO(nullptr);
    delete root;
    delete skybox;
    delete quadtree;
    loaded = false;
}

void Hachiko::Scene::DestroyGameObject(GameObject* game_object) const
{
    if (App->editor->GetSelectedGameObject() == game_object)
    {
        App->editor->SetSelectedGO(nullptr);
    }

    quadtree->Remove(game_object);
}

Hachiko::ComponentCamera* Hachiko::Scene::GetMainCamera() const
{
    // This will return the first camera it comes across in the hierarchy in a 
    // depth-first manner:

    // TODO: This will be costly for any method that will call this on
    // a method/function that is called each frame. Make this getter a cached 
    // component camera that is only updated when a component camera is added/
    // removed to the scene. Or have ComponentCamera have a property called 
    // is_main, that will be set by making all other component cameras in scene
    // is_main = false.

    return root->GetComponentInDescendants<ComponentCamera>();
}

Hachiko::GameObject* Hachiko::Scene::CreateNewGameObject(GameObject* parent, const char* name)
{
    GameObject* final_parent = parent != nullptr ? parent : root;
    GameObject* new_game_object = final_parent->CreateChild();

    new_game_object->SetName(name);

    // This will insert itself into quadtree on first bounding box update:
    return new_game_object;
}

void Hachiko::Scene::HandleInputMaterial(ResourceMaterial* material)
{
    // TODO: Change the location of this, it has no reason to be here
    GameObject* game_object = App->editor->GetSelectedGameObject();
    if (game_object == nullptr)
    {
        HE_LOG("No game object selected to apply a material on");
        return;
    }

    ComponentMeshRenderer* component_mesh_renderer = game_object->GetComponent<ComponentMeshRenderer>();
    if (component_mesh_renderer != nullptr)
    {
        component_mesh_renderer->AddResourceMaterial(material);
        return;
    }
    // TODO: If the material is not used decrease reference count
}

Hachiko::GameObject* Hachiko::Scene::Raycast(const float3& origin, const float3& destination) const
{
    LineSegment line_seg(origin, destination);
    return Raycast(line_seg);
}

Hachiko::GameObject* Hachiko::Scene::Find(UID id) const
{
    return root->Find(id);
}

Hachiko::GameObject* Hachiko::Scene::Raycast(const LineSegment& segment) const
{
    GameObject* selected = nullptr;
    float closest_hit_distance = inf;

    std::vector<GameObject*> game_objects;
    quadtree->GetRoot()->GetIntersections(game_objects, segment);

    for (GameObject* game_object : game_objects)
    {
        auto* mesh_renderer = game_object->GetComponent<ComponentMeshRenderer>();
        if (mesh_renderer)
        {
            // Transform ray to mesh space, more efficient
            LineSegment local_segment(segment);
            local_segment.Transform(game_object->GetTransform()->GetGlobalMatrix().Inverted());

            const float* vertices = mesh_renderer->GetVertices();
            const unsigned* indices = mesh_renderer->GetIndices();
            for (unsigned i = 0; i < mesh_renderer->GetBufferSize(ResourceMesh::Buffers::INDICES); i += 3)
            {
                Triangle triangle;
                triangle.a = vec(&vertices[indices[i] * 3]);
                triangle.b = vec(&vertices[indices[i + 1] * 3]);
                triangle.c = vec(&vertices[indices[i + 2] * 3]);

                float hit_distance;
                float3 hit_point;
                if (local_segment.Intersects(triangle, &hit_distance, &hit_point))
                {
                    if (hit_distance < closest_hit_distance)
                    {
                        closest_hit_distance = hit_distance;
                        selected = game_object;
                    }
                }
            }
        }
    }
    return selected;
}

void Hachiko::Scene::Save(YAML::Node& node) const
{
    node[SCENE_NAME] = GetName();
    node[ROOT_ID] = GetRoot()->GetID();
    for (int i = 0; i < GetRoot()->children.size(); ++i)
    {
        GetRoot()->children[i]->Save(node[CHILD_NODE][i]);
    }
}

void Hachiko::Scene::Load(const YAML::Node& node)
{
    if (!node[CHILD_NODE].IsDefined())
    {
        // Loaded as an empty scene:
        loaded = true;

        return;
    }

    SetName(node[SCENE_NAME].as<std::string>().c_str());
    root->SetID(node[ROOT_ID].as<UID>());
    const YAML::Node children_node = node[CHILD_NODE];

    for (unsigned i = 0; i < children_node.size(); ++i)
    {
        std::string child_name = children_node[i][GAME_OBJECT_NAME].as<std::string>();
        UID child_uid = children_node[i][GAME_OBJECT_ID].as<UID>();
        const auto child = new GameObject(root, child_name.c_str(), child_uid);
        child->scene_owner = this;
        child->Load(children_node[i]);
    }

    App->navigation->BuildNavmesh(this);

    loaded = true;
}

void Hachiko::Scene::GetNavmeshData(std::vector<float>& scene_vertices, std::vector<int>& scene_triangles, std::vector<float>& scene_normals, AABB& scene_bounds)
{
    // Ensure that all scene is fresh (bounding boxes were not updated if using right after loading scene)
    root->Update();
    // TODO: Have an array of meshes on scene to not make this recursive ?
    scene_vertices.clear();
    scene_triangles.clear();
    scene_normals.clear();
    scene_bounds.SetNegativeInfinity();

    std::function<void(GameObject*)> get_navmesh_data = [&](GameObject* go)
    {
        ComponentMeshRenderer* mesh_renderer = go->GetComponent<ComponentMeshRenderer>();        
        const float4x4& global_transform = go->GetTransform()->GetGlobalMatrix();
        // TODO: Add a distinction to filter out meshes that are not part of navigation (navigable flag or not static objects, also flag?)
        if (mesh_renderer && mesh_renderer->IsNavigable())
        {
            // Use previous amount of mesh vertices to point to the correct indices
            // Divide size/3 because its a vector of floats not of float3
            int indices_offset = static_cast<int>(scene_vertices.size()) / 3;
            
            const float* vertices = mesh_renderer->GetVertices();
            for (int i = 0; i < mesh_renderer->GetBufferSize(ResourceMesh::Buffers::VERTICES); i += 3)
            {
                
                float4 global_vertex = global_transform * float4(vertices[i], vertices[i + 1], vertices[i + 2], 1.0f);
                // w is excluded, so we pass float 3 as desired
                scene_vertices.insert(scene_vertices.end(), &global_vertex.x, &global_vertex.w);
            }
            
            const unsigned* indices = mesh_renderer->GetIndices();
            int n_indices = mesh_renderer->GetBufferSize(ResourceMesh::Buffers::INDICES);
            auto inserted = scene_triangles.insert(scene_triangles.end(), indices, indices + n_indices);
            std::for_each(inserted, scene_triangles.end(), [&](int& v) { v += indices_offset; });

            const float* normals = mesh_renderer->GetNormals();
            for (int i = 0; i < mesh_renderer->GetBufferSize(ResourceMesh::Buffers::NORMALS); i += 3)
            {
                float4 global_normal = global_transform * float4(normals[i], normals[i + 1], normals[i + 2], 1.0f);
                // w is excluded, so we pass float 3 as desired
                scene_normals.insert(scene_normals.end(), &global_normal.x, &global_normal.w);
            }

            scene_bounds.Enclose(go->GetAABB());
        }

        for (auto& child : go->children)
        {
            get_navmesh_data(child);
        }
    };

    get_navmesh_data(root);
}

Hachiko::GameObject* Hachiko::Scene::CreateDebugCamera()
{
    GameObject* camera = CreateNewGameObject(root, "Debug Camera");
    camera->GetTransform()->SetLocalPosition(float3(5, 5, 0));
    camera->CreateComponent(Component::Type::CAMERA);
    camera->GetTransform()->LookAtTarget(float3(0, 5, 0));

    auto* debug_camera = camera->GetComponent<ComponentCamera>();
    debug_camera->SetFarPlane(100.0f);
    debug_camera->draw_frustum = true;

    return camera;
}

void Hachiko::Scene::Start() const
{
    root->Start();
}

void Hachiko::Scene::Update() const
{
    OPTICK_CATEGORY("UpdateScene", Optick::Category::Scene);
    root->Update();
}