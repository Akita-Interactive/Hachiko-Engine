#include "core/hepch.h"
#include "ModuleTexture.h"

#include "resources/ResourceTexture.h"
#include "modules/ModuleResources.h"

#include "GLFont.h"
#include "FTLabel.h"

Hachiko::ModuleTexture::ModuleTexture() = default;

Hachiko::ModuleTexture::~ModuleTexture() = default;

bool Hachiko::ModuleTexture::Init()
{
    // Initialize image library
    ilInit();

    // Initialize fonts library
    if (FT_Init_FreeType(&freetype_lib))
    {
        HE_LOG("Failed to load FreeType library.");
        return false;
    }

    return true;
}

bool Hachiko::ModuleTexture::CleanUp()
{
    // Release image library
    ilShutDown();

    // Release fonts library
    FT_Done_FreeType(freetype_lib);

    return true;
}

Hachiko::ResourceTexture* Hachiko::ModuleTexture::ImportTextureResource(UID uid, const char* path, bool flip)
{
    std::filesystem::path texture_path = path;

    unsigned int img_id = LoadImg(path, flip);

    if (img_id == 0)
    {
        return nullptr;
    }
 
    ResourceTexture* texture = new ResourceTexture(uid);
    texture->path = path;
    texture->SetName(texture_path.filename().replace_extension().string().c_str());
    texture->min_filter = GL_LINEAR_MIPMAP_LINEAR;
    texture->mag_filter = GL_LINEAR;
    texture->wrap = GL_CLAMP;

    texture->bpp = ilGetInteger(IL_IMAGE_BPP);
    texture->width = ilGetInteger(IL_IMAGE_WIDTH);
    texture->height = ilGetInteger(IL_IMAGE_HEIGHT);
    texture->format = ilGetInteger(IL_IMAGE_FORMAT);

    unsigned char* data = ilGetData();
    texture->data_size = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
    texture->data = new unsigned char[texture->data_size];
    memcpy(texture->data, data, texture->data_size);

    DeleteImg(img_id);

    return texture;
}

Hachiko::Texture Hachiko::ModuleTexture::Load(const char* path, bool flip)
{
    Texture texture;
    texture.path = path;
    unsigned int img_id = LoadImg(path, flip);

    if (img_id != 0)
    {
        glGenTextures(1, &texture.id);
        glBindTexture(GL_TEXTURE_2D, texture.id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     ilGetInteger(IL_IMAGE_BPP),
                     texture.width = ilGetInteger(IL_IMAGE_WIDTH),
                     texture.height = ilGetInteger(IL_IMAGE_HEIGHT),
                     0,
                     ilGetInteger(IL_IMAGE_FORMAT),
                     GL_UNSIGNED_BYTE,
                     ilGetData());
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
        DeleteImg(img_id);
        texture.loaded = true; // False by default
    }
    return texture;
}

void Hachiko::ModuleTexture::Unload(Texture& texture)
{
    if (texture.loaded)
    {
        glDeleteTextures(1, &texture.id);
        texture.loaded = false;
    }
}

Hachiko::TextureCube Hachiko::ModuleTexture::LoadCubeMap(TextureCube& cube)
{
    constexpr bool flip = true;
    cube.loaded = true;

    glGenTextures(1, &cube.id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cube.id);

    // Expected file order x, -x, y, -y, z, -z
    for (unsigned i = 0; i < static_cast<unsigned>(TextureCube::Side::COUNT); ++i)
    {
        cube.resources[i] = static_cast<ResourceTexture*>(App->resources->GetResource(Resource::Type::TEXTURE, cube.uids[i]));
        
        ResourceTexture* cube_face = cube.resources[i];
        if (!cube_face)
        {
            cube.loaded = false;
            continue; // Try loading the other parts despite failing
        }

        //ilBindImage(cube.resources[i]->GetImageId());
        // iluFlipImage();
        // Take advantage of opengl enum with index
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0, cube_face->format, cube_face->width, cube_face->height,//0, cube_face->bpp, cube_face->width, cube_face->height,
                     0, cube_face->format, GL_UNSIGNED_BYTE, cube_face->data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return cube;
}

void Hachiko::ModuleTexture::Bind(unsigned id, unsigned slot)
{
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id);
}

void Hachiko::ModuleTexture::Unbind(unsigned slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Hachiko::Font Hachiko::ModuleTexture::LoadFont(const char* path)
{
    Font font;
    font.path = path;
    try
    {
        font.gl_font = std::shared_ptr<GLFont>(new GLFont(path, freetype_lib));
        font.loaded = true;

        return font;
    }
    catch (std::exception& e)
    {
        // Catch exception and return unloaded font if fails
        return font;
    }
}

void SetOption(unsigned option, unsigned value)
{
    glTexParameteri(GL_TEXTURE_2D, option, value);
}

void Hachiko::ModuleTexture::OptionsMenu() const
{
    const char* labels_mag[] = {"Linear", "Nearest"};
    const unsigned values_mag[] = {GL_LINEAR, GL_NEAREST};
    const char* labels_min[]
        = {"Nearest", "Linear", "Nearest Mipmaps Nearest Criteria", "Nearest Mipmap Linear Criteria", "Linear Mipmaps (Two Closest) Nearest Criteria", "Linear Mipmaps (Two Closest) Linear Criteria"};
    const unsigned values_min[] = {GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR};
    static int mag_filter = 0; // Default is GL_LINEAR
    static int min_filter = 3; // Default is GL_NEAREST_MIPMAP_LINEAR

    if (ImGui::Combo("Mag Filter", &mag_filter, labels_mag, IM_ARRAYSIZE(labels_mag)))
        SetOption(GL_TEXTURE_MAG_FILTER, values_mag[mag_filter]);
    if (ImGui::Combo("Min Filter", &min_filter, labels_min, IM_ARRAYSIZE(labels_min)))
        SetOption(GL_TEXTURE_MIN_FILTER, values_min[min_filter]);
}

unsigned int Hachiko::ModuleTexture::LoadImg(const char* path, bool flip)
{
    ILuint img_id; // The image name to return.
    ilGenImages(1, &img_id); // Grab a new image name.
    ilBindImage(img_id);
    if (!ilLoadImage(path))
        return 0;
    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    if (flip)
        iluFlipImage();
    return img_id;
}

::byte* Hachiko::ModuleTexture::GetData()
{
    return ilGetData();
}

void Hachiko::ModuleTexture::DeleteImg(unsigned& img_id)
{
    ilDeleteImages(1, &img_id);
}

std::string Hachiko::TextureCube::SideString(Side side)
{
    enum class Side
    {
        RIGHT,
        LEFT,
        TOP,
        BOTTOM,
        CENTER,
        BACK,
        COUNT
    };

    switch (side)
    {
    case TextureCube::Side::RIGHT :
        return std::string("right") + CUBE_ID;
    case TextureCube::Side::LEFT:
        return std::string("left") + CUBE_ID;
    case TextureCube::Side::TOP:
        return std::string("top") + CUBE_ID;
    case TextureCube::Side::BOTTOM:
        return std::string("bottom") + CUBE_ID;
    case TextureCube::Side::CENTER:
        return std::string("center") + CUBE_ID;
    case TextureCube::Side::BACK:
        return std::string("back") + CUBE_ID;
    }
    return std::string("unknown") + CUBE_ID;
}
