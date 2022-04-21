#include "core/hepch.h"

#include "ImporterManager.h"

#include "MeshImporter.h"
#include "ModelImporter.h"
#include "TextureImporter.h"
#include "MaterialImporter.h"
#include "AnimationImporter.h"

using namespace Hachiko;

ImporterManager::ImporterManager()
{
    const auto mesh = new MeshImporter();
    const auto model = new ModelImporter();
    const auto texture = new TextureImporter();
    const auto material = new MaterialImporter();
    const auto animation = new AnimationImporter();

    importers.reserve(static_cast<size_t>(Importer::Type::COUNT));
    importers.push_back(std::make_pair(model->GetType(), model));
    importers.push_back(std::make_pair(mesh->GetType(), mesh));
    importers.push_back(std::make_pair(texture->GetType(), texture));
    importers.push_back(std::make_pair(material->GetType(), material));
    importers.push_back(std::make_pair(animation->GetType(), animation));
}

ImporterManager::~ImporterManager()
{
    for (auto it : importers)
    {
        delete it.second;
    }
}

void ImporterManager::Import(const std::filesystem::path& asset_path, const Resource::Type asset_type)
{
    assert(!asset_path.empty() && "Module Import abort - Given an empty asset path");
    YAML::Node meta = CreateMeta(asset_path.string().c_str());
    GetImporter(asset_type)->Import(asset_path.string().c_str(), meta);

    std::string meta_path = StringUtils::Concat(asset_path.parent_path().string(), "\\",
        asset_path.filename().replace_extension(META_EXTENSION).string());
    FileSystem::Save(meta_path.c_str(), meta);
}

Resource* Hachiko::ImporterManager::Load(Resource::Type type, const char* path)
{
    return GetImporter(type)->Load(path);
}

bool Hachiko::ImporterManager::IsImported(const char* path, Resource::Type type) const
{
    return GetImporter(type)->IsImported(path);
}

Importer* Hachiko::ImporterManager::GetImporter(Resource::Type type) const
{
    auto it = std::find_if(importers.begin(), importers.end(), [&](const std::pair<Importer::Type, Importer*>& element)
        { 
            return element.first == ToImporterType(type);
        });

    assert(it != importers.end());
    return it->second;
}

Importer::Type ImporterManager::ToImporterType(const Resource::Type type) const
{
    Importer::Type iType = Importer::Type::COUNT;
    switch (type)
    {
    case Resource::Type::MODEL:
        iType = Importer::Type::MODEL;
        break;

    case Resource::Type::MESH:
        iType = Importer::Type::MESH;
        break;

    case Resource::Type::TEXTURE:
        iType = Importer::Type::TEXTURE;
        break;

    case Resource::Type::MATERIAL:
        iType = Importer::Type::MATERIAL;
        break;
    case Resource::Type::ANIMATION:
        iType = Importer::Type::ANIMATION;
        break;
    case Resource::Type::SCENE:
        iType = Importer::Type::SCENE;
        break;
    case Resource::Type::AUDIO:
    case Resource::Type::VIDEO:
    case Resource::Type::SCRIPT:
    case Resource::Type::UNKNOWN:
        // TODO: This is a hack. We need to implement our own assert with message
        assert(false && "Unhandled resource type");
    }

    return iType;
}

YAML::Node Hachiko::ImporterManager::CreateMeta(const char* path)
{
    YAML::Node node;
    std::string file_timestamp;
    std::string file_path(path);
    GetFileLastWriteTimestamp(StringUtils::StringToWString(file_path), file_timestamp);

    node["General"]["id"] = UUID::GenerateUID();
    node["General"]["path"] = file_path;
    node["General"]["timestamp"] = file_timestamp;

    return node;
}
