#pragma once

#include <MathGeoLib.h>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <typeinfo>

#include "utils/UUID.h"
#include "components/Component.h"
#include "resources/Resource.h"

#if defined(HACHIKO_API)
// Do Nothing
#elif defined(_MSC_VER)
#define HACHIKO_API __declspec(dllexport)
#endif

namespace Hachiko
{
    class ComponentTransform;
    class ComponentCamera;
    class Program;
    class Scene;

    class HACHIKO_API GameObject final
    {
        friend class Component;
        friend class Scene;
        friend class ModuleRender;
        friend class ModuleSceneManager;

    public:
        GameObject(const char* name = "Unnamed", UID uid = UUID::GenerateUID());
        GameObject(GameObject* parent, const float4x4& transform, const char* name = "Unnamed", UID uid = UUID::GenerateUID());
        GameObject(GameObject* parent,
                   const char* name = "Unnamed",
                   UID uid = UUID::GenerateUID(),
                   const float3& translation = float3::zero,
                   const Quat& rotation = Quat::identity,
                   const float3& scale = float3::one);

    private:
        ~GameObject();

    public:
        void SetNewParent(GameObject* new_parent);

        void AddComponent(Component* component);
        bool AttemptRemoveComponent(const Component* component);
        /// <summary>
        /// Do not use this unless it's mandatory. Use AttemptRemoveComponent
        /// instead.
        /// </summary>
        /// <param name="component">Component to be removed.</param>
        void ForceRemoveComponent(Component* component);

        Component* CreateComponent(Component::Type type);
        void RemoveChild(GameObject* gameObject);

        /// <summary>
        /// Creates a new GameObject as child of the root of current Scene.
        /// </summary>
        /// <returns>Created GameObject.</returns>
        static GameObject* Instantiate();
        /// <summary>
        /// Creates a new GameObject as child of this GameObject.
        /// </summary>
        /// <returns>Created GameObject.</returns>
        GameObject* CreateChild();

        void Start();
        void Stop();
        void Update();
        void DrawAll(ComponentCamera* camera, Program* program) const;
        void Draw(ComponentCamera* camera, Program* program) const;
        void DrawStencil(ComponentCamera* camera, Program* program);

        void SetActive(bool set_active);
        void SetActiveNonRecursive(bool set_active);

        [[nodiscard]] GameObject* Find(UID id) const;

        void OnTransformUpdated();

        void DebugDrawAll();
        void DebugDraw() const;
        void DrawBones() const;

        void Save(YAML::Node& node, bool as_prefab = false) const;
        void CollectObjectsAndComponents(std::vector<const GameObject*>& object_collector, std::vector<const Component*>& component_collector);
        void Load(const YAML::Node& node, bool as_prefab = false, bool meshes_only = false);
        static void CollectResources(const YAML::Node& node, std::map<Resource::Type, std::set<UID>>& resources);

        void SavePrefabReferences(YAML::Node& node, std::vector<const GameObject*>& object_collection, std::vector<const Component*>& component_collection) const;
        void LoadPrefabReferences(std::vector<const GameObject*>& object_collection, std::vector<const Component*>& component_collection);

        void SetTimeScaleMode(TimeScaleMode time_scale_mode) const;


        [[nodiscard]] const std::vector<Component*>& GetComponents() const
        {
            return components;
        }

        [[nodiscard]] ComponentTransform* GetTransform() const
        {
            return transform;
        }

        [[nodiscard]] const std::string& GetName() const
        {
            return name;
        }

        void SetName(const std::string& new_name)
        {
            name = new_name;
        }

        [[nodiscard]] UID GetID() const
        {
            return uid;
        }

        void SetID(const UID new_id)
        {
            uid = new_id;
        }

        [[nodiscard]] bool IsActive() const
        {
            return active;
        }

        template<typename RetComponent>
        RetComponent* GetComponent()
        {
            for (Component* component : components)
            {
                if (typeid(*component) == typeid(RetComponent))
                {
                    return static_cast<RetComponent*>(component);
                }
            }

            return nullptr;
        }

        template<typename RetComponent>
        [[nodiscard]] std::vector<RetComponent*> GetComponents() const
        {
            std::vector<RetComponent*> components_of_type;

            components_of_type.reserve(components.size());

            for (Component* component : components)
            {
                if (typeid(*component) == typeid(RetComponent))
                {
                    components_of_type.push_back(static_cast<RetComponent*>(component));
                }
            }

            return components_of_type;
        }

        template<typename RetComponent>
        std::vector<RetComponent*> GetComponentsInDescendants() const
        {
            std::vector<RetComponent*> components_in_descendants;

            for (GameObject* child : children)
            {
                std::vector<RetComponent*> components_in_child = child->GetComponents<RetComponent>();

                for (RetComponent* component_in_child : components_in_child)
                {
                    components_in_descendants.push_back(component_in_child);
                }

                std::vector<RetComponent*> components_in_childs_descendants = child->GetComponentsInDescendants<RetComponent>();

                for (RetComponent* component_in_childs_descendants : components_in_childs_descendants)
                {
                    components_in_descendants.push_back(component_in_childs_descendants);
                }
            }

            return components_in_descendants;
        }

        template<typename RetComponent>
        RetComponent* GetComponentInDescendants() const
        {
            for (GameObject* child : children)
            {
                RetComponent* component = child->GetComponent<RetComponent>();

                if (component != nullptr)
                {
                    return component;
                }

                component = child->GetComponentInDescendants<RetComponent>();

                if (component != nullptr)
                {
                    return component;
                }
            }

            return nullptr;
        }

        [[nodiscard]] Component* GetComponent(Component::Type type) const;
        [[nodiscard]] std::vector<Component*> GetComponents(Component::Type type) const;
        [[nodiscard]] std::vector<Component*> GetComponentsInDescendants(Component::Type type) const;

        [[nodiscard]] GameObject* GetFirstChildWithName(const std::string& child_name) const;
        [[nodiscard]] GameObject* FindDescendantWithName(const std::string& child_name) const;

        void ChangeEmissiveColor(float4 color, bool include_children = false, bool override_flag = false);
        std::vector<float4> GetEmissiveColors() const;
        void ResetEmissive(bool include_children = false);
        void ChangeTintColor(float4 color, bool include_children = false);
        void ChangeDissolveProgress(float progress, bool include_children = false);
        void SetVisible(bool v, bool include_children = false);

        const OBB* GetFirstMeshRendererOBB() const;

        void SimplifyChildren();
    private:
        void Simplify(GameObject* target);

    public:
        std::string name;
        Scene* scene_owner = nullptr;
        GameObject* parent = nullptr;
        std::vector<GameObject*> children{};
        bool active = true;

    private:
        bool started = false;
        std::vector<Component*> components{};
        ComponentTransform* transform = nullptr;
        UID uid = 0;
    };
} // namespace Hachiko
