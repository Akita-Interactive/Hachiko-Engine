#pragma once

#include "utils/UUID.h"

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
    class ResourceNavMesh;

    class Scene
    {
        friend class ModuleSceneManager;

    public:
        Scene();
        ~Scene() override;

        void CleanScene();

        // --- Life cycle Scene --- //
        void Start() const;
        void Update() const;

        // --- GameObject Management --- //
        ComponentCamera* GetMainCamera() const;
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

        [[nodiscard]] GameObject* Raycast(const float3& origin, const float3& destination) const;

        GameObject* Find(UID id) const;

        void SetName(const char* new_name)
        {
            name = new_name;
        }

        void Save(YAML::Node& node) const;
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

        Skybox* skybox = nullptr;
        Quadtree* quadtree = nullptr;

    public:
        class Memento
        {
        public:
            Memento(std::string content) :
                // content(std::move(content))
            content(content)
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

        [[nodiscard]] Memento* CreateSnapshot() const;
        void Restore(const Memento*) const;
    };
}
