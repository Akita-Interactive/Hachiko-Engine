#pragma once

#include "HachikoApiDefine.h"
#include "scripting/ScriptingMacros.h"
#include "scripting/serialization/IRuntimeSerializable.h"
#include "components/Component.h"

#include <unordered_map>

struct ImGuiContext;

namespace Hachiko
{
class GameObject;

namespace Scripting
{
class HACHIKO_API Script : public Component, public IRuntimeSerializable
{
    SERIALIZATION_METHODS(true)
public:
    virtual ~Script() = default;
    
    void Update() override;
    void Start() override;
    void Awake();

    void Save(YAML::Node& node) const override;
    void Load(const YAML::Node& node) override;
    void DrawGui() override;

    virtual void OnAwake() {};
    virtual void OnStart() {};
    virtual void OnUpdate() {};
    virtual void OnDisable() {};
    virtual void OnEnable() {};
    virtual void OnEditor(ImGuiContext* context) {};

    const std::string& GetName() const;

protected:
    explicit Script(GameObject* new_game_object, std::string new_name);

private:
    std::string name;

};
} // mamespace Scripting
} // namespace Hachiko