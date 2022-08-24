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

void Hachiko::MaterialImporter::Import(const char* path, YAML::Node& meta)
{
    // Only 1 material per material asset will exist
    static const int resource_index = 0;
    UID uid = ManageResourceUID(Resource::Type::MATERIAL, resource_index, meta);

    YAML::Node material_node = YAML::LoadFile(path);    
    FileSystem::Save(GetResourcePath(Resource::Type::MATERIAL, uid).c_str(), material_node);
}

void Hachiko::MaterialImporter::Save(UID id, const Resource* res)
{
    // To save material we just update the asset and call the whole asset import pipeline
    const ResourceMaterial* material = static_cast<const ResourceMaterial*>(res);

    GenerateMaterialAssetFile(material);
    const std::string material_asset_path = StringUtils::Concat(GetResourcesPreferences()->GetAssetsPath(Resource::AssetType::MATERIAL), 
        material->GetName().c_str(), MATERIAL_EXTENSION);
    App->resources->ImportAssetFromAnyPath(material_asset_path);
}

Hachiko::Resource* Hachiko::MaterialImporter::Load(UID id)
{
    const std::string material_library_path = GetResourcePath(Resource::Type::MATERIAL, id);

    if (!FileSystem::Exists(material_library_path.c_str()))
    {
        return nullptr;
    }

    YAML::Node node = YAML::LoadFile(material_library_path);
    ResourceMaterial* material = new ResourceMaterial(id);

    material->SetName(node[MATERIAL_NAME].as<std::string>());
    material->diffuse_color = node[MATERIAL_DIFFUSE_COLOR].as<float4>();
    material->specular_color = node[MATERIAL_SPECULAR_COLOR].as<float4>();
    material->emissive_color = node[MATERIAL_EMISSIVE_COLOR].as<float4>();
    material->smoothness = node[MATERIAL_SMOOTHNESS].as<float>();
    material->metalness_value = node[MATERIAL_METALNESS_VALUE].as<float>();
    material->is_metallic = node[MATERIAL_IS_METALLIC].as<unsigned>();
    material->is_transparent = node[MATERIAL_IS_TRANSPARENT].as<unsigned>();

    UID texture_id = node[MATERIAL_DIFFUSE_ID].as<UID>();
    if (texture_id)
    {
        material->diffuse = static_cast<ResourceTexture*>(App->resources->GetResource(Resource::Type::TEXTURE, texture_id));
    }

    texture_id = node[MATERIAL_SPECULAR_ID].as<UID>();
    if (texture_id)
    {
        material->specular = static_cast<ResourceTexture*>(App->resources->GetResource(Resource::Type::TEXTURE, texture_id));
    }

    texture_id = node[MATERIAL_NORMALS_ID].as<UID>();
    if (texture_id)
    {
        material->normal = static_cast<ResourceTexture*>(App->resources->GetResource(Resource::Type::TEXTURE, texture_id));
    }

    texture_id = node[MATERIAL_METALNESS_ID].as<UID>();
    if (texture_id)
    {
        material->metalness = static_cast<ResourceTexture*>(App->resources->GetResource(Resource::Type::TEXTURE, texture_id));
    }

    texture_id = node[MATERIAL_EMISSIVE_ID].as<UID>();
    if (texture_id)
    {
        material->emissive = static_cast<ResourceTexture*>(App->resources->GetResource(Resource::Type::TEXTURE, texture_id));
    }

    return material;
}

Hachiko::UID Hachiko::MaterialImporter::CreateEmptyMaterial(const std::string& name)
{
    // This uid wont be used
    ResourceMaterial* material = new ResourceMaterial(0);
    material->SetName(name);
    GenerateMaterialAssetFile(material);

    const std::string material_path = StringUtils::Concat(GetResourcesPreferences()->GetAssetsPath(Resource::AssetType::MATERIAL), 
        material->GetName().c_str(), MATERIAL_EXTENSION);

    UID material_uid = App->resources->ImportAssetFromAnyPath(material_path)[0];

    delete material;
    return material_uid;
}

