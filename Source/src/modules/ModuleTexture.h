#pragma once
#include "Module.h"

#include <il.h>
#include <string>

#include "ft2build.h"
#include "freetype.h"
#include "GLfont.h"

namespace Hachiko
{
    class ResourceTexture;
    
    struct Texture // TODO: removed
    {
        bool loaded = false;
        unsigned id = 0;
        std::string path;
        unsigned width = 0;
        unsigned height = 0;
    };

    struct TextureCube
    {
        enum class Side
        {
            RIGHT,
            LEFT,
            // Order between top and bottom is swaped from opengl
            // Because we flip the y to correct not properly flipped resources
            BOTTOM,
            TOP,            
            CENTER,
            BACK,
            COUNT
        };

        static std::string SideString(Side side);
        bool loaded = false;
        unsigned id{};
        UID uids[static_cast<unsigned>(Side::COUNT)] = {0};
        ResourceTexture* resources[static_cast<unsigned>(Side::COUNT)] = {nullptr};
    };

    struct Font
    {
        bool loaded = false;
        std::string path;
        std::shared_ptr<GLFont> gl_font = nullptr;
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
        static TextureCube LoadCubeMap(TextureCube& cube);
        static void Bind(unsigned id, unsigned slot);
        static void Unbind(unsigned slot);

        Font LoadFont(const char* path);

        [[nodiscard]] short GetDevilVersion() const
        {
            return devil_version;
        }

        void OptionsMenu() const;

        static ::byte* GetData();
        static unsigned int LoadImg(const char* path, bool flip = true);
        static void DeleteImg(unsigned& img_id);

    private:
        const short devil_version = IL_VERSION;        

        FT_Library freetype_lib;
    };
} // namespace Hachiko