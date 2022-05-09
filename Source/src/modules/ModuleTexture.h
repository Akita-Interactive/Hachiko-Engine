#pragma once
#include "Module.h"

#include <il.h>
#include <string>

namespace Hachiko
{
    struct Texture // TODO: removed
    {
        bool loaded = false;
        unsigned id = 0;
        std::string path;
        unsigned width = 0;
        unsigned height = 0;
    };

    struct TextureCube // TODO: removed
    {
        bool loaded = false;
        unsigned id{};
        unsigned widths[6]{};
        unsigned heighths[6]{};

        TextureCube() = default;
        TextureCube(const TextureCube& other) = default;
        TextureCube& operator=(const TextureCube& other) =  default;
    };
    
    class ResourceTexture;

    class ModuleTexture final : public Module
    {
    public:
        ModuleTexture();
        ~ModuleTexture() override;

        bool Init() override;
        bool CleanUp() override;

        static ResourceTexture* ImportTextureResource(UID uid, const char* path, bool flip = true);

        [[deprecated]]
        static Texture Load(const char* path, bool flip = true);
        [[deprecated]]
        static void Unload(Texture& texture);
        static TextureCube LoadCubeMap(const char* paths[6]);
        static void Bind(unsigned id, unsigned slot);
        static void Unbind(unsigned slot);

        [[nodiscard]] short GetDevilVersion() const
        {
            return devil_version;
        }

        void OptionsMenu() const;

    private:
        const short devil_version = IL_VERSION;
        static unsigned int LoadImg(const char* path, bool flip = true);
        static void DeleteImg(unsigned& img_id);
    };
}
