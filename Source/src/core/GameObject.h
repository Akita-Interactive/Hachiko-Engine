#pragma once

#include <MathGeoLib.h>
#include <vector>
#include <typeinfo>

#include "utils/UUID.h"
#include "components/Component.h"

namespace Hachiko
{
class JsonFormatterValue;
class ComponentTransform;
class ComponentCamera;
class Program;
class Scene;

class GameObject final
{
    friend class Component;

public:
    GameObject(const char* name = "Unnamed");
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

    Component* CreateComponent(Component::Type type);
    /// <summary>
    /// Do not use this unless it's mandatory. Use AttemptRemoveComponent 
    /// instead.
    /// </summary>
    /// <param name="component">Component to be removed.</param>
    void ForceRemoveComponent(Component* component);
    void RemoveChild(GameObject* gameObject);

    //void Destroy();
    void Start();
    void Stop();
    void Update();
    void DrawAll(ComponentCamera* camera, Program* program) const;
    void Draw(ComponentCamera* camera, Program* program) const;
    void DrawStencil(ComponentCamera* camera, Program* program);

    void SetActive(bool set_active);
    
    
    bool IsActive() const
    {
        return active;
    }

    void OnTransformUpdated();

    void DebugDrawAll();
    void DebugDraw() const;
    void DrawBoundingBox() const;
    void UpdateBoundingBoxes();

    UID getUID() const
    {
        return uid;
    }

    void Save(JsonFormatterValue j_gameObject) const;
    void Load(JsonFormatterValue j_gameObject);

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
                components_of_type.
                    push_back(static_cast<RetComponent*>(component));
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
            std::vector<RetComponent*> components_in_child = 
                child->GetComponents<RetComponent>();

            for (RetComponent* component_in_child : components_in_child)
            {
                components_in_descendants.push_back(component_in_child);
            }

            std::vector<RetComponent*> components_in_childs_descendants = 
                child->GetComponentsInDescendants<RetComponent>();

            for (RetComponent* component_in_childs_descendants : 
                components_in_childs_descendants)
            {
                components_in_descendants.
                    push_back(component_in_childs_descendants);
            }
        }

        return components_in_descendants;
    }

    std::vector<Component*> GetComponents(Component::Type type) const;
    std::vector<Component*> GetComponentsInDescendants(
        Component::Type type) const;

public:
    std::string name;
    Scene* scene_owner = nullptr;
    GameObject* parent = nullptr;
    std::vector<GameObject*> children;

    bool active = true;

private:
    bool started = false;
    std::vector<Component*> components;
    ComponentTransform* transform = nullptr;

    AABB aabb;
    OBB obb;

    UID uid = 0;
};
}
        
