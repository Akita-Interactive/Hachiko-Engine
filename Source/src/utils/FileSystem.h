#pragma once

namespace Hachiko
{
    class PathNode;

    class FileSystem
    {
    public:
        FileSystem();
        ~FileSystem();

        bool Init();

        // --- CRUD --- //
        static void CreateDir(const char* directory_path);
        static char* Load(const char* file_path);
        static bool Save(const char* file_path, const void* buffer, unsigned int size, bool append = false);
        static bool Save(const char* path, const YAML::Node& node);
        static void Delete(const char* file_path);

        // --- Folder utils --- //
        static bool Copy(const char* source_file_path, const char* destination_file_path, bool fail_if_exist = false);
        static std::string NormalizePath(const char* full_path);
        static bool Exists(const char* file_path);
        static bool IsDirectory(const char* directory_path);

        // --- File utils --- //
        static void SplitFilePath(const char* full_path, std::string* path, std::string* file = nullptr, std::string* extension = nullptr);
        static std::string GetFileNameAndExtension(const char* file_path);
        static std::string GetFileExtension(const char* file_path);
        static bool HasExtension(const char* path);
        static bool HasExtension(const char* path, std::vector<std::string> extensions);

        // --- Path utils --- //
        static PathNode GetAllFiles(const char* directory, std::vector<std::string>* filter_ext, std::vector<std::string>* ignore_ext);
        static void DiscoverFiles(const char* directory, std::vector<std::string>& file_list, std::vector<std::string>& dir_list);

        const std::string& GetWorkingDirectory() const
        {
            return working_directory;
        }

        const std::wstring& GetWorkingDirectoryW() const
        {
            return working_directory_w;
        }

    private:
        void CreateContext();
        std::string working_directory;
        std::wstring working_directory_w;
    };
}