Hachiko::UID Hachiko::MaterialImporter::CreateMaterialAssetFromAssimp(const std::string& model_path, aiMaterial* ai_material)
{  
    
    // This uid wont be used
    ResourceMaterial* material = new ResourceMaterial(0);
    std::string name = ai_material->GetName().C_Str();
    if (name.find(':') != std::string::npos)
    {
        name.erase(remove(name.begin(), name.end(), ':'), name.end());
    }
    material->SetName(name);

    const std::string material_path = StringUtils::Concat(GetResourcesPreferences()->GetAssetsPath(Resource::AssetType::MATERIAL), material->GetName(), MATERIAL_EXTENSION);

    if (FileSystem::Exists(material_path.c_str()))
    {
        // Do not overwrite material if its already defined
        UID material_uid = App->resources->ImportAssetFromAnyPath(material_path)[0];
        delete material;
        return material_uid;
    }
    
    aiColor4D color;
    if (ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
    {
        ColorCopy(color, material->diffuse_color);
    }
    if (ai_material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
    {
        ColorCopy(color, material->specular_color);
    }
    if (ai_material->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS)
    {
        ColorCopy(color, material->emissive_color);
    }

    Hachiko::TextureImporter texture_importer;
    material->diffuse = texture_importer.CreateTextureAssetFromAssimp(model_path, ai_material, aiTextureType_DIFFUSE);
    material->specular = texture_importer.CreateTextureAssetFromAssimp(model_path, ai_material, aiTextureType_SPECULAR);
    material->normal = texture_importer.CreateTextureAssetFromAssimp(model_path, ai_material, aiTextureType_NORMALS);
    material->metalness = texture_importer.CreateTextureAssetFromAssimp(model_path, ai_material, aiTextureType_METALNESS);
    material->emissive = texture_importer.CreateTextureAssetFromAssimp(model_path, ai_material, aiTextureType_EMISSIVE);
    GenerateMaterialAssetFile(material);
    
    UID material_uid = App->resources->ImportAssetFromAnyPath(material_path)[0];
    delete material;
    return material_uid;
}

void Hachiko::MaterialImporter::GenerateMaterialAssetFile(const ResourceMaterial* material)
{
    const std::string material_asset_path = StringUtils::Concat(GetResourcesPreferences()->GetAssetsPath(Resource::AssetType::MATERIAL), 
        material->GetName().c_str(), MATERIAL_EXTENSION);
    
    YAML::Node material_node;
    material_node[MATERIAL_NAME] = material->GetName();

    material_node[MATERIAL_DIFFUSE_ID] = (material->HasDiffuse()) ? material->diffuse->GetID() : 0;
    material_node[MATERIAL_SPECULAR_ID] = (material->HasSpecular()) ? material->specular->GetID() : 0;
    material_node[MATERIAL_NORMALS_ID] = (material->HasNormal()) ? material->normal->GetID() : 0;
    material_node[MATERIAL_METALNESS_ID] = (material->HasMetalness()) ? material->metalness->GetID() : 0;
    material_node[MATERIAL_EMISSIVE_ID] = (material->HasEmissive()) ? material->emissive->GetID() : 0;

    material_node[MATERIAL_DIFFUSE_COLOR] = material->diffuse_color;
    material_node[MATERIAL_SPECULAR_COLOR] = material->specular_color;
    material_node[MATERIAL_EMISSIVE_COLOR] = material->emissive_color;
    material_node[MATERIAL_SMOOTHNESS] = material->smoothness;
    material_node[MATERIAL_METALNESS_VALUE] = material->metalness_value;
    material_node[MATERIAL_IS_METALLIC] = material->is_metallic;
    material_node[MATERIAL_IS_TRANSPARENT] = material->is_transparent;
    FileSystem::Save(material_asset_path.c_str(), material_node);
}
