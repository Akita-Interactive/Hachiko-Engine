#include "core/hepch.h"
#include "FontImporter.h"

#include "modules/ModuleResources.h"
#include "resources/ResourceFont.h"
#include "ImporterManager.h"

Hachiko::FontImporter::FontImporter()
{
    // Initialize fonts library
    if (FT_Init_FreeType(&freetype_lib))
    {
        assert(0 && "Failed to load FreeType library.");
    }
}

Hachiko::FontImporter::~FontImporter()
{
    // Release fonts library
    FT_Done_FreeType(freetype_lib);
}

void Hachiko::FontImporter::Import(const char* path, YAML::Node& meta)
{
    // Only 1 font will exist
    static const int resource_index = 0;
    UID font_uid = ManageResourceUID(Resource::Type::FONT, resource_index, meta);
    ResourceFont* font = new ResourceFont();
    font->gl_font = std::make_unique<GLFont>(path, freetype_lib);

    if (font != nullptr)
    {
        Save(font_uid, font);
    }
    delete font;
}

Hachiko::Resource* Hachiko::FontImporter::Load(UID id)
{
    const std::string file_path = GetResourcePath(Resource::Type::ANIMATION, id) +".ttf";
    if (!std::filesystem::exists(file_path))
    {
        return nullptr;
    }
    ResourceFont* font = new ResourceFont(id);
    font->gl_font = std::make_unique<GLFont>(file_path.c_str(), freetype_lib);

    return font;
}

void Hachiko::FontImporter::Save(UID id, const Resource* resource)
{
    const ResourceFont* font = static_cast<const ResourceFont*>(resource);
    // Freetype is dumb and needs the correct extension to work
    const std::string lib_font_path = GetResourcePath(Resource::Type::ANIMATION, id) + ".ttf";

    // Just use the original font file in lib
    FileSystem::Copy(font->gl_font->getFontFile(), lib_font_path.c_str());
}
