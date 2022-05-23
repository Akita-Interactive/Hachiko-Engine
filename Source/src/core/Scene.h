#pragma once

#include "utils/UUID.h"
#include <utility>

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
    class ResourceMaterial;

    class Scene
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
        [[nodiscard]] ComponentCamera* GetMainCamera() const;
        void DestroyGameObject(GameObject* game_object) const;
        GameObject* CreateNewGameObject(GameObject* parent = nullptr, const char* name = nullptr);

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

        [[nodiscard]] bool IsLoaded() const
        {
            return loaded;
        }

        [[nodiscard]] const char* GetName() const
        {
            return name.c_str();
        }

        [[nodiscard]] UID GetNavmeshID() const
        {
            return navmesh_id;
        }

        void SetNavmeshID(UID new_navmesh_id)
        {
            navmesh_id = new_navmesh_id;
        }
        
        [[nodiscard]] GameObject* Raycast(const float3& origin, const float3& destination) const;

        [[nodiscard]] GameObject* Find(UID id) const;

        void SetName(const char* new_name)
        {
            name = new_name;
        }

        void Save(YAML::Node& node);
        void Load(const YAML::Node& node);

        void GetNavmeshData(std::vector<float>& scene_vertices, std::vector<int>& scene_triangles, std::vector<float>& scene_normals, AABB& scene_bounds);

        std::vector<ComponentDirLight*> dir_lights{};
        std::vector<ComponentPointLight*> point_lights{};
        std::vector<ComponentSpotLight*> spot_lights{};

        

    private:
        std::string name;
        GameObject* root = nullptr;
        ComponentCamera* culling_camera = nullptr;
        bool loaded = false;

        UID navmesh_id = 0;

        Skybox* skybox = nullptr;
        Quadtree* quadtree = nullptr;

    public:
        class Memento
        {
        public:
            Memento(std::string content) :
                // content(std::move(content))
            content(std::move(content))
            {
            }

            ~Memento() = default;

            [[nodiscard]] std::string GetContent() const
            {
                return content;
            }
            
        private:
            std::string content;
        };

        [[nodiscard]] Memento* CreateSnapshot();
        void Restore(const Memento*) const;
    };
}
