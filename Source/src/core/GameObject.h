#pragma once

#include <MathGeoLib.h>
#include <vector>
#include <string>
#include <typeinfo>

#include "utils/UUID.h"
#include "components/Component.h"

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

    public:
        GameObject(const char* name = "Unnamed", UID uid = UUID::GenerateUID());
        GameObject(GameObject* parent, const float4x4& transform, const char* name = "Unnamed", UID uid = UUID::GenerateUID());
        GameObject(GameObject* parent,
                   const char* name = "Unnamed",
                   UID uid = UUID::GenerateUID(),
                   const float3& translation = float3::zero,
                   const Quat& rotation = Quat::identity,
                   const float3& scale = float3::one);
        virtual ~GameObject();

        void SetNewParent(GameObject* new_parent);

        void AddComponent(Component* component);
        bool AttemptRemoveComponent(Component* component);
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
        void Update();
        void DrawAll(ComponentCamera* camera, Program* program) const;
        void Draw(ComponentCamera* camera, Program* program) const;
        void DrawStencil(ComponentCamera* camera, Program* program);

        void SetActive(bool set_active);

        [[nodiscard]] bool IsActive() const
        {
            return active;
        }

        [[nodiscard]] GameObject* Find(UID id) const;

        void OnTransformUpdated();

        void DebugDrawAll();
        void DebugDraw() const;
        void DrawBoundingBox() const;
        void DrawBones() const;
        void UpdateBoundingBoxes();

        [[nodiscard]] UID GetID() const
        {
            return uid;
        }

        void SetID(const UID new_id)
        {
            uid = new_id;
        }

        void Save(YAML::Node& node, bool as_prefab = false) const;
        void Load(const YAML::Node& node, bool as_prefab = false);

        [[nodiscard]] const OBB& GetOBB() const
        {
            return obb;
        }

        const AABB& GetAABB()
        {
            return aabb;
        }

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

        GameObject* Find(UID id) const;

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
        std::vector<RetComponent*> GetComponents() const
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

        [[nodiscard]] std::vector<Component*> GetComponents(Component::Type type) const;
        [[nodiscard]] std::vector<Component*> GetComponentsInDescendants(Component::Type type) const;

        GameObject* GetFirstChildWithName(const std::string& child_name) const;
        Hachiko::GameObject* FindDescendantWithName(const std::string& child_name) const;

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
        //bool in_quadtree = false;
        AABB aabb;
        OBB obb;
        UID uid = 0;
    };
} // namespace Hachiko
