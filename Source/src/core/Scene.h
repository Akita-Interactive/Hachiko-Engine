#pragma once

#include "core/serialization/ISerializable.h"

namespace Hachiko
{
    class ModuleSceneManager;
    class GameObject;
    class ComponentTransform;
    class ComponentCamera;
    class ComponentDirLight;
    class ComponentPointLight;
    class ComponentSpotLight;
    class Skybox;
    class Quadtree;
    class ResourceModel;
    class ResourceMaterial;

    class Scene : public ISerializable
    {
        friend class ModuleSceneManager;

    public:
        Scene();
        ~Scene();

        void CleanScene();

        // --- Life cycle Scene --- //
        void Start() const;
        void Update() const;

        // --- GameObject Management --- //
        ComponentCamera* GetMainCamera() const;
        void AddGameObject(GameObject* new_object, GameObject* parent = nullptr) const;
        void DestroyGameObject(GameObject* game_object) const;
        GameObject* CreateNewGameObject(GameObject* parent = nullptr, const char* name = nullptr);

        void HandleInputModel(ResourceModel* model);
        void HandleInputMaterial(ResourceMaterial* material);

        [[nodiscard]] GameObject* Raycast(const LineSegment& segment) const;
        [[nodiscard]] GameObject* GetRoot() const
        {
            return root;
        }

        [[nodiscard]] ComponentCamera* GetCullingCamera() const
        {
            return culling_camera;
        }

        void SetCullingCamera(ComponentCamera* v)
        {
            culling_camera = v;
        }

        // --- Quadtree --- //
        [[nodiscard]] Quadtree* GetQuadtree() const
        {
            return quadtree;
        }

        // --- Debug --- //
        GameObject* CreateDebugCamera();

        // --- Skybox --- //
        [[nodiscard]] Skybox* GetSkybox() const
        {
            return skybox;
        }

        bool IsLoaded() const 
        {
            return loaded;
        }

        [[nodiscard]] const char* GetName() const 
        {
            return name.c_str();
        }
        
        [[nodiscard]] GameObject* Raycast(const float3& origin, const float3& destination) const;

        GameObject* Find(UID id) const;

        void SetName(const char* new_name)
        {
            name = new_name;
        }

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

        void GetNavmeshData(std::vector<float>& scene_vertices, std::vector<int>& scene_triangles, std::vector<float>& scene_normals, AABB& scene_bounds);
        
        std::vector<ComponentDirLight*> dir_lights;
        std::vector<ComponentPointLight*> point_lights;
        std::vector<ComponentSpotLight*> spot_lights;

    private:
        bool draw_all_bounding_boxes = false;
        std::string name;
        GameObject* root = nullptr;
        ComponentCamera* culling_camera = nullptr;
        bool loaded = false;

        Skybox* skybox;
        Quadtree* quadtree = nullptr;
    };
}
