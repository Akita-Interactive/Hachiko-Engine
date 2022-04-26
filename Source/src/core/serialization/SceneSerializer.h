#pragma once

namespace Hachiko
{
    class Scene;
    class GameObject;
    class Component;
    class ResourcesPreferences;

    class SceneSerializer
    {
        friend ModuleSceneManager;

    public:
        SceneSerializer() = default;
        ~SceneSerializer() = default;

        bool Save(const Scene* scene, const char* path = nullptr);
    private:
        // Load is private to ensure only ModuleScene can call it 
        // as we retrieve ownership of scene object pointer
        Scene* Load(const char* path);
        ResourcesPreferences* preferences;
    };
}