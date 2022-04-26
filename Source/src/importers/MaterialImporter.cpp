#include "core/hepch.h"

#include "MaterialImporter.h"

#include "modules/ModuleResources.h"
#include "modules/ModuleTexture.h"

#include "resources/ResourceMaterial.h"
#include "importers/TextureImporter.h"
#include "core/preferences/src/ResourcesPreferences.h"

void ColorCopy(const aiColor4D& assimp_color, float4& color)
{
    color.x = assimp_color.r;
    color.y = assimp_color.g;
    color.z = assimp_color.b;
    color.w = assimp_color.a;
}

Hachiko::MaterialImporter::MaterialImporter() : Importer(Importer::Type::MATERIAL)
{
}

void Hachiko::MaterialImporter::Import(const char* path)
{
    HE_LOG("Entering MaterialImporter: %s", path);
    Assimp::Importer import;
    //import.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
    const aiScene* scene = nullptr;
    const std::filesystem::path material_path(path);
    const std::string model_name = material_path.filename().replace_extension().string();
    const std::string material_output_path = GetResourcesPreferences()->GetAssetsPath(Resource::Type::MATERIAL) + model_name + MATERIAL_EXTENSION;
    scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        HE_LOG("ERROR::ASSIMP::%c", import.GetErrorString());
        return;
    }

    YAML::Node material_node;
    for (unsigned i = 0; i < scene->mNumMaterials; ++i)
    {
        aiMaterial* material = scene->mMaterials[i];
        Hachiko::UID material_id = UUID::GenerateUID();
        material_node[MODEL_MATERIAL_NODE][i][MODEL_MATERIAL_ID] = material_id;
        Import(material, material_id);
    }

    FileSystem::Save(material_output_path.c_str(), material_node);
}

void Hachiko::MaterialImporter::Save(const Resource* res) 
{
    const ResourceMaterial* material = static_cast<const ResourceMaterial*>(res);
    const std::string material_library_path = GetResourcesPreferences()->GetAssetsPath(Resource::Type::MATERIAL) + material->GetName();

    YAML::Node material_node;
    material_node[MATERIAL_ID] = material->GetID();
    material_node[GENERIC_TYPE] = static_cast<int>(material->GetType());
    material_node[MATERIAL_NAME] = material->GetName();

    material_node[MATERIAL_DIFFUSE_NAME] = (material->HasDiffuse()) ? material->diffuse->GetName() : std::string();
    material_node[MATERIAL_SPECULAR_NAME] = (material->HasSpecular()) ? material->specular->GetName() : std::string();
    material_node[MATERIAL_NORMALS_NAME] = (material->HasNormal()) ? material->normal->GetName() : std::string();

    material_node[MATERIAL_DIFFUSE_PATH] = (material->HasDiffuse()) ? material->diffuse->GetAssetPath() : std::string();
    material_node[MATERIAL_SPECULAR_PATH] = (material->HasSpecular()) ? material->specular->GetAssetPath() : std::string();
    material_node[MATERIAL_NORMALS_PATH] = (material->HasNormal()) ? material->normal->GetAssetPath() : std::string();

    material_node[MATERIAL_DIFFUSE_COLOR] = material->diffuse_color;
    material_node[MATERIAL_SPECULAR_COLOR] = material->specular_color;
    material_node[MATERIAL_SHININESS] = material->shininess;

    FileSystem::Save(material_library_path.c_str(), material_node);
}

Hachiko::Resource* Hachiko::MaterialImporter::Load(const char* material_path)
{
    if (!std::filesystem::exists(material_path))
    {
        return nullptr;
    }

    YAML::Node node = YAML::LoadFile(material_path);
    Hachiko::ResourceMaterial* material = new ResourceMaterial(node[MATERIAL_ID].as<UID>());

    material->SetName(node[MATERIAL_NAME].as<std::string>());
    material->diffuse_color = node[MATERIAL_DIFFUSE_COLOR].as<float4>();
    material->specular_color = node[MATERIAL_SPECULAR_COLOR].as<float4>();
    material->shininess = node[MATERIAL_SHININESS].as<float>();

    std::string diffuse_path = node[MATERIAL_DIFFUSE_PATH].as<std::string>();
    std::string specular_path = node[MATERIAL_SPECULAR_PATH].as<std::string>();
    std::string normals_path = node[MATERIAL_NORMALS_PATH].as<std::string>();
    
    material->diffuse = App->resources->GetTexture(node[MATERIAL_DIFFUSE_NAME].as<std::string>(), diffuse_path);
    material->specular = App->resources->GetTexture(node[MATERIAL_SPECULAR_NAME].as<std::string>(), specular_path);
    material->normal = App->resources->GetTexture(node[MATERIAL_NORMALS_NAME].as<std::string>(), normals_path);

    return material;
}

void Hachiko::MaterialImporter::CreateMaterial(const std::string& name) 
{
    ResourceMaterial* material = new ResourceMaterial(Hachiko::UUID::GenerateUID());
    material->SetName(name);

    Save(material);

    delete material;
}

void Hachiko::MaterialImporter::Import(aiMaterial* ai_material, const UID& id) 
{
    if (!id)
    {
        const_cast<UID&>(id) = UUID::GenerateUID();
    }
    
    const auto material = new ResourceMaterial(id);
    material->SetName(ai_material->GetName().C_Str());
    
    aiColor4D color;
    if (ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
    {
        ColorCopy(color, material->diffuse_color);
    }
    if (ai_material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
    {
        ColorCopy(color, material->specular_color);
    }

    ai_material->Get(AI_MATKEY_SHININESS, material->shininess);

    material->diffuse = ImportTexture(ai_material, aiTextureType_DIFFUSE);
    material->specular = ImportTexture(ai_material, aiTextureType_SPECULAR);
    material->normal = ImportTexture(ai_material, aiTextureType_NORMALS);

    Save(material);

    delete material->diffuse;
    delete material->specular;
    delete material->normal;
    delete material;
}

Hachiko::ResourceTexture* Hachiko::MaterialImporter::ImportTexture(const aiMaterial* ai_material, aiTextureType type)
{
    static const int index = 0;
    aiString file;
    aiReturn ai_ret = ai_material->GetTexture(type, index, &file); 
    if ( ai_ret == AI_FAILURE || ai_ret == AI_OUTOFMEMORY)
    {
        return nullptr;
    }

    ResourceTexture* output_texture = nullptr;
    Hachiko::TextureImporter texture_importer;
    const std::string model_path = App->resources->GetLastResourceLoadedPath().u8string();
    const char* asset_path = App->preferences->GetResourcesPreference()->GetAssetsPath(Resource::Type::TEXTURE);
    std::vector<std::string> search_paths;
    const std::string model_texture_path(file.data);
    const std::string filename = model_texture_path.substr(model_texture_path.find_last_of("/\\") + 1);
    
    search_paths.emplace_back(asset_path + filename);
    search_paths.emplace_back(model_path + "\\" + filename);
    search_paths.emplace_back(file.data);

    for (std::string& path : search_paths)
    {
        output_texture = static_cast<ResourceTexture*>(texture_importer.ImportTexture(path.c_str()));
        if (output_texture == nullptr)
        {
            continue;
        }
            
        if (path != (asset_path + filename))
        {
            std::filesystem::copy_file(path.c_str(), (asset_path + filename),
                std::filesystem::copy_options::skip_existing);
            output_texture->SetAssetPath(asset_path + filename);
        }

        break;
    }

    return output_texture;
}
