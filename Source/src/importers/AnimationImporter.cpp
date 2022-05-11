#include "core/hepch.h"
#include "AnimationImporter.h"

#include "core/preferences/src/ResourcesPreferences.h"

#include "resources/ResourceAnimation.h"

Hachiko::AnimationImporter::AnimationImporter() : Importer(Importer::Type::ANIMATION) 
{}

void Hachiko::AnimationImporter::Import(const char* path, YAML::Node& meta) 
{
    HE_LOG("Entering Animation Importer: %s", path);
    Assimp::Importer import;

    const aiScene* scene = nullptr;
    const std::filesystem::path material_path(path);

    scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    assert(scene->mNumAnimations == 1);

    Import(scene->mAnimations[0], meta[GENERAL_NODE][GENERAL_ID].as<UID>());
}

void Hachiko::AnimationImporter::ImportWithMeta(const char* path, YAML::Node& meta) 
{
    Import(path, meta);
}

void Hachiko::AnimationImporter::Save(const Resource* resource) 
{
    const ResourceAnimation* animation = static_cast<const ResourceAnimation*>(resource);
    const std::string file_path = StringUtils::Concat(GetResourcesPreferences()->GetLibraryPath(Resource::Type::ANIMATION), std::to_string(resource->GetID()));

    unsigned file_size = 0;

    unsigned header[3] = {
        animation->GetDuration(),
        animation->GetName().length(),
        animation->channels.size()
    };
    file_size += sizeof(header) + animation->GetName().length();

    std::vector<unsigned> second_header(3 * animation->channels.size());
    unsigned i = 0;
    for (auto it = animation->channels.begin(); it != animation->channels.end(); ++it)
    {
        second_header[i] = it->first.length();
        second_header[i + 1] = it->second.num_positions;
        second_header[i + 2] = it->second.num_rotations;
        file_size += it->first.length() + sizeof(float3) * it->second.num_positions + sizeof(Quat) * it->second.num_rotations;
        i += 3;
    }
    file_size += sizeof(unsigned) * second_header.size();
    
    char* file_buffer = new char[file_size];
    char* cursor = file_buffer;
    unsigned size_bytes = 0;

    size_bytes = sizeof(header);
    memcpy(cursor, header, size_bytes);
    cursor += size_bytes;

    size_bytes = sizeof(unsigned) * second_header.size();
    memcpy(cursor, &second_header[0], size_bytes);
    cursor += size_bytes;

    size_bytes = animation->GetName().length();
    memcpy(cursor, animation->GetName().c_str(), size_bytes);
    cursor += size_bytes;

    for (auto it = animation->channels.begin(); it != animation->channels.end(); ++it)
    {
        size_bytes = it->first.length();
        memcpy(cursor, it->first.c_str(), size_bytes);
        cursor += size_bytes;

        size_bytes = sizeof(float3) * it->second.num_positions;
        memcpy(cursor, it->second.positions.get(), size_bytes);
        cursor += size_bytes;

        size_bytes = sizeof(Quat) * it->second.num_rotations;
        memcpy(cursor, it->second.rotations.get(), size_bytes);
        cursor += size_bytes;
    }
    
    FileSystem::Save(file_path.c_str(), file_buffer, file_size);
    delete[] file_buffer;
}

Hachiko::Resource* Hachiko::AnimationImporter::Load(UID id)
{
    assert(id && "Unable to load module. Given an empty id");

    const std::string file_path = StringUtils::Concat(GetResourcesPreferences()->GetLibraryPath(Resource::Type::ANIMATION), std::to_string(id));

    char* file_buffer = FileSystem::Load(file_path.c_str());
    char* cursor = file_buffer;
    unsigned size_bytes = 0;

    const auto animation = new ResourceAnimation(id);

    unsigned header[3];
    size_bytes = sizeof(header);
    memcpy(header, cursor, size_bytes);
    cursor += size_bytes;

    //header[0] duration
    //header[1] nameLength
    //header[2] numChanels


    animation->SetDuration(header[0]);

    std::vector<unsigned> second_header(header[2] * 3);
    size_bytes = header[2] * 3 * sizeof(unsigned);
    memcpy(&second_header[0], cursor, size_bytes);
    cursor += size_bytes;

    size_bytes = header[1];
    std::string name = "";
    for (unsigned z = 0; z < size_bytes; ++z)
        name += cursor[z];
    cursor += size_bytes;

    animation->SetName(name.c_str());

    animation->channels.reserve(header[2]);
    for (unsigned i = 0; i < header[2] * 3; i += 3)
    {
        size_bytes = second_header[i];
        std::string chanel_name = "";
        for (unsigned j = 0; j < size_bytes; ++j)
            chanel_name += cursor[j];
        cursor += size_bytes;

        ResourceAnimation::Channel& channel = animation->channels[chanel_name];
        channel.num_positions = second_header[i + 1];
        channel.num_rotations = second_header[i + 2];
        channel.positions = std::make_unique<float3[]>(channel.num_positions);
        channel.rotations = std::make_unique<Quat[]>(channel.num_rotations);

        size_bytes = sizeof(float3) * channel.num_positions;
        memcpy(channel.positions.get(), cursor, size_bytes);
        cursor += size_bytes;

        size_bytes = sizeof(Quat) * channel.num_rotations;
        memcpy(channel.rotations.get(), cursor, size_bytes);
        cursor += size_bytes;
    }

    delete[] file_buffer;

    return animation;
}

void Hachiko::AnimationImporter::Import(const aiAnimation* animation, UID id)
{

    unsigned int first = 0;
    unsigned int last = UINT_MAX;

    float scale = 1.0f; // TODO: This should be setable by param or maybe inspector var

    const auto r_animation = new ResourceAnimation(id);

    r_animation->SetName(animation->mName.C_Str());

    r_animation->SetDuration(unsigned(1000 * animation->mDuration / animation->mTicksPerSecond));

    r_animation->channels.reserve(animation->mNumChannels);

    for (unsigned i = 0; i < animation->mNumChannels; ++i)
    {
        const aiNodeAnim* node = animation->mChannels[i];
        ResourceAnimation::Channel& channel = r_animation->channels[std::string(node->mNodeName.C_Str())];

        unsigned int pos_first = 0;
        unsigned int pos_last = 1;

        unsigned int rot_first = 0;
        unsigned int rot_last = 1;

        if (node->mNumPositionKeys > 1)
        {
            pos_first = std::min(first, node->mNumPositionKeys);
            pos_last = std::max(pos_first, std::min(last, node->mNumPositionKeys));
        }

        if (node->mNumRotationKeys > 1)
        {
            rot_first = std::min(first, node->mNumRotationKeys);
            rot_last = std::max(rot_first, std::min(last, node->mNumRotationKeys));
        }

        channel.num_positions = pos_last - pos_first;
        channel.num_rotations = rot_last - rot_first;

        channel.positions = std::make_unique<float3[]>(channel.num_positions);
        channel.rotations = std::make_unique<Quat[]>(channel.num_rotations);

        for (unsigned j = 0; j < (pos_last - pos_first); ++j)
        {
            channel.positions[j] = (*reinterpret_cast<math::float3*>(&node->mPositionKeys[j + pos_first].mValue)) * scale;
        }

        for (unsigned j = 0; j < (rot_last - rot_first); ++j)
        {
            const aiQuaternion& quat = node->mRotationKeys[j + rot_first].mValue;
            channel.rotations[j] = Quat(quat.x, quat.y, quat.z, quat.w);
        }
    }

    Save(r_animation);

    delete r_animation;
}