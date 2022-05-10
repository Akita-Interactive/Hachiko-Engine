#include "core/hepch.h"

#include "components/ComponentTransform.h"
#include "components/ComponentCamera.h"
#include "components/ComponentMeshRenderer.h"
#include "components/ComponentDirLight.h"
#include "components/ComponentPointLight.h"
#include "components/ComponentSpotLight.h"
#include "components/ComponentAnimation.h"
#include "components/ComponentAgent.h"
#include "components/ComponentObstacle.h"


// UI
#include "components/ComponentCanvas.h"
#include "components/ComponentCanvasRenderer.h"
#include "components/ComponentTransform2D.h"
#include "components/ComponentImage.h"
#include "components/ComponentButton.h"
#include "Components/ComponentProgressBar.h"
#include "components/ComponentText.h"

#include "Application.h"
#include "modules/ModuleSceneManager.h"
#include "modules/ModuleScriptingSystem.h" // For instantiating Scripts.


#include <debugdraw.h>

Hachiko::GameObject::GameObject(const char* name, UID uid) :
    name(name),
    uid(uid)
{
    AddComponent(new ComponentTransform(this, float3::zero, Quat::identity, float3::one));
}

Hachiko::GameObject::GameObject(GameObject* parent, const float4x4& transform, const char* name, UID uid) :
    name(name),
    uid(uid)
{
    AddComponent(new ComponentTransform(this, transform));
    SetNewParent(parent);
}

Hachiko::GameObject::GameObject(GameObject* parent, const char* name, UID uid, const float3& translation, const Quat& rotation, const float3& scale) :
    name(name),
    uid(uid)
{
    AddComponent(new ComponentTransform(this, translation, rotation, scale));
    SetNewParent(parent);
}

Hachiko::GameObject::~GameObject()
{
    if (parent)
    {
        parent->RemoveChild(this);
    }

    if (scene_owner)
    {
        scene_owner->DestroyGameObject(this);
    }

    for (GameObject* child : children)
    {
        child->parent = nullptr;
        RELEASE(child);
    }
    for (Component* component : components)
    {
        RELEASE(component);
    }
    scene_owner = nullptr;
}

void Hachiko::GameObject::RemoveChild(GameObject* game_object)
{
    assert(game_object != nullptr);
    children.erase(std::remove(children.begin(), children.end(), game_object), children.end());
}

Hachiko::GameObject* Hachiko::GameObject::CreateChild()
{
    GameObject* new_child = new GameObject(this);
    // Ensure that child's scene_owner is same with this GameObject's 
    // scene_owner:
    new_child->scene_owner = scene_owner;

    return new_child;
}

Hachiko::GameObject* Hachiko::GameObject::Instantiate()
{
    return App->scene_manager->GetActiveScene()->GetRoot()->CreateChild();
}

void Hachiko::GameObject::SetNewParent(GameObject* new_parent)
{
    if (new_parent == parent)
    {
        return;
    }

    if (parent)
    {
        parent->RemoveChild(this);
    }

    parent = new_parent;

    if (new_parent)
    {
        float4x4 temp_matrix = this->GetTransform()->GetGlobalMatrix();
        new_parent->children.push_back(this);
        this->GetTransform()->SetGlobalTransform(temp_matrix);
    }
}

void Hachiko::GameObject::AddComponent(Component* component)
{
    switch (component->GetType())
    {
    case (Component::Type::TRANSFORM):
    {
        components.push_back(component);
        transform = static_cast<ComponentTransform*>(component);
        component->SetGameObject(this);
        break;
    }
    default:
    {
        components.push_back(component);
        component->SetGameObject(this);
        break;
    }
    }
}

