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
#include "components/ComponentAudioListener.h"
#include "components/ComponentAudioSource.h"
#include "components/ComponentParticleSystem.h"
#include "components/ComponentBillboard.h"
#include "components/ComponentVideo.h"

// UI
#include "components/ComponentCanvas.h"
#include "components/ComponentCanvasRenderer.h"
#include "components/ComponentTransform2D.h"
#include "components/ComponentImage.h"
#include "components/ComponentButton.h"
#include "components/ComponentProgressBar.h"
#include "components/ComponentText.h"

#include "importers/PrefabImporter.h"
#include "scripting/Script.h"

#include "Application.h"
#include "modules/ModuleSceneManager.h"
#include "modules/ModuleScriptingSystem.h" // For instantiating Scripts.
#include "modules/ModuleResources.h"

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
    App->scene_manager->RemovedGameObject(this);
    if (parent)
    {
        parent->RemoveChild(this);
    }

    for (GameObject* child : children)
    {
        child->parent = nullptr;
        RELEASE(child)
    }
    for (const Component* component : components)
    {
        RELEASE(component);
    }
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
        case Component::Type::TRANSFORM:
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
        case Component::Type::TRANSFORM:
            return transform;
        case Component::Type::CAMERA:
            new_component = new ComponentCamera(this);
            break;
        case Component::Type::ANIMATION:
            new_component = new ComponentAnimation(this);
            break;
        case Component::Type::MESH_RENDERER:
            new_component = new ComponentMeshRenderer(this);
            break;
        case Component::Type::DIRLIGHT:
            new_component = new ComponentDirLight(this);
            break;
        case Component::Type::POINTLIGHT:
            new_component = new ComponentPointLight(this);
            break;
        case Component::Type::SPOTLIGHT:
            new_component = new ComponentSpotLight(this);
            break;
        case Component::Type::CANVAS:
            if (!GetComponent<ComponentCanvas>())
            {
                new_component = new ComponentCanvas(this);
            }
            break;
        case Component::Type::CANVAS_RENDERER:
            if (!GetComponent<ComponentCanvasRenderer>())
            {
                new_component = new ComponentCanvasRenderer(this);
            }
            break;
        case Component::Type::TRANSFORM_2D:
            if (!GetComponent<ComponentTransform2D>())
            {
                new_component = new ComponentTransform2D(this);
            }
            break;
        case Component::Type::IMAGE:
            if (!GetComponent<ComponentImage>())
            {
                new_component = new ComponentImage(this);
            }
            break;
        case Component::Type::BUTTON:
            if (!GetComponent<ComponentButton>())
            {
                new_component = new ComponentButton(this);
            }
            break;
        case Component::Type::PROGRESS_BAR:
            if (!GetComponent<ComponentProgressBar>())
            {
                new_component = new ComponentProgressBar(this);
            }
            break;
        case (Component::Type::TEXT):
            if (!GetComponent<ComponentText>())
            {
                new_component = new ComponentText(this);
            }
            break;
        case Component::Type::OBSTACLE:
            if (!GetComponent<ComponentObstacle>())
            {
                new_component = new ComponentObstacle(this);
            }
            break;
        case Component::Type::AGENT:
            if (!GetComponent<ComponentAgent>())
            {
                new_component = new ComponentAgent(this);
            }
            break;
        case Component::Type::AUDIO_LISTENER:
            if (!GetComponent<ComponentAudioListener>())
            {
                new_component = new ComponentAudioListener(this);
            }
            break;
        case Component::Type::AUDIO_SOURCE:
            if (!GetComponent<ComponentAudioSource>())
            {
                new_component = new ComponentAudioSource(this);
            }
            break;
        case Component::Type::PARTICLE_SYSTEM:
            if (!GetComponent<ComponentParticleSystem>())
            {
                new_component = new ComponentParticleSystem(this);
            }
            break;
        case Component::Type::BILLBOARD:
            if (!GetComponent<ComponentBillboard>())
            {
                new_component = new ComponentBillboard(this);
            }
            break;
        case Component::Type::VIDEO:
            if (!GetComponent<ComponentVideo>())
            {
                new_component = new ComponentVideo(this);
            }
            break;
    }

    if (new_component != nullptr)
    {
        components.push_back(new_component);
    }
    else
    {
        HE_ERROR("Falied to create component");
    }

    return new_component;
}
void Hachiko::GameObject::SetActiveNonRecursive(bool set_active)
{
    if (!active && set_active)
    {
        Start();
    }
    else if (active && !set_active)
    {
        for (Component* component : components)
        {
            component->OnDisable();
        }
    }
    active = set_active;
}

