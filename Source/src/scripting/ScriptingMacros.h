#pragma once

#include "scripting/serialization/SerializedField.h"
#include <unordered_map>
#include <string>

#define SERIALIZE_FIELD(type, name) type name

#define SERIALIZATION_METHODS_true                                            \
public:                                                                       \
    virtual void SerializeTo(                                                 \
        std::unordered_map<std::string, SerializedField>& serialized_fields   \
    ) override;                                                               \
    virtual void DeserializeFrom(                                             \
        std::unordered_map<std::string, SerializedField>& serialized_fields   \
    ) override;                                                               \
    virtual void OnEditor() override;                                         \
    virtual void OnLoad() override;                                           \
    virtual void OnSave(YAML::Node& node) const override;                                           

#define SERIALIZATION_METHODS_false                                           \
public:                                                                       \
    void SerializeTo(                                                         \
        std::unordered_map<std::string, SerializedField>& serialized_fields   \
    ) override;                                                               \
    void DeserializeFrom(                                                     \
        std::unordered_map<std::string, SerializedField>& serialized_fields   \
    ) override;                                                               \
    void OnEditor() override;                                                 \
    void OnLoad() override;                                                   \
    void OnSave(YAML::Node& node) const override;                                           

#define SCRIPTING_METHODS_BASE                                                \
public:                                                                       \
    virtual void SerializeTo(                                                 \
        std::unordered_map<std::string, SerializedField>& serialized_fields   \
    ) override;                                                               \
    virtual void DeserializeFrom(                                             \
        std::unordered_map<std::string, SerializedField>& serialized_fields   \
    ) override;                                                               \
    virtual void OnEditor() {};                                               \
    virtual void OnLoad() {};                                                 \
    virtual void OnSave(YAML::Node& node) const {};

// This needs to be added to all script sub classes:
#define SERIALIZATION_METHODS(is_virtual) SERIALIZATION_METHODS_##is_virtual
