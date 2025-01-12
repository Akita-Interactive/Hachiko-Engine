#include "core/hepch.h"
#include "ModelImporter.h"
#include "MeshImporter.h"
#include "MaterialImporter.h"
#include "AnimationImporter.h"
#include "PrefabImporter.h"

#include "core/preferences/src/ResourcesPreferences.h"
#include "modules/ModuleResources.h"
#include "modules/ModuleSceneManager.h"
#include "components/ComponentMeshRenderer.h"
#include "components/ComponentAnimation.h"
#include "components/ComponentTransform.h"

#include "resources/ResourceAnimation.h"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

void Hachiko::ModelImporter::Import(const char* path, YAML::Node& meta)
{
    HE_LOG("Entering ModelImporter: %s", path);
    Assimp::Importer import;
    import.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
    const aiScene* scene = nullptr;

    const std::filesystem::path model_path(path);
    meta[MODEL_NAME] = model_path.filename().replace_extension().string();

    unsigned flags = aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality | aiProcess_LimitBoneWeights
                     | aiProcess_SplitLargeMeshes | aiProcess_Triangulate | aiProcess_GenUVCoords | aiProcess_SortByPType | aiProcess_FindDegenerates | aiProcess_FindInvalidData 
                     | aiProcess_GlobalScale;
    
    scene = import.ReadFile(path, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        HE_LOG("ERROR::ASSIMP::%c", import.GetErrorString());
        return;
    }

    ImportModel(path, scene, meta);
}



void Hachiko::ModelImporter::ImportModel(const char* path, const aiScene* scene, YAML::Node& meta)
{
    Hachiko::MeshImporter mesh_importer;
    Hachiko::MaterialImporter material_importer;
    Hachiko::AnimationImporter animation_importer;
    PrefabImporter prefab_importer;
    
    meta.remove(RESOURCES);
    auto resource_ids = std::vector<UID>();
    auto resource_types = std::vector<Resource::Type>();

    bool has_bones = false;
    for (unsigned mesh_index = 0; mesh_index < scene->mNumMeshes; ++mesh_index)
    {
        if (scene->mMeshes[mesh_index]->mNumBones > 0)
        {
            has_bones = true;
        }
    }
    for (unsigned mesh_index = 0; mesh_index < scene->mNumMeshes; ++mesh_index)
    {
        Hachiko::UID mesh_id;
        if (ExistsInMetaArray(meta[MESHES], mesh_index))
        {
            mesh_id = meta[MESHES][mesh_index].as<UID>();
        }
        else
        {
            mesh_id = UUID::GenerateUID();
            meta[MESHES][mesh_index] = mesh_id;
        }
        aiMesh* mesh = scene->mMeshes[mesh_index];
        // Create mesh from assimp and since its not a separate asset manage its id.
        mesh_importer.ImportFromAssimp(mesh_id, mesh);
        resource_ids.push_back(mesh_id);
        resource_types.push_back(Resource::Type::MESH);
    }

    for (unsigned material_index = 0; material_index < scene->mNumMaterials; ++material_index)
    {
        // Model importer decides theuid of new materials to keep them under control
        Hachiko::UID material_id;
        aiMaterial* material = scene->mMaterials[material_index];
        // Create material from assimp and generate it as an asset, if it already exists its data is refreshed
        UID material_uid = material_importer.CreateMaterialAssetFromAssimp(path, material);
        // Since it is its own asset we dont decide its id
        meta[MATERIALS][material_index] = material_uid;
        resource_ids.push_back(material_uid);
        resource_types.push_back(Resource::Type::MATERIAL);
    }

    if (scene->HasAnimations())
    {
        for (unsigned animation_index = 0; animation_index < scene->mNumAnimations; ++animation_index)
        {
            Hachiko::UID animaiton_id;
            if (ExistsInMetaArray(meta[ANIMATIONS], animation_index))
            {
                animaiton_id = meta[ANIMATIONS][animation_index].as<UID>();
            }
            else
            {
                animaiton_id = UUID::GenerateUID();
                meta[ANIMATIONS][animation_index] = animaiton_id;
            }

            
            aiAnimation* animation = scene->mAnimations[animation_index];

            meta[ANIMATION_NAMES][animation_index] = animation->mName.C_Str();
            // Create animation from assimp, since its not a separate asset manage its id
            animation_importer.CreateAnimationFromAssimp(animation, animaiton_id);
            resource_ids.push_back(animaiton_id);
            resource_types.push_back(Resource::Type::ANIMATION);
        }
    }

    std::string filename = FileSystem::GetFileName(path);

    // Import scene tree into gameobjects
    GameObject* model_root = new GameObject(scene->mRootNode->mName.C_Str());

    // If it has bones we dont want to combine intermediate nodes since they are used for animation
    ImportNode(model_root, scene, scene->mRootNode, meta, !has_bones);

    // Import animations
    if (meta[ANIMATIONS].IsDefined())
    {
        ComponentAnimation* animation = static_cast<ComponentAnimation*>(model_root->children[0]->CreateComponent(Component::Type::ANIMATION));
    }

    // Create prefab
    UID prefab_uid = meta[PREFAB_ID].IsDefined() ? meta[PREFAB_ID].as<UID>() : UUID::GenerateUID();
    prefab_importer.CreateGeneratedPrefab(filename.c_str(), prefab_uid, model_root->children[0]);
    App->scene_manager->RemoveGameObject(model_root);
    meta[PREFAB_ID] = prefab_uid;

    for (unsigned i = 0; i < resource_ids.size(); ++i)
    {
        AddResource(resource_ids[i], resource_types[i], meta);
    }    
    AddResource(prefab_uid, Resource::Type::PREFAB, meta);

    // Remove extra resources if the amount of resources has decreased
}

