#pragma once

#include <yaml-cpp/yaml.h>

namespace Hachiko
{
    // TODO: This class has to be removed as each preference type are different from eachother
    class Preferences
    {
    public:
        enum class Type
        {
            GLOBALS = 0,
            EDITOR,
            CAMERA,
            GAME,
            RENDER,
            RESOURCE_PREFS,
            COUNT // to know how many they are
        };

        Preferences(Type type) : type(type)
        {
        }

        virtual ~Preferences() = default;

        virtual void LoadConfigurationData(const YAML::Node& node) = 0;
        virtual void SaveConfigurationData(YAML::Node& node) = 0;
        virtual Type GetType() { return type; }
        [[nodiscard]] const std::string& GetGroupName() const { return group_name; }

    private:
        Type type;

    protected:
        std::string group_name = "";
    };
}
