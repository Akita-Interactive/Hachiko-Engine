#include "core/hepch.h"
#include "Scene.h"

#include "components/ComponentTransform.h"
#include "components/ComponentCamera.h"
#include "components/ComponentMesh.h"
#include "components/ComponentMaterial.h"
#include "components/ComponentImage.h"

#include "modules/ModuleTexture.h"
#include "modules/ModuleEditor.h"
#include "modules/ModuleCamera.h"
#include "modules/ModuleDebugDraw.h"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

Hachiko::Scene::Scene():
    root(new GameObject(nullptr, float4x4::identity, "Root")),
    culling_camera(App->camera->GetMainCamera()),
    skybox(new Skybox()),
    quadtree(new Quadtree())
{
    // TODO: Send hardcoded values to preferences
    quadtree->SetBox(AABB(float3(-100000000, -100000000, -100000000), float3(100000000, 100000000, 100000000)));
}

Hachiko::Scene::~Scene()
{
    CleanScene();
}

void Hachiko::Scene::CleanScene() const
{
    App->editor->SetSelectedGO(nullptr);
    delete root;
    delete skybox;
    delete quadtree;
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
    return SearchMainCamera(root);
}

Hachiko::ComponentCamera* Hachiko::Scene::SearchMainCamera(GameObject* game_object) const
{
    ComponentCamera* component_camera = nullptr;
    component_camera = game_object->GetComponent<ComponentCamera>();
    if (component_camera != nullptr)
    {
        return component_camera;
    }

    for (GameObject* child : game_object->children)
    {
        component_camera = SearchMainCamera(child);
        if (component_camera != nullptr)
        {
            return component_camera;
        }
    }
    return nullptr;
}

void Hachiko::Scene::AddGameObject(GameObject* new_object, GameObject* parent) const
{
    GameObject* new_parent = parent ? parent : root;
    new_parent->children.push_back(new_object);
    quadtree->Insert(new_object);
}

Hachiko::GameObject* Hachiko::Scene::CreateNewGameObject(const char* name, GameObject* parent)
{
    // It will insert itself into quadtree on first bounding box update
    const auto game_object = new GameObject(parent ? parent : root, name);
    game_object->scene_owner = this;
    return game_object;
}

Hachiko::GameObject* Hachiko::Scene::LoadFBX(const std::string& path)
{
    const auto model_path = path.substr(0, path.find_last_of("/\\") + 1);
    const auto file_name = path.substr(path.find_last_of("/\\") + 1);
    const auto name = file_name.substr(0, file_name.find_last_of('.'));

    GameObject* model = nullptr;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GlobalScale);
    if (scene)
    {
        //std::vector<ResourceMaterial*> materials = LoadMaterials(scene, model_path, name);
        model = CreateNewGameObject(name.c_str(), root);
        LoadNode(scene, scene->mRootNode, model, model_path);
        //materials.clear();
    }
    else
    {
        HE_LOG("Error loading file %s: %s", model_path.c_str(), aiGetErrorString());
    }
    importer.FreeScene();
    return model;
}

Hachiko::GameObject* Hachiko::Scene::LoadImageObject(const std::string& path)
{
    const auto file_name = path.substr(path.find_last_of("/\\") + 1);
    const auto name = file_name.substr(0, file_name.find_last_of('.'));

    GameObject* game_object = nullptr;
    game_object = CreateNewGameObject(name.c_str(), root);
    game_object->CreateComponent(Component::Type::TRANSFORM_2D);
    game_object->CreateComponent(Component::Type::CANVAS_RENDERER);
    game_object->CreateComponent(Component::Type::IMAGE);
    ComponentImage* image = game_object->GetComponent<ComponentImage>();
    image->Import(path.c_str());

    return game_object;
}

void Hachiko::Scene::Save(JsonFormatterValue j_scene) const
{
    // GameObjects
    const JsonFormatterValue j_root = j_scene["GORoot"];
    root->Save(j_root);

    //TODO: Save camera position
}

void Hachiko::Scene::Load(JsonFormatterValue j_scene)
{
    //CleanScene(); 
    delete root;

    // Load GameObjects
    const JsonFormatterValue j_root = j_scene["GORoot"];
    root = new GameObject(nullptr, "Root", j_root["Uid"]);
    root->scene_owner = this;
    root->Load(j_root);
}

Hachiko::GameObject* Hachiko::Scene::RayCast(const float3& origin, const float3& destination) const
{
    LineSegment line_seg(origin, destination);
    return RayCast(line_seg);
}

Hachiko::GameObject* Hachiko::Scene::RayCast(const LineSegment& segment) const
{
    GameObject* selected = nullptr;
    float closest_hit_distance = inf;

    std::vector<GameObject*> game_objects;
    quadtree->GetRoot()->GetIntersections(game_objects, segment);

    for (GameObject* game_object : game_objects)
    {
        auto* mesh = game_object->GetComponent<ComponentMesh>();
        if (mesh)
        {
            // Transform ray to mesh space, more efficient
            LineSegment local_segment(segment);
            local_segment.Transform(game_object->GetTransform()->GetGlobalMatrix().Inverted());

            const float* vertices = mesh->GetVertices();
            const unsigned* indices = mesh->GetIndices();
            for (unsigned i = 0; i < mesh->GetBufferSize(ResourceMesh::Buffers::INDICES); i += 3)
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

void Hachiko::Scene::CreateLights()
{
    GameObject* sun = CreateNewGameObject("Sun", root);
    sun->GetTransform()->SetLocalPosition(float3(1, 1, -1));
    sun->GetTransform()->LookAtTarget(float3(0, 0, 0));
    sun->CreateComponent(Component::Type::DIRLIGHT);

    GameObject* spot = CreateNewGameObject("Spot Light", root);
    spot->GetTransform()->SetLocalPosition(float3(-1, 1, -1));

    spot->CreateComponent(Component::Type::SPOTLIGHT);

    GameObject* point = CreateNewGameObject("Point Light", root);
    point->GetTransform()->SetLocalPosition(float3(0, 1, -1));
    point->CreateComponent(Component::Type::POINTLIGHT);
}

void Hachiko::Scene::LoadNode(const aiScene* scene, const aiNode* node, GameObject* parent, const std::string& model_path)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        GameObject* model_part = CreateNewGameObject(node->mName.C_Str(), parent);
        model_part->CreateComponent(Component::Type::MESH);
        model_part->GetComponent<ComponentMesh>()->Import(mesh);
        model_part->CreateComponent(Component::Type::MATERIAL);
        model_part->GetComponent<ComponentMaterial>()->Import(scene->mMaterials[mesh->mMaterialIndex], model_path, node->mName.C_Str());

        aiVector3D aiTranslation, aiScale;
        aiQuaternion aiRotation;
        node->mTransformation.Decompose(aiScale, aiRotation, aiTranslation);
        model_part->GetTransform()->SetLocalTransform(float3(aiTranslation.x, aiTranslation.y, aiTranslation.z),
                                                                          Quat(aiRotation.x, aiRotation.y, aiRotation.z, aiRotation.w),
                                                                          float3(aiScale.x, aiScale.y, aiScale.z));
    }

    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        LoadNode(scene, node->mChildren[i], parent, model_path);
    }
}

Hachiko::GameObject* Hachiko::Scene::CreateDebugCamera()
{
    GameObject* camera = CreateNewGameObject("Debug Camera", root);
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
    root->Update();
}