Hachiko::Component* Hachiko::GameObject::CreateComponent(Component::Type type)
{
    Component* new_component = nullptr;
    switch (type)
    {
    case (Component::Type::TRANSFORM):
        return transform;
        break;
    case (Component::Type::CAMERA):
        new_component = new ComponentCamera(this);
        break;
    case (Component::Type::ANIMATION):
        new_component = new ComponentAnimation(this);
        break;
    case (Component::Type::MESH_RENDERER):
        new_component = new ComponentMeshRenderer(this);
        break;
    case (Component::Type::DIRLIGHT):
        new_component = new ComponentDirLight(this);
        break;
    case (Component::Type::POINTLIGHT):
        new_component = new ComponentPointLight(this);
        break;
    case (Component::Type::SPOTLIGHT):
        new_component = new ComponentSpotLight(this);
        break;
    case (Component::Type::CANVAS):
        if (!GetComponent<ComponentCanvas>())
        {
            new_component = new ComponentCanvas(this);
        }
        break;
    case (Component::Type::CANVAS_RENDERER):
        if (!GetComponent<ComponentCanvasRenderer>())
        {
            new_component = new ComponentCanvasRenderer(this);
        }
        break;
    case (Component::Type::TRANSFORM_2D):
        if (!GetComponent<ComponentTransform2D>())
        {
            new_component = new ComponentTransform2D(this);
        }
        break;
    case (Component::Type::IMAGE):
        if (!GetComponent<ComponentImage>())
        {
            new_component = new ComponentImage(this);
        }
        break;
    case (Component::Type::BUTTON):
        if (!GetComponent<ComponentButton>())
        {
            new_component = new ComponentButton(this);
        }
        break;
    case (Component::Type::PROGRESS_BAR):
        if (!GetComponent<ComponentProgressBar>())
        {
            new_component = new ComponentProgressBar(this);
        }
        break;
    case (Component::Type::TEXT):
        if (!GetComponent<ComponentProgressBar>())
        {
            new_component = new ComponentText(this);
        }
        break;
    case (Component::Type::OBSTACLE):
        if (!GetComponent<ComponentObstacle>())
            new_component = new ComponentObstacle(this);
        break;
    case (Component::Type::AGENT):
        if (!GetComponent<ComponentAgent>())
            new_component = new ComponentAgent(this);
        break;
    }

    if (new_component != nullptr)
    {
        components.push_back(new_component);
    }
    else
    {
        HE_LOG("Falied to create component");
    }
    return new_component;
}

void Hachiko::GameObject::SetActive(bool set_active)
{
    if (!active && set_active)
    {
        Start();
    }
    active = set_active;
}

void Hachiko::GameObject::Start()
{
    if (!started)
    {
        transform->Start();
        for (Component* component : components)
        {
            component->Start();
        }

        for (GameObject* child : children)
        {
            if (child->IsActive())
            {
                child->Start();
            }
        }
        started = true;
    }
}

void Hachiko::GameObject::Update()
{
    if (transform && transform->HasChanged())
    {
        OnTransformUpdated();
    }

    // NOTE: It is weird that a non-sense nullptr exception we were facing is 
    // solved by converting the for loop for children vector to use the follow
    // ing for loop instead of range based and iterator based ones. Thanks to 
    // Vicenc for coming up with this approach. Maybe we should convert all 
    // vector loops to be like the ones following.
    // TODO: Ask this to the teachers.

    for (int i = 0; i < components.size(); ++i)
    {
        components[i]->Update();
    }

    for (int i = 0; i < children.size(); ++i)
    {
        if (children[i]->IsActive())
        {
            children[i]->Update();
        }
    }
}

void Hachiko::GameObject::DrawAll(ComponentCamera* camera, Program* program) const
{
    OPTICK_CATEGORY("Draw", Optick::Category::Rendering);

    // Draw yourself
    Draw(camera, program);
    // Draw children recursively
    for (const GameObject* child : children)
    {
        child->DrawAll(camera, program);
    }
}

void Hachiko::GameObject::Draw(ComponentCamera* camera, Program* program) const
{
    OPTICK_CATEGORY("Draw", Optick::Category::Rendering);
    // Call draw on all components
    for (Component* component : components)
    {
        component->Draw(camera, program);
    }
}

void Hachiko::GameObject::DrawStencil(ComponentCamera* camera, Program* program)
{
    auto* mesh_renderer = GetComponent<ComponentMeshRenderer>();
    if (mesh_renderer)
    {
        mesh_renderer->DrawStencil(camera, program);
    }
}

void Hachiko::GameObject::OnTransformUpdated()
{
    // Update components
    for (Component* component : components)
    {
        component->OnTransformUpdated();
    }

    // Update children
    for (GameObject* child : children)
    {
        child->OnTransformUpdated();
    }

    UpdateBoundingBoxes();
}

void Hachiko::GameObject::DebugDrawAll()
{
    // Draw yourself
    DebugDraw();
    // Draw children recursively
    for (GameObject* child : children)
    {
        child->DebugDrawAll();
    }
}

