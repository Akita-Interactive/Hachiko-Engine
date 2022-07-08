#pragma once

#include "Importer.h"

namespace Hachiko
{
    class ResourceStateMachine;

    class StateMachineImporter : public Importer
    {
        friend class ModelImporter;

    public:
        StateMachineImporter() = default;
        ~StateMachineImporter() override = default;

        // Copies the state machine file to lib
        void Import(const char* path, YAML::Node& meta) override;

        void Save(UID id, const Resource* resource) override;
        Resource* Load(UID id) override;

        void GenerateAssetStateMachine(const ResourceStateMachine* state_machine);
        UID CreateAsset(const std::string& name);
    };
} // namespace Hachiko