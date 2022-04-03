#include "core/hepch.h"
#include "scripting/Script.h"
#include "core/GameObject.h"
#include "Application.h"
#include "modules/ModuleScriptingSystem.h"

Hachiko::Scripting::Script::Script(GameObject* new_game_object, 
	std::string new_name) 
	: Component(Hachiko::Component::Type::SCRIPT, new_game_object)
	, ISerializable()
	, name(new_name) 
{
	active = true;
}

void Hachiko::Scripting::Script::Update() 
{
    if (!App->scripting_system->ShouldUpdateScripts())
    {
        return;
    }
	
	// TODO(Baran): Add check for play mode/editor mode to run update of 
	// script.

    __try
    {
        OnUpdate();
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        HE_LOG("Exception occured on script '%s'", GetName().c_str());

        App->scripting_system->StopUpdatingScripts();

        HE_LOG("Therefore, scripts are paused.");
    }
}

void Hachiko::Scripting::Script::DrawGui()
{
    ImGui::PushID(this);
    
    if (ImGuiUtils::CollapsingHeader(game_object, this, name.c_str()))
    {
        ImGui::Text(name.c_str());
        ImGui::SameLine();
        ImGui::Text("TO BE IMPLEMENTED.");
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