void Hachiko::GameObject::DebugDraw() const
{
    //if (in_quadtree)
    //{
    DrawBoundingBox();
    //}
    DrawBones();
    for (Component* component : components)
    {
        component->DebugDraw();
    }
}

void Hachiko::GameObject::DrawBoundingBox() const
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

void Hachiko::GameObject::DrawBones() const
{
    if (parent != nullptr)
    {
        dd::line(this->GetTransform()->GetGlobalPosition(), parent->GetTransform()->GetGlobalPosition(), dd::colors::Blue);
        dd::axisTriad(this->GetTransform()->GetGlobalMatrix(), 0.005f, 0.1f);
    }
}

void Hachiko::GameObject::UpdateBoundingBoxes()
{
    /* Improvement in quadtree: only add the ones with a mesh 
    in_quadtree = false;
    
    constexpr float default_bounding_size = 1.0f;
    // If there is no mesh generate a default size
    aabb.SetNegativeInfinity();
    aabb.SetFromCenterAndSize(transform->GetGlobalPosition(), float3(default_bounding_size));
    obb = aabb;

    bool mesh_renderer_found = false;
    for (int i = 0; i < components.size(); ++i)
    {
        if (components[i]->GetType() == Component::Type::MESH)
        {
            ComponentMeshRenderer* component_mesh_renderer = static_cast<ComponentMeshRenderer*>(components[i]);
            if (mesh_renderer_found)
            {
                math::OBB aux_obb = component_mesh_renderer->GetAABB();
                aux_obb.Transform(transform->GetGlobalMatrix());
                aabb.Enclose(aux_obb);

                if (obb.Volume() < aux_obb.Volume())
                {
                    // TODO: keep the biggest obb
                    obb = aux_obb;
                }
            }
            else
            {
                mesh_renderer_found = true;

                obb = component_mesh_renderer->GetAABB();
                obb.Transform(transform->GetGlobalMatrix());
                // Enclose is accumulative, reset the box
                aabb.SetNegativeInfinity();
                aabb.Enclose(obb);
            }
        }
    }
    // Without the check main camera crashes bcs there is no quadtree
    if (scene_owner)
    {
        // TODO: only insert if there a mesh
        const Quadtree* quadtree = scene_owner->GetQuadtree();
        quadtree->Remove(this);
        if (mesh_renderer_found)
        {
            in_quadtree = true;
            quadtree->Insert(this);
        }
    } */

    ComponentMeshRenderer* component_mesh_renderer = GetComponent<ComponentMeshRenderer>();
    if (component_mesh_renderer != nullptr)
    {
        obb = component_mesh_renderer->GetAABB();
        obb.Transform(transform->GetGlobalMatrix());
        // Enclose is accumulative, reset the box
        aabb.SetNegativeInfinity();
        aabb.Enclose(obb);
    }
    else
    {
        constexpr float default_bounding_size = 1.0f;
        // If there is no mesh generate a default size
        aabb.SetNegativeInfinity();
        aabb.SetFromCenterAndSize(transform->GetGlobalPosition(), float3(default_bounding_size));
        obb = aabb;
    }

    // Without the check main camera crashes bcs there is no quadtree
    if (scene_owner)
    {
        const Quadtree* quadtree = scene_owner->GetQuadtree();
        quadtree->Remove(this);
        quadtree->Insert(this);
    }
}

bool Hachiko::GameObject::AttemptRemoveComponent(Component* component)
{
    //TODO: Should I delete the component?
    if (component->CanBeRemoved())
    {
        components.erase(std::remove(components.begin(), components.end(), component));
        return true;
    }

    return false;
}

void Hachiko::GameObject::ForceRemoveComponent(Component* component)
{
    components.erase(std::remove(components.begin(), components.end(), component));
}

void Hachiko::GameObject::Save(YAML::Node& node, bool as_prefab) const
{
    if (!as_prefab)
    {
        node[GAME_OBJECT_ID] = uid;
    }
    
    node[GAME_OBJECT_NAME] = name.c_str();
    node[GAME_OBJECT_ENABLED] = active;
   
    for (unsigned i = 0; i < components.size(); ++i)
    {
        if (!as_prefab)
        {
            node[COMPONENT_NODE][i][COMPONENT_ID] = static_cast<size_t>(components[i]->GetID());
        }

        node[COMPONENT_NODE][i][COMPONENT_TYPE] = static_cast<int>(components[i]->GetType());
        node[COMPONENT_NODE][i][COMPONENT_ENABLED] = components[i]->IsActive();
        components[i]->Save(node[COMPONENT_NODE][i]);
    }

    for (unsigned i = 0; i < children.size(); ++i)
    {
        children[i]->Save(node[CHILD_NODE][i], as_prefab);
    }
}

