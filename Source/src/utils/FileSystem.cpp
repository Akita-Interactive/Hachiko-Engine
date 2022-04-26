#include "core/hepch.h"
#include "FileSystem.h"
#include "PathNode.h"

Hachiko::FileSystem::FileSystem()
{
    HE_LOG("Creating virtual file system");
    // Create base path to be ready for other modules
    char* base_path = SDL_GetBasePath();
    PHYSFS_init(base_path);
    SDL_free(base_path);

    PHYSFS_mount(".", nullptr, 0);

    PHYSFS_setWriteDir(".");
}

Hachiko::FileSystem::~FileSystem()
{
    PHYSFS_deinit();
}

bool Hachiko::FileSystem::Init()
{
    HE_LOG("Init virtual file system");

    CreateContext();

    char* write_path = SDL_GetPrefPath("Akita", "Hachiko");
    SDL_free(write_path);
    return true;
}

void Hachiko::FileSystem::CreateContext()
{
    char engine_path[MAX_PATH];
    wchar_t engine_path_w[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, engine_path);
    GetCurrentDirectoryW(MAX_PATH, engine_path_w);
    working_directory = engine_path;
    working_directory_w = engine_path_w;


    HE_LOG("Engine context: %s", working_directory.c_str());
}

char* Hachiko::FileSystem::Load(const char* file_path)
{
    char* buffer = nullptr;

    PHYSFS_file* fs_file = PHYSFS_openRead(file_path);

    if (!fs_file)
    {
        HE_LOG("Error opening file %s (%s).\n", fs_file, PHYSFS_getLastError());
        return nullptr;
    }
    defer {
        PHYSFS_close(fs_file);
    };

    const auto size = static_cast<PHYSFS_sint32>(PHYSFS_fileLength(fs_file));

    if (size < 0)
    {
        HE_LOG("File size couldn't be determined for %s (%s).\n", fs_file, PHYSFS_getLastError());
        return nullptr;
    }

    buffer = new char[size];
    const PHYSFS_sint64 read = PHYSFS_read(fs_file, buffer, 1, size);

    if (read < size)
    {
        HE_LOG("Error reading file %s (%s).\n", fs_file, PHYSFS_getLastError());
        return nullptr;
    }

    return buffer;
}

bool Hachiko::FileSystem::Save(const char* file_path, const void* buffer, unsigned int size, bool append)
{
    const bool overwrite = PHYSFS_exists(file_path) != 0;
    PHYSFS_File* file = append ? PHYSFS_openAppend(file_path) : PHYSFS_openWrite(file_path);

    if (!file)
    {
        HE_LOG("Error saving file %s (%s).\n", file_path, strerror(errno));
        return false;
    }
    defer {
        PHYSFS_close(file);
    };

    const PHYSFS_sint64 written = PHYSFS_write(file, buffer, 1, size);
    if (written != size)
    {
        HE_LOG("File System error while writing to file %s: %s", file, PHYSFS_getLastError());
        return false;
    }
    if (append == true)
    {
        HE_LOG("Added %u data to [%s%s]", size, PHYSFS_getWriteDir(), file);
    }
    else if (overwrite == false)
    {
        HE_LOG("New file created [%s%s] of %u bytes", PHYSFS_getWriteDir(), file, size);
    }
    return true;
}

bool Hachiko::FileSystem::Save(const char* path, const YAML::Node& node)
{
    std::ofstream output(path);
    output << node;
    output.close();
    return true;
}

bool Hachiko::FileSystem::Exists(const char* file_path)
{
    return PHYSFS_exists(file_path) != 0 && !std::string(file_path).empty();
}

bool Hachiko::FileSystem::IsDirectory(const char* directory_path)
{
    return PHYSFS_isDirectory(directory_path) != 0;
}

void Hachiko::FileSystem::CreateDir(const char* directory_path)
{
    if (!Exists(directory_path) && !PHYSFS_mkdir(directory_path))
    {
        HE_LOG(PHYSFS_getLastError());
    }
}

bool Hachiko::FileSystem::Copy(const char* source_file_path, const char* destination_file_path, bool fail_if_exist)
{
    return CopyFile(source_file_path, destination_file_path, fail_if_exist);
}

void Hachiko::FileSystem::Delete(const char* file_path)
{
    if (!PHYSFS_delete(file_path))
    {
        HE_LOG("Can't erase file %s. (%s)\n", file_path, PHYSFS_getLastError());
    }
}

