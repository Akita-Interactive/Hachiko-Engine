#include "core/hepch.h"
#include "scripting/Script.h"
#include "core/GameObject.h"
#include "Application.h"
#include "modules/ModuleScriptingSystem.h"

Hachiko::Scripting::Script::Script(GameObject* new_game_object, 
	std::string new_name) 
	: Component(Hachiko::Component::Type::SCRIPT, new_game_object)
	, IRuntimeSerializable()
	, name(new_name) 
{
	active = true;
}

void Hachiko::Scripting::Script::Update() 
{
    if (!App->scripting_system->ShouldExecuteScripts())
    {
        return;
    }
	

#ifndef SCRIPTING_UNSAFE
    __try
    {
        OnUpdate();
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        HE_LOG("Exception occured on script '%s::OnUpdate'", GetName().c_str());

        App->scripting_system->StopExecutingScripts();

        HE_LOG("Therefore, scripts are paused.");
    }
#else
    OnUpdate();
#endif // SCRIPTING_UNSAFE
}

void Hachiko::Scripting::Script::Start()
{
    if (!App->scripting_system->ShouldExecuteScripts())
    {
        return;
    }

#ifndef SCRIPTING_UNSAFE
    __try
    {
        OnStart();
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        HE_LOG("Exception occured on script '%s::OnStart'", GetName().c_str());

        App->scripting_system->StopExecutingScripts();

        HE_LOG("Therefore, scripts are paused.");
    }
#else
    OnStart();
#endif // SCRIPTING_UNSAFE
}

void Hachiko::Scripting::Script::Awake()
{
    if (!App->scripting_system->ShouldExecuteScripts())
    {
        return;
    }

#ifndef SCRIPTING_UNSAFE
    __try
    {
        OnAwake();
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        HE_LOG("Exception occured on script '%s::OnAwake'", GetName().c_str());

        App->scripting_system->StopExecutingScripts();

        HE_LOG("Therefore, scripts are paused.");
    }
#else
    OnAwake();
#endif // SCRIPTING_UNSAFE
}

void Hachiko::Scripting::Script::Save(YAML::Node& node) const
{
    node.SetTag("script");
    node[SCRIPT_NAME] = name;
    
    OnSave(node);
}

void Hachiko::Scripting::Script::Load(const YAML::Node& node)
{
    // Copy the node so we can defer the loading process of the scripts to the 
    // time when all the scene is loaded. So that scripts are loaded after 
    // everything else and any attached component or gameobject is already 
    // created at the load time of the script. This may be improved so that 
    // the nodes are stored inside ModuleScriptingSystem, and scripts don't care
    // about storing these:
    load_node = node;
    // TODO: See if storing these in ModuleScriptingSystem is a better solution
    // after VS2.
}

void Hachiko::Scripting::Script::SavePrefabReferences(YAML::Node& node, std::vector<const GameObject*>& object_collection, std::vector<const Component*>& component_collection) const
{
    static const std::string component_node("@Component");
    static const std::string gameobject_node("@GameObject");
    // Iterate over the serialized yaml and replace uids for index references 
    // obtained from collecting gos and components from the scene into a vector.
    // 0 Means it was not found, on load first collection object will contain nullptr to make this work seamlessly
    for (YAML::iterator it = node.begin(); it != node.end(); ++it)
    {
        if (it->first.as<std::string>().find(gameobject_node) != std::string::npos)
        {
            UID gameobject_uid = it->second.as<UID>();
            auto found = std::find_if(object_collection.begin() + 1, object_collection.end(), 
                [&](const GameObject* go) { return go->GetID() == gameobject_uid; });

            unsigned gameobject_index = 0;

            if (found != object_collection.end())
            {
                gameobject_index = std::distance(object_collection.begin(), found);
            }

            it->second = gameobject_index;
        }
        else if (it->first.as<std::string>().find(component_node) != std::string::npos)
        {
            UID component_uid = it->second.as<UID>();
            auto found = std::find_if(component_collection.begin() + 1, component_collection.end(), 
                [&](const Component* c) { return c->GetID() == component_uid; });

            unsigned component_index = 0;
            
            if (found != component_collection.end())
            {
                component_index = std::distance(component_collection.begin(), found);
            }

            it->second = component_index;
        }
    }
}

void Hachiko::Scripting::Script::LoadPrefabReferences(std::vector<const GameObject*>& object_collection, std::vector<const Component*>& component_collection)
{
    static const std::string component_node("@Component");
    static const std::string gameobject_node("@GameObject");

    // Iterate over the serialized yaml and replace uids for index references
    // obtained from collecting gos and components from the scene into a vector.
    // 0 Means it was not found, on load first collection object will contain nullptr to make this work seamlessly    
    for (YAML::iterator it = load_node.begin(); it != load_node.end(); ++it)
    {
        if (it->first.as<std::string>().find(gameobject_node) != std::string::npos)
        {
            unsigned gameobject_index = it->second.as<unsigned>();
            UID gameobject_uid = 0;
            
            // Index 0 contains nullptr
            if (gameobject_index)
            {
                gameobject_uid = object_collection[gameobject_index]->GetID();
            }

            it->second = gameobject_uid;
        }
        else if (it->first.as<std::string>().find(component_node) != std::string::npos)
        {
            unsigned component_index = it->second.as<unsigned>();
            UID component_uid = 0;

            // Index 0 contains nullptr
            if (component_index)
            {
                component_uid = component_collection[component_index]->GetID();
            }

            it->second = component_uid;
        }
    }
}

void Hachiko::Scripting::Script::DrawGui()
{
    ImGui::PushID(this);
    
    if (ImGuiUtils::CollapsingHeader(game_object, this, name.c_str()))
    {
        OnEditor();
    }

    ImGui::PopID();
}

const std::string& Hachiko::Scripting::Script::GetName() const
{
	return name;
}

void Hachiko::Scripting::Script::SerializeTo(std::unordered_map<std::string, 
	SerializedField>& serialized_fields)
{
    // For field game_object : GameObject
	serialized_fields["game_object"] = SerializedField(
		std::string("game_object"), 
		std::make_any<GameObject*>(game_object), 
		std::string("GameObject*"));

	// For field active : bool
	serialized_fields["active"] = SerializedField(
		std::string("active"), 
		std::make_any<bool>(active), 
		std::string("bool"));

    // For field uid : UID
	serialized_fields["uid"] = SerializedField(
		std::string("uid"), 
		std::make_any<UID>(GetID()), 
		std::string("UID"));
}

void Hachiko::Scripting::Script::DeserializeFrom(
	std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	if (serialized_fields.empty())
	{
        return;
	}

	// For field game_object : GameObject
    game_object = 
		std::any_cast<GameObject*>(serialized_fields["game_object"].copy);

	// For field active : bool
	active = std::any_cast<bool>(serialized_fields["active"].copy);

	// For field uid : UID
    OverrideUID(std::any_cast<UID>(serialized_fields["uid"].copy));
}