void Hachiko::GameObject::Load(const YAML::Node& node, bool as_prefab)
{   
    const YAML::Node components_node = node[COMPONENT_NODE];
    for (unsigned i = 0; i < components_node.size(); ++i)
    {

        UID component_id;
        if (!as_prefab)
        {
            component_id = components_node[i][COMPONENT_ID].as<UID>();
        }
        else
        {
            component_id = UUID::GenerateUID();
        }
        
        bool active = components_node[i][COMPONENT_ENABLED].as<bool>();
        const auto type = static_cast<Component::Type>(
            components_node[i][COMPONENT_TYPE].as<int>());

        Component* component = nullptr;

        if (type == Component::Type::SCRIPT)
        {
            std::string script_name =
                components_node[i][SCRIPT_NAME].as<std::string>();
            component = (Component*)(
                App->scripting_system->InstantiateScript(script_name, this));

            if (component != nullptr)
            {
                this->AddComponent(component);
            }
        }
        else
        {
            component = CreateComponent(type);
        }

        if (component != nullptr)
        {
            component->SetID(component_id);
            component->Load(components_node[i]);
            active ? component->Enable() : component->Disable();
        }
    }

    const YAML::Node children_nodes = node[CHILD_NODE];
    if (!children_nodes.IsDefined())
    {
        return;
    }

    for (unsigned i = 0; i < children_nodes.size(); ++i)
    {
        std::string child_name = children_nodes[i][GAME_OBJECT_NAME].as<std::string>();
        UID child_uid;
        if (!as_prefab)
        {
            child_uid = children_nodes[i][GAME_OBJECT_ID].as<UID>();
        }
        else
        {
            child_uid = UUID::GenerateUID();
        }
        
        const auto child = new GameObject(this, child_name.c_str(), child_uid);
        child->scene_owner = scene_owner;
        child->Load(children_nodes[i], as_prefab);
    }
}

Hachiko::GameObject* Hachiko::GameObject::Find(UID id) const
{
    for (GameObject* child : children)
    {
        if (child->uid == id)
        {
            return child;
        }

        GameObject* descendant = child->Find(id);

        if (descendant != nullptr)
        {
            return descendant;
        }
    }

    return nullptr;
}

std::vector<Hachiko::Component*> 
Hachiko::GameObject::GetComponents(Component::Type type) const
{
    std::vector<Component*> components_of_type;

    components_of_type.reserve(components.size());

    for (Component* component : components)
    {
        if (component->GetType() == type)
        {
            components_of_type.push_back(component);
        }
    }

    return components_of_type;
}

std::vector<Hachiko::Component*>
Hachiko::GameObject::GetComponentsInDescendants(Component::Type type) const
{
    std::vector<Component*> components_in_descendants;

    for (GameObject* child : children)
    {
        std::vector<Component*> components_in_child =
            child->GetComponents(type);

        for (Component* component_in_child : components_in_child)
        {
            components_in_descendants.push_back(component_in_child);
        }

        std::vector<Component*> components_in_childs_descendants =
            child->GetComponentsInDescendants(type);

        for (Component* component_in_childs_descendants :
             components_in_childs_descendants)
        {
            components_in_descendants.push_back(
                component_in_childs_descendants);
        }
    }

    return components_in_descendants;
}

Hachiko::GameObject* Hachiko::GameObject::GetFirstChildWithName(
    const std::string& child_name) const
{
    for (GameObject* child : children)
    {
        if (child->name == child_name)
        {
            return child;
        }
    }

    return nullptr;
}

Hachiko::GameObject* Hachiko::GameObject::FindDescendantWithName(const std::string& child_name) const
{
    for (GameObject* child : children)
    {
        if (child->name == child_name)
        {
            return child;
        }
    }

    for (GameObject* child : children)
    {
        GameObject* found_child = child->FindDescendantWithName(child_name);
        if (found_child != nullptr)
        {
            return found_child;
        }
    }

    return nullptr;
}
