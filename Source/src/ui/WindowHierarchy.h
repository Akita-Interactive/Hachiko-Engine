#pragma once
#include "Window.h"

namespace Hachiko
{
    class GameObject;

    class WindowHierarchy final : public Window
    {
    public:
        WindowHierarchy();
        ~WindowHierarchy() override;

        void Update() override;

        void GOSelected(GameObject* game_object);

    private:
        void DrawHierarchyTree(const GameObject* game_object);
        bool RecursiveDraw(GameObject* game_object);
        bool DrawGameObject(GameObject* game_object, bool stop);
        bool DragAndDrop(GameObject* game_object);


        GameObject* dragged_object = nullptr;

        bool collapse = false;

        // Auxiliar variables for opening the hierarchy to see the selected object
        int index = 0;
        std::vector<GameObject*> selected_object_ancestors;
    };
}
