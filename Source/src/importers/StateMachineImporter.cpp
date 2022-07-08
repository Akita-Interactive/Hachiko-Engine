#include "core/hepch.h"

#include "StateMachineImporter.h"
#include "resources/ResourceStateMachine.h"
#include "resources/ResourceAnimation.h"
#include "modules/ModuleResources.h"

void Hachiko::StateMachineImporter::Import(const char* path, YAML::Node& meta)
{
    static const int resource_index = 0;
    UID uid = ManageResourceUID(Resource::Type::STATE_MACHINE, resource_index, meta);

    FileSystem::Copy(path, GetResourcePath(Resource::Type::STATE_MACHINE, uid).c_str());
}

void Hachiko::StateMachineImporter::Save(UID id, const Resource* resource)
{
    // To save state machine we just update the asset and call the whole asset import pipeline
    const ResourceStateMachine* state_machine = static_cast<const ResourceStateMachine*>(resource);
      
    GenerateAssetStateMachine(state_machine);

    const std::string asset_path = StringUtils::Concat(GetResourcesPreferences()->GetAssetsPath(Resource::AssetType::STATE_MACHINE), 
        state_machine->state_m_name, STATE_MACHINE_EXTENSION);

    App->resources->ImportAssetFromAnyPath(asset_path);
}

Hachiko::Resource* Hachiko::StateMachineImporter::Load(UID id)
{
    const std::string state_machine_library_path = GetResourcePath(Resource::Type::STATE_MACHINE, id);

    if (!FileSystem::Exists(state_machine_library_path.c_str()))
    {
        return nullptr;
    }

    YAML::Node node = YAML::LoadFile(state_machine_library_path);
    ResourceStateMachine* state_machine = new ResourceStateMachine(id);

    // Name
    state_machine->state_m_name = node[SM_NAME].as<std::string>();

    // Default Node
    state_machine->SetDefaultNode(node[SM_DEFAULT_NODE].as<unsigned int>());

    // Clips
    if (node[SM_CLIPS].size() > 0)
    {
        for (unsigned int clip_index = 0; clip_index < node[SM_CLIPS].size(); ++clip_index)
        {
            state_machine->AddClip(
                node[SM_CLIPS][clip_index][SM_CLIP_NAME].as<std::string>(), 
                node[SM_CLIPS][clip_index][SM_CLIP_ID].as<UID>(), 
                node[SM_CLIPS][clip_index][SM_CLIP_LOOP].as<bool>()
            );
        }
    }

    // Nodes
    if (node[SM_NODES].size() > 0)
    {
        for (unsigned int node_index = 0; node_index < node[SM_NODES].size(); ++node_index)
        {
            state_machine->AddNode(
                node[SM_NODES][node_index][SM_NODE_NAME].as<std::string>(), 
                node[SM_NODES][node_index][SM_NODE_CLIP_NAME].as<std::string>()
            );
        }
    }

    // Transitions
    if (node[SM_TRANSITIONS].size() > 0)
    {
        for (unsigned int transition_index = 0; transition_index < node[SM_TRANSITIONS].size(); ++transition_index)
        {
            state_machine->AddTransition(node[SM_TRANSITIONS][transition_index][SM_TRANSITION_SOURCE].as<std::string>(),
                                         node[SM_TRANSITIONS][transition_index][SM_TRANSITION_TARGET].as<std::string>(),
                                         node[SM_TRANSITIONS][transition_index][SM_TRANSITION_TRIGGER].as<std::string>(),
                                         node[SM_TRANSITIONS][transition_index][SM_TRANSITION_BLEND].as<unsigned int>());
        }            
    }

    return state_machine;
}

void Hachiko::StateMachineImporter::GenerateAssetStateMachine(const ResourceStateMachine* state_machine)
{
    const std::string asset_path = StringUtils::Concat(GetResourcesPreferences()->GetAssetsPath(Resource::AssetType::STATE_MACHINE), state_machine->state_m_name, STATE_MACHINE_EXTENSION);

    YAML::Node state_machine_node;

    // Save name
    state_machine_node[SM_NAME] = state_machine->GetName();

    // Save default node
    state_machine_node[SM_DEFAULT_NODE] = state_machine->GetDefaultNode();

    // Save clips
    if (state_machine->GetNumClips() > 0)
    {
        for (unsigned int clip_index = 0; clip_index < state_machine->GetNumClips(); ++clip_index)
        {
            state_machine_node[SM_CLIPS][clip_index][SM_CLIP_NAME] = state_machine->GetClipName(clip_index);
            state_machine_node[SM_CLIPS][clip_index][SM_CLIP_ID] = state_machine->GetClipRes(clip_index) != nullptr 
                ? state_machine->GetClipRes(clip_index)->GetID() : 0;
            state_machine_node[SM_CLIPS][clip_index][SM_CLIP_LOOP] = state_machine->GetClipLoop(clip_index);
        }
    }

    // Save nodes

    if (state_machine->GetNumNodes() > 0)
    {
        for (unsigned int node_index = 0; node_index < state_machine->GetNumNodes(); ++node_index)
        {
            state_machine_node[SM_NODES][node_index][SM_NODE_NAME] = state_machine->GetNodeName(node_index);
            state_machine_node[SM_NODES][node_index][SM_NODE_CLIP_NAME] = state_machine->GetNodeClip(node_index);
        }
    }

    // Save transitions

    if (state_machine->GetNumTransitions() > 0)
    {
        for (unsigned int transition_index = 0; transition_index < state_machine->GetNumTransitions(); ++transition_index)
        {
            state_machine_node[SM_TRANSITIONS][transition_index][SM_TRANSITION_SOURCE] = state_machine->GetTransitionSource(transition_index);
            state_machine_node[SM_TRANSITIONS][transition_index][SM_TRANSITION_TARGET] = state_machine->GetTransitionTarget(transition_index);
            state_machine_node[SM_TRANSITIONS][transition_index][SM_TRANSITION_TRIGGER] = state_machine->GetTransitionTrigger(transition_index);
            state_machine_node[SM_TRANSITIONS][transition_index][SM_TRANSITION_BLEND] = state_machine->GetTransitionBlend(transition_index);
        }
    }

    FileSystem::Save(asset_path.c_str(), state_machine_node);
}

Hachiko::UID Hachiko::StateMachineImporter::CreateAsset(const std::string& name)
{
    ResourceStateMachine state_machine = ResourceStateMachine(0);
    state_machine.SetName(name);

    GenerateAssetStateMachine(&state_machine);

    const std::string state_machine_path = StringUtils::Concat(GetResourcesPreferences()->GetAssetsPath(Resource::AssetType::STATE_MACHINE), name, STATE_MACHINE_EXTENSION);
    return App->resources->ImportAssetFromAnyPath(state_machine_path)[0];
}
