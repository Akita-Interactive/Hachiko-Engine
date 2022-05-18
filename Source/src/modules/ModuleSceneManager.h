#pragma once
#include "Module.h"

#include "core/Scene.h"
#include "core/serialization/SceneSerializer.h"

namespace Hachiko
{
    class GameObject;
    class ComponentCamera;
    class ResourcesPreferences;
    class ResourceScene;

    class ModuleSceneManager final : public Module
    {
    public:
        ModuleSceneManager() = default;
        ~ModuleSceneManager() override = default;

        // --- Life cycle --- //
        bool Init() override;
        
        void AttemptScenePause();
        void AttemptScenePlay();
        void AttemptSceneStop();
        bool IsScenePlaying();
        
        UpdateStatus Update(float delta) override;
        bool CleanUp() override;

        GameObject* GetRoot()
        {
            return main_scene->root;
        }

        [[nodiscard]] const GameObject* GetRoot() const
        {
            return main_scene->root;
        }

        Scene* GetActiveScene()
        {
            return main_scene;
        }

        [[nodiscard]] const Scene* GetActiveScene() const
        {
            return main_scene;
        }

        void CreateEmptyScene(const char* name = nullptr);

        void LoadScene(UID new_scene_id);
        void SaveScene(const char* file_path = nullptr);

        GameObject* Raycast(const float3& origin, const float3& destination);
        void SwitchTo(UID new_scene_id);

        void ReloadScene();

        void OptionsMenu();

    private:
        void LoadScene(ResourceScene* scene);
        // Deletes current resource it it doesnt come from resource manager (for now assume it when id 0)
        void SetSceneResource(ResourceScene* scene);
        void RefreshTemporaryScene();
        Scene* main_scene = nullptr;
        SceneSerializer* serializer = nullptr;
        ResourcesPreferences* preferences = nullptr;


        bool scene_ready_to_load = false;
        bool scene_autosave = false;
        ResourceScene* scene_resource;
        ResourceScene* temporary_scene_resource;
        UID scene_to_load_id;
    };
} // namespace Hachiko
