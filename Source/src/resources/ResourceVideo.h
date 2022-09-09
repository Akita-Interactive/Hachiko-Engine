#pragma once
#include "Resource.h"

namespace Hachiko
{
    class ResourceVideo : public Resource
    {
    public:
        ResourceVideo();
        ResourceVideo(UID id);
        ~ResourceVideo() override;

        void DrawGui();
        const std::filesystem::path& GetVideoPath();
        void SetVideoPath(const std::filesystem::path& path);
    
    private: 
        std::filesystem::path video_path;
    };
} // namespace Hachiko