#include "core/hepch.h"
#include "ResourceVideo.h"

Hachiko::ResourceVideo::ResourceVideo() 
	: Resource(Resource::Type::VIDEO)
{
}

Hachiko::ResourceVideo::ResourceVideo(UID id)
	: Resource(id, Resource::Type::VIDEO)
{
}

Hachiko::ResourceVideo::~ResourceVideo() 
{
}

void Hachiko::ResourceVideo::DrawGui() 
{
}

const std::filesystem::path& Hachiko::ResourceVideo::GetVideoPath()
{
    return video_path;
}

void Hachiko::ResourceVideo::SetVideoPath(const std::filesystem::path& path)
{
	video_path = path;
}