std::string Hachiko::FileSystem::NormalizePath(const char* full_path)
{
    std::string newPath(full_path);
    for (char& i : newPath)
    {
        if (i == '\\')
        {
            i = '/';
        }
    }
    return newPath;
}

std::string Hachiko::FileSystem::GetFileNameAndExtension(const char* file_path)
{
    const char* last_slash = strrchr(file_path, '/');
    const char* last_backslash = strrchr(file_path, '\\');
    const char* last_separator = last_slash >= last_backslash ? last_slash : last_backslash;

    if (last_separator == nullptr)
    {
        return file_path;
    }

    const char* name_extension = last_separator + 1;
    return name_extension;
}

std::string Hachiko::FileSystem::GetFileExtension(const char* file_path)
{
    const char* last_slash = strrchr(file_path, '/');
    const char* last_backslash = strrchr(file_path, '\\');
    const char* last_separator = last_slash >= last_backslash ? last_slash : last_backslash;

    const char* lastDot = strrchr(file_path, '.');

    auto extension = std::string(lastDot);
    std::transform(extension.begin(), extension.end(), extension.begin(), tolower);

    return extension;
}

Hachiko::PathNode Hachiko::FileSystem::GetAllFiles(const char* directory, std::vector<std::string>* filter_ext, std::vector<std::string>* ignore_ext)
{
    PathNode root;
    if (Exists(directory))
    {
        return root;
    }

    root.path = directory;
    SplitFilePath(directory, nullptr, &root.localPath);
    if (root.localPath.empty())
    {
        root.localPath = directory;
    }

    std::vector<std::string> file_list, dir_list;
    DiscoverFiles(directory, file_list, dir_list);

    //Adding all child directories
    for (auto& i : dir_list)
    {
        std::string str = directory;
        str.append("/").append(i);
        root.children.push_back(GetAllFiles(str.c_str(), filter_ext, ignore_ext));
    }
    //Adding all child files
    for (auto& i : file_list)
    {
        //Filtering extensions
        bool filter = true, discard = false;
        if (filter_ext != nullptr)
        {
            filter = HasExtension(i.c_str(), *filter_ext);
        }
        if (ignore_ext != nullptr)
        {
            discard = HasExtension(i.c_str(), *ignore_ext);
        }
        if (filter == true && discard == false)
        {
            std::string str = directory;
            str.append("/").append(i);
            root.children.push_back(GetAllFiles(str.c_str(), filter_ext, ignore_ext));
        }
    }
    root.isFile = HasExtension(root.path.c_str());
    root.isLeaf = root.children.empty() == true;
    return root;
}

void Hachiko::FileSystem::SplitFilePath(const char* full_path, std::string* path, std::string* file, std::string* extension)
{
    if (full_path == nullptr || path == nullptr || file == nullptr || extension == nullptr)
    {
        return;
    }

    const std::string full(full_path);
    const size_t pos_separator = full.find_last_of("\\/");
    const size_t pos_dot = full.find_last_of('.');
    if (pos_separator < full.length())
    {
        *path = full.substr(0, pos_separator + 1);
    }
    else
    {
        path->clear();
    }

    if (pos_separator < full.length())
    {
        *file = full.substr(pos_separator + 1, pos_dot - pos_separator - 1);
    }
    else
    {
        *file = full.substr(0, pos_dot);
    }

    if (pos_dot < full.length())
    {
        *extension = full.substr(pos_dot + 1);
    }
    else
    {
        extension->clear();
    }
}

void Hachiko::FileSystem::DiscoverFiles(const char* directory, std::vector<std::string>& file_list, std::vector<std::string>& dir_list)
{
    char** rc = PHYSFS_enumerateFiles(directory);

    for (char** i = rc; *i != nullptr; i++)
    {
        std::string str = std::string(directory) + std::string("/") + std::string(*i);
        if (IsDirectory(str.c_str()))
        {
            dir_list.emplace_back(*i);
        }
        else
        {
            file_list.emplace_back(*i);
        }
    }

    PHYSFS_freeList(rc);
}

bool Hachiko::FileSystem::HasExtension(const char* path)
{
    std::string ext;
    SplitFilePath(path, nullptr, nullptr, &ext);
    return !ext.empty();
}

bool Hachiko::FileSystem::HasExtension(const char* path, std::vector<std::string> extensions)
{
    std::string ext;
    SplitFilePath(path, nullptr, nullptr, &ext);
    if (ext.empty())
    {
        return true;
    }
    
    for (auto& extension : extensions)
    {
        if (extension == ext)
        {
            return true;
        }
    }
    return false;
}
