#include "core/hepch.h"
#include "TextureImporter.h"

#include "core/preferences/src/ResourcesPreferences.h"

#include "resources/ResourceTexture.h"
#include "modules/ModuleTexture.h"
#include "modules/ModuleResources.h"

Hachiko::TextureImporter::TextureImporter() 
	: Importer(Importer::Type::TEXTURE)
{
}

void Hachiko::TextureImporter::Import(const char* path)
{
    Hachiko::UID uid = Hachiko::UUID::GenerateUID();
    Resource* texture = ImportTexture(path, uid);
    delete texture;
}

Hachiko::Resource* Hachiko::TextureImporter::Load(const char* path)
{
    const std::filesystem::path file_path(path);
    if (!std::filesystem::exists(file_path))
    {
        return nullptr;
    }

    char* file_buffer = FileSystem::Load(file_path.string().c_str());
    char* cursor = file_buffer;
    unsigned size_bytes = 0;

    auto texture = new ResourceTexture();
    texture->SetName(file_path.filename().string());
    unsigned header[9];
    size_bytes = sizeof(header);
    memcpy(header, cursor, size_bytes);
    cursor += size_bytes;

    unsigned path_size = header[0];
    texture->width = header[1];
    texture->height = header[2];
    texture->format = header[3];
    texture->bpp = header[4];
    texture->min_filter = header[5];
    texture->mag_filter = header[6];
    texture->wrap = header[7];
    texture->data_size = header[8];

    size_bytes = path_size;
    texture->path = "";
    for (unsigned i = 0; i < size_bytes; ++i)
    {
        texture->path += cursor[i];
    }
    cursor += size_bytes;

    size_bytes = texture->data_size;
    texture->data = new byte[size_bytes];
    memcpy(texture->data, cursor, size_bytes);
    cursor += size_bytes;

    texture->GenerateBuffer();

    delete[] file_buffer;

    return texture;
}

void Hachiko::TextureImporter::Save(const Hachiko::Resource* res)
{
    const ResourceTexture* texture = static_cast<const ResourceTexture*>(res);
    const std::string file_path = StringUtils::Concat(GetResourcesPreferences()->GetLibraryPath(Resource::Type::TEXTURE), texture->GetName());

    unsigned header[9] = {
        texture->path.length(),
        texture->width,
        texture->height,
        texture->format,
        texture->bpp,
        texture->min_filter,
        texture->mag_filter,
        texture->wrap,
        texture->data_size
    };

    unsigned file_size = 0;
    file_size += sizeof(header);
    file_size += texture->path.length();
    file_size += texture->data_size;

    const auto file_buffer = new char[file_size];
    char* cursor = file_buffer;
    unsigned size_bytes = 0;

    size_bytes = sizeof(header);
    memcpy(cursor, header, size_bytes);
    cursor += size_bytes;

    size_bytes = texture->path.length();
    memcpy(cursor, texture->path.c_str(), size_bytes);
    cursor += size_bytes;

    size_bytes = texture->data_size;
    memcpy(cursor, texture->data, size_bytes);
    cursor += size_bytes;

    FileSystem::Save(file_path.c_str(), file_buffer, file_size);
    delete[] file_buffer;
}

Hachiko::Resource* Hachiko::TextureImporter::ImportTexture(const char* path, UID id)
{
    Hachiko::UID uid = id;
    if (!id)
    {
        uid = Hachiko::UUID::GenerateUID();
    }

    // TODO: texture configuration to flip so this will not be needed
    std::string str_path = std::string(path);
    int extension_index = str_path.rfind('.');
    std::string extension = str_path.substr(extension_index + 1, str_path.length() - (extension_index + 1));
    //

    ResourceTexture* texture = App->texture->ImportResource(uid, path, extension != "tif"); // TODO: could we extract ModuleTexture functionality to this importer?

    if (texture != nullptr)
    {
        Save(texture);
    }

    return texture;
}