void Hachiko::GameObject::SetActive(bool set_active)
{
    for (GameObject* child : children)
    {
        child->SetActive(set_active);
    }

    if (!active && set_active)
    {
        Start();
    }
    else if (active && !set_active)
    {
        for (Component* component : components)
        {
            component->OnDisable();
        }
    }
    active = set_active;
}

void Hachiko::GameObject::Start()
{
    if (!started)
    {

        for (GameObject* child : children)
        {
            if (child->IsActive())
            {
                child->Start();
            }
        }

        for (Component* component : components)
        {
            if (component->GetType() == Component::Type::SCRIPT)
            {
                continue;
            }
            component->Start();
        }
        started = true;
    }
}

void Hachiko::GameObject::Stop()
{
    if (started)
    {
        for (Component* component : components)
        {
            component->Stop();
        }

        for (GameObject* child : children)
        {
            if (child->IsActive())
            {
                child->Stop();
            }
        }
        started = false;
    }
}

void Hachiko::GameObject::Update()
{
    if (!active || (parent != nullptr && !parent->active))   return;

    if (transform->HasChanged())
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
        if (!components[i]->IsActive())
        {
            continue;
        }
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
    //DrawBones();
    for (Component* component : components)
    {
        component->DebugDraw();
    }
}

void Hachiko::GameObject::DrawBones() const
{
    if (parent != nullptr)
    {
        dd::line(this->GetTransform()->GetGlobalPosition(), parent->GetTransform()->GetGlobalPosition(), dd::colors::Blue);
        dd::axisTriad(this->GetTransform()->GetGlobalMatrix(), 0.005f, 0.1f);
    }
}

bool Hachiko::GameObject::AttemptRemoveComponent(const Component* component)
{
    if (component->CanBeRemoved())
    {
        const auto it = std::find(components.begin(), components.end(), component);
        if (it != components.end())
        {
            delete *it;
            components.erase(it);
        }
        return true;
    }

    return false;
}

// Be aware that this method does not free the memory of the component
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

void Hachiko::GameObject::CollectObjectsAndComponents(std::vector<const GameObject*>& object_collector, std::vector<const Component*>& component_collector)
{
    object_collector.push_back(this);

    for (unsigned i = 0; i < components.size(); ++i)
    {
        component_collector.push_back(components[i]);
    }

    for (unsigned i = 0; i < children.size(); ++i)
    {
        children[i]->CollectObjectsAndComponents(object_collector, component_collector);
    }
}

void Hachiko::GameObject::Load(const YAML::Node& node, bool as_prefab, bool meshes_only)
{
    // Save the enabled state of the game object:
    bool is_active = node[GAME_OBJECT_ENABLED].IsDefined() ? node[GAME_OBJECT_ENABLED].as<bool>() : true;
    SetActive(is_active);

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

        bool component_is_active = components_node[i][COMPONENT_ENABLED].as<bool>();
        const auto type = static_cast<Component::Type>(components_node[i][COMPONENT_TYPE].as<int>());

        Component* component = nullptr;

        if (meshes_only)
        {
            if (type == Component::Type::MESH_RENDERER || type == Component::Type::TRANSFORM)
            {
                component = CreateComponent(type);
            }
        }
        else if (type == Component::Type::SCRIPT)
        {
            std::string script_name = components_node[i][SCRIPT_NAME].as<std::string>();
            component = (Component*)App->scripting_system->InstantiateScript(script_name, this);

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
            component_is_active ? component->Enable() : component->Disable();
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
        child->Load(children_nodes[i], as_prefab, meshes_only);
    }
}