void Hachiko::ModelImporter::ImportNode(GameObject* parent, const aiScene* scene ,const aiNode* assimp_node, YAML::Node& meta, bool combine_intermediate_nodes)
{
    std::string node_name = assimp_node->mName.C_Str();

    aiVector3D aiTranslation, aiScale;
    aiQuaternion aiRotation;
    assimp_node->mTransformation.Decompose(aiScale, aiRotation, aiTranslation);
    float3 pos(aiTranslation.x, aiTranslation.y, aiTranslation.z);
    Quat rot(aiRotation.x, aiRotation.y, aiRotation.z, aiRotation.w);
    float3 scale(aiScale.x, aiScale.y, aiScale.z);

    float4x4 transform = float4x4::FromTRS(pos, rot, scale);

    bool dummy_node = true;
    while (dummy_node)
    {
        dummy_node = false;

        if (combine_intermediate_nodes && node_name.find(AUXILIAR_NODE) != std::string::npos && assimp_node->mNumChildren == 1)
        {
            assimp_node = assimp_node->mChildren[0];
            assimp_node->mTransformation.Decompose(aiScale, aiRotation, aiTranslation);

            pos = float3(aiTranslation.x, aiTranslation.y, aiTranslation.z);
            rot = Quat(aiRotation.x, aiRotation.y, aiRotation.z, aiRotation.w);
            scale = float3(aiScale.x, aiScale.y, aiScale.z);

            transform = transform * float4x4::FromTRS(pos, rot, scale);;

            node_name = assimp_node->mName.C_Str();
            dummy_node = true;
        }
    }

    // Create go
    GameObject* go = new GameObject(parent, assimp_node->mName.C_Str());
    MeshImporter mesh_importer;
    MaterialImporter material_importer;

    // Load resources as components    
    for (unsigned int j = 0; j < assimp_node->mNumMeshes; ++j)
    {
        unsigned mesh_idx = assimp_node->mMeshes[j];
        const aiMesh* assimp_mesh = scene->mMeshes[assimp_node->mMeshes[j]];
        unsigned material_idx = assimp_mesh->mMaterialIndex;
        
        ComponentMeshRenderer* mesh_renderer = static_cast<ComponentMeshRenderer*>(go->CreateComponent(Component::Type::MESH_RENDERER));
        ResourceMesh* mesh_resource = static_cast<ResourceMesh*>(App->resources->GetResource(Resource::Type::MESH, meta[MESHES][mesh_idx].as<UID>()));
        ResourceMaterial* material_resource = static_cast<ResourceMaterial*>(App->resources->GetResource(Resource::Type::MATERIAL, meta[MATERIALS][material_idx].as<UID>()));
        mesh_renderer->SetMeshResource(mesh_resource);     
        mesh_renderer->SetMaterialResource(material_resource);
    }

    go->GetTransform()->SetLocalTransform(transform);

    for (unsigned i = 0; i < assimp_node->mNumChildren; ++i)
    {
        auto child_node = assimp_node->mChildren[i];
        ImportNode(go, scene, child_node, meta, combine_intermediate_nodes);
    }    
}