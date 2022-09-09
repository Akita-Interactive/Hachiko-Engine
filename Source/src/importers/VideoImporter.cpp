#include "core/hepch.h"

#include "VideoImporter.h"
#include "resources/ResourceVideo.h"

void Hachiko::VideoImporter::Import(const char* path, YAML::Node& meta)
{
    static const int resource_index = 0;
    UID uid = ManageResourceUID(Resource::Type::TEXTURE, resource_index, meta);
    const std::string library_path = StringUtils::Concat(App->preferences->GetResourcesPreference()->GetLibraryPath(Resource::Type::VIDEO), std::to_string(uid));
    FileSystem::Copy(path, library_path.c_str());
    return;
}

Hachiko::Resource* Hachiko::VideoImporter::Load(UID id)
{
    ResourceVideo* res = new ResourceVideo(id);
    res->SetVideoPath(
        StringUtils::Concat(App->preferences->GetResourcesPreference()->GetLibraryPath(Resource::Type::VIDEO),
            std::to_string(id).c_str()));
    return res;
}

void Hachiko::VideoImporter::Save(UID id, const Resource* resource)
{
}