void Hachiko::GameObject::CollectResources(const YAML::Node& node, std::map<Resource::Type, std::set<UID>>& resources)
{
    const YAML::Node components_node = node[COMPONENT_NODE];
    for (unsigned i = 0; i < components_node.size(); ++i)
    {
        const auto type = static_cast<Component::Type>(components_node[i][COMPONENT_TYPE].as<int>());

        switch (type)
        {
            case Component::Type::ANIMATION:
                ComponentAnimation::CollectResources(components_node[i], resources);
                break;
            case Component::Type::MESH_RENDERER:
                ComponentMeshRenderer::CollectResources(components_node[i], resources);
                break;
            case Component::Type::IMAGE:
                ComponentImage::CollectResources(components_node[i], resources);
                break;
            case Component::Type::TEXT:
                ComponentText::CollectResources(components_node[i], resources);
                break;
            case Component::Type::PARTICLE_SYSTEM:
                ComponentParticleSystem::CollectResources(components_node[i], resources);
                break;
            case Component::Type::BILLBOARD:
                ComponentBillboard::CollectResources(components_node[i], resources);
                break;
        }
    }

    const YAML::Node children_nodes = node[CHILD_NODE];
    if (!children_nodes.IsDefined())
    {
        return;
    }

    for (unsigned i = 0; i < children_nodes.size(); ++i)
    {
        CollectResources(children_nodes[i], resources);
    }
}

void Hachiko::GameObject::SavePrefabReferences(YAML::Node& node, std::vector<const GameObject*>& object_collection, std::vector<const Component*>& component_collection) const
{
    for (unsigned i = 0; i < components.size(); ++i)
    {
        Component::Type type = components[i]->GetType();
        if (type == Component::Type::SCRIPT)
        {
            // Override script data
            Scripting::Script* script_component = static_cast<Scripting::Script*>(components[i]);
            script_component->SavePrefabReferences(node[COMPONENT_NODE][i], object_collection, component_collection);
        }
    }

    for (unsigned i = 0; i < children.size(); ++i)
    {
        children[i]->SavePrefabReferences(node[CHILD_NODE][i], object_collection, component_collection);
    }
}

void Hachiko::GameObject::LoadPrefabReferences(std::vector<const GameObject*>& object_collection, std::vector<const Component*>& component_collection)
{
    for (unsigned i = 0; i < components.size(); ++i)
    {
        if (components[i]->GetType() == Component::Type::SCRIPT)
        {
            // Override script data, script already has its yaml data assigned on load
            Scripting::Script* script_component = static_cast<Scripting::Script*>(components[i]);
            script_component->LoadPrefabReferences(object_collection, component_collection);
        }
    }

    for (unsigned i = 0; i < children.size(); ++i)
    {
        children[i]->LoadPrefabReferences(object_collection, component_collection);
    }
}

