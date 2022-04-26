#include "core/hepch.h"
#include "AnimationImporter.h"

#include "core/preferences/src/ResourcesPreferences.h"

#include "resources/ResourceAnimation.h"

Hachiko::AnimationImporter::AnimationImporter() : Importer(Importer::Type::ANIMATION) {}

void Hachiko::AnimationImporter::Import(const char* path) {}

void Hachiko::AnimationImporter::Save(const Resource* resource) {}

Hachiko::Resource* Hachiko::AnimationImporter::Load(const char* model_path)
{
    return nullptr;
}

bool Hachiko::AnimationImporter::IsImported(const char* path)
{
    return false;
}

Hachiko::ResourceAnimation* Hachiko::AnimationImporter::Import(const aiAnimation* animation)
{
    const auto r_mesh = new ResourceAnimation(UUID::GenerateUID());

    // Convert to milliseconds
    r_mesh->SetDuration(unsigned(1000 * animation->mDuration / animation->mTicksPerSecond));

    r_mesh->channels.reserve(animation->mNumChannels);

    for (unsigned i = 0; i < animation->mNumChannels; ++i)
    {
        const aiNodeAnim* node = animation->mChannels[i];
        ResourceAnimation::Channel& channel = r_mesh->channels[std::string(node->mNodeName.C_Str())];

        unsigned int pos_first = 0;
        unsigned int pos_last = 1;

        unsigned int rot_first = 0;
        unsigned int rot_last = 1;

        if (node->mNumPositionKeys > 1)
        {
            pos_first = node->mNumPositionKeys;
            pos_last = std::max(pos_first, node->mNumPositionKeys);
        }

        if (node->mNumRotationKeys > 1)
        {
            rot_first = node->mNumRotationKeys;
            rot_last = std::max(rot_first, node->mNumRotationKeys);
        }

        channel.num_positions = pos_last - pos_first;
        channel.num_rotations = rot_last - rot_first;

        channel.positions = std::make_unique<float3[]>(channel.num_positions);
        channel.rotations = std::make_unique<Quat[]>(channel.num_rotations);

        for (unsigned j = 0; j < (pos_last - pos_first); ++j)
        {
            channel.positions[j] = (*reinterpret_cast<math::float3*>(&node->mPositionKeys[j + pos_first].mValue)) * 1.0f;
        }

        for (unsigned j = 0; j < (rot_last - rot_first); ++j)
        {
            const aiQuaternion& quat = node->mRotationKeys[j + rot_first].mValue;
            channel.rotations[j] = Quat(quat.x, quat.y, quat.z, quat.w);
        }
    }

    return r_mesh;
}