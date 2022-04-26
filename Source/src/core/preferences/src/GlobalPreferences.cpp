#include "core/hepch.h"
#include "GlobalPreferences.h"

using namespace Hachiko;

GlobalPreferences::GlobalPreferences()
    : Preferences(Type::GLOBALS)
{
    group_name = GLOBAL_NODE;
}

void GlobalPreferences::LoadConfigurationData(const YAML::Node& node)
{
    for (auto it = node.begin(); it != node.end(); ++it)
    {
        if (it->first.as<std::string>()._Equal(TITLE_NODE))
        {
            title = it->second.as<std::string>();
            continue;
        }

        if (it->first.as<std::string>()._Equal(VERSION_NODE))
        {
            version = it->second.as<std::string>();
        }
    }
}

void GlobalPreferences::SaveConfigurationData(YAML::Node& node)
{
    node[group_name][TITLE_NODE] = title;
    node[group_name][VERSION_NODE] = version;
}