void Hachiko::GameObject::SetTimeScaleMode(TimeScaleMode time_scale_mode) const
{
    for (Component* component : components)
    {
        component->SetTimeScaleMode(time_scale_mode);
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

Hachiko::Component* Hachiko::GameObject::GetComponent(Component::Type type) const
{
    for (Component* component : components)
    {
        if (component->GetType() == type)
        {
            return component;
        }
    }

    return nullptr;
}

std::vector<Hachiko::Component*> Hachiko::GameObject::GetComponents(Component::Type type) const
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

std::vector<Hachiko::Component*> Hachiko::GameObject::GetComponentsInDescendants(Component::Type type) const
{
    std::vector<Component*> components_in_descendants;

    for (GameObject* child : children)
    {
        std::vector<Component*> components_in_child = child->GetComponents(type);

        for (Component* component_in_child : components_in_child)
        {
            components_in_descendants.push_back(component_in_child);
        }

        std::vector<Component*> components_in_childs_descendants = child->GetComponentsInDescendants(type);
        for (Component* component_in_childs_descendants : components_in_childs_descendants)
        {
            components_in_descendants.push_back(component_in_childs_descendants);
        }
    }

    return components_in_descendants;
}

Hachiko::GameObject* Hachiko::GameObject::GetFirstChildWithName(const std::string& child_name) const
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

void Hachiko::GameObject::ChangeEmissiveColor(float4 color, bool include_children, bool override_flag)
{
    std::vector<ComponentMeshRenderer*> v_mesh_renderer = GetComponents<ComponentMeshRenderer>();
    for (int i = 0; i < v_mesh_renderer.size(); ++i)
    {
        v_mesh_renderer[i]->OverrideEmissive(color, override_flag);
    }

    if (!include_children)
        return;

    for (GameObject* child : children)
    {
        child->ChangeEmissiveColor(color, include_children, override_flag);
    }
}

std::vector<float4> Hachiko::GameObject::GetEmissiveColors() const
{
    std::vector<ComponentMeshRenderer*> v_mesh_renderer = GetComponents<ComponentMeshRenderer>();

    std::vector<float4> emissive_colors;
    emissive_colors.reserve(v_mesh_renderer.size());

    for (ComponentMeshRenderer* renderer : v_mesh_renderer)
    {
        emissive_colors.push_back(
            renderer->GetOverrideEmissiveColor());
    }

    if (emissive_colors.empty())
    {
        emissive_colors.push_back(float4::zero);
    }

    return emissive_colors;
}


void Hachiko::GameObject::ResetEmissive(bool include_children)
{
    std::vector<ComponentMeshRenderer*> v_mesh_renderer = GetComponents<ComponentMeshRenderer>();
    for (int i = 0; i < v_mesh_renderer.size(); ++i)
    {
        v_mesh_renderer[i]->LiftOverrideEmissive();
    }

    if (!include_children)
        return;

    for (GameObject* child : children)
    {
        child->ResetEmissive(include_children);
    }
}

void Hachiko::GameObject::ChangeTintColor(float4 color, bool include_children)
{
    std::vector<ComponentMeshRenderer*> v_mesh_renderer = GetComponents<ComponentMeshRenderer>();
    for (int i = 0; i < v_mesh_renderer.size(); ++i)
    {
        v_mesh_renderer[i]->SetTintColor(color);
    }

    if (!include_children)
        return;

    for (GameObject* child : children)
    {
        child->ChangeTintColor(color, include_children);
    }
}

void Hachiko::GameObject::ChangeDissolveProgress(float progress, bool include_children) 
{
    std::vector<ComponentMeshRenderer*> v_mesh_renderer = GetComponents<ComponentMeshRenderer>();
    for (ComponentMeshRenderer* component_mesh : v_mesh_renderer)
    {
        component_mesh->SetDissolveProgress(progress);
    }

    if (!include_children)
        return;

    for (GameObject* child : children)
    {
        child->ChangeDissolveProgress(progress, include_children);
    }
}

void Hachiko::GameObject::SetVisible(bool v, bool include_children)
{
    std::vector<ComponentMeshRenderer*> v_mesh_renderer = GetComponents<ComponentMeshRenderer>();
    for (int i = 0; i < v_mesh_renderer.size(); ++i)
    {
        v_mesh_renderer[i]->SetVisible(v);
    }

    if (!include_children)
        return;

    for (GameObject* child : children)
    {
        child->SetVisible(v, include_children);
    }
}

const OBB* Hachiko::GameObject::GetFirstMeshRendererOBB() const
{
    std::vector<ComponentMeshRenderer*> comp_mesh_renderer = GetComponents<ComponentMeshRenderer>();

    if (!comp_mesh_renderer.empty())
    {
        return &comp_mesh_renderer[0]->GetOBB();
    }
    return nullptr;
}

void Hachiko::GameObject::SimplifyChildren()
{
    std::vector<GameObject*> aux_children = children;
    for (int i = 0; i < aux_children.size(); ++i)
    {
        aux_children[i]->Simplify(this);

        if (aux_children[i]->components.size() <= 1)
            delete aux_children[i];
    }
}

void Hachiko::GameObject::Simplify(GameObject* target) 
{
    while (children.size() > 0)
    {
        children[0]->Simplify(target);
        
        if (children[0]->components.size() <= 1)
            delete children[0];
        else
            children[0]->SetNewParent(target);
    }
}
