#include "core/hepch.h"
#include "PreferenceManager.h"

#include "src/CameraPreferences.h"
#include "src/EditorPreferences.h"
#include "src/GlobalPreferences.h"
#include "src/RenderPreferences.h"
#include "src/ResourcesPreferences.h"

using namespace Hachiko;

PreferenceManager::PreferenceManager(const char* cfg)
    : globals(new GlobalPreferences())
    , editor(new EditorPreferences())
    , render(new RenderPreferences())
    , camera(new CameraPreferences())
    , resources(new ResourcesPreferences())
    , config_file(cfg)
{
    preferences.reserve(static_cast<size_t>(Preferences::Type::COUNT));

    preferences.emplace_back(globals);
    preferences.emplace_back(render);
    preferences.emplace_back(editor);
    preferences.emplace_back(camera);
    preferences.emplace_back(resources);

    if (std::filesystem::exists(config_file.c_str()))
    {
        nodes_vec = YAML::LoadAllFromFile(config_file);
        LoadConfigurationFile();
    }
    else
    {
        std::filesystem::create_directory(SETTINGS_FOLDER);
    }
}

PreferenceManager::~PreferenceManager()
{
    for (const auto it : preferences)
    {
        delete it;
    }
    preferences.clear();
}

void PreferenceManager::LoadConfigurationFile() const
{
    for (auto node : nodes_vec)
    {
        for (auto it : preferences)
        {
            if (!node[it->GetGroupName()].IsDefined())
            {
                continue;
            }
            it->LoadConfigurationData(node[it->GetGroupName()]);
        }
    }
}

void PreferenceManager::SaveConfigurationFile() const
{
    YAML::Node output;
    for (const auto it : preferences)
    {
        it->SaveConfigurationData(output);
    }
    std::ofstream fout(config_file);
    fout << output;
}

Preferences* PreferenceManager::GetPreference(const Preferences::Type type) const
{
    for (const auto it : preferences)
    {
        if (it->GetType() == type)
        {
            return it;
        }
    }
    return nullptr;
}
