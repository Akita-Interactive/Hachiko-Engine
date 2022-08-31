#pragma once

#include "Window.h"

namespace Hachiko
{
    struct Entry
    {
        bool is_directory = false;
        std::filesystem::path path;
        std::string name;
    };

    class WindowResource final : public Window
    {

    public:
        WindowResource();
        ~WindowResource() override = default;
        void Init() override;
        void Update() override;
        static void Reload();
    private:
        void LoadAsset(const std::string& path);
        void CreateMaterialGui(const ImVec2& button_size);
        void UpdateDirectoryInfo();

        std::string auxiliary_name;
        std::filesystem::path current_path;
        std::vector<Entry> current_content;
        std::filesystem::path root_directory;
    };
}
