#include "core/hepch.h"
#include "CameraPreferences.h"

using namespace Hachiko;

CameraPreferences::CameraPreferences()
    : Preferences(Type::CAMERA)
{
    group_name = CAMERA_NODE;
}

void CameraPreferences::LoadConfigurationData(const YAML::Node& node)
{
    for (auto it = node.begin(); it != node.end(); ++it)
    {
        if (it->first.as<std::string>()._Equal(MOVE_SPEED))
        {
            move_speed = it->second.as<int>();
            continue;
        }

        if (it->first.as<std::string>()._Equal(ROTATION_SPEED))
        {
            rotation_speed = it->second.as<int>();
            continue;
        }

        if (it->first.as<std::string>()._Equal(ORBIT_SPEED))
        {
            orbit_speed = it->second.as<int>();
            continue;
        }

        if (it->first.as<std::string>()._Equal(ZOOM_SPEED))
        {
            zoom_speed = it->second.as<int>();
            continue;
        }

        if (it->first.as<std::string>()._Equal(FOV))
        {
            fov = it->second.as<float>();
        }
    }
}

void CameraPreferences::SaveConfigurationData(YAML::Node& node)
{
    node[group_name][MOVE_SPEED] = move_speed;
    node[group_name][ROTATION_SPEED] = rotation_speed;
    node[group_name][ORBIT_SPEED] = orbit_speed;
    node[group_name][ZOOM_SPEED] = zoom_speed;
    node[group_name][FOV] = fov;
}
