#include "core/hepch.h"

#include "GLFont.h"
#include <fstream>

GLFont::GLFont(const char* fontFile, FT_Library& ft) {
    // Initialize FreeType

    /* _error = FT_Init_FreeType(&_ft);
    if(_error) {
        throw std::exception("Failed to initialize FreeType");
    }*/
    setFontFile(fontFile, ft);
}

GLFont::~GLFont() {}


void GLFont::setFontFile(const char* fontFile, FT_Library& ft)
{
    _fontFile = (char*)fontFile;

    // Create a new font
    _error = FT_New_Face(ft,       // FreeType instance handle
                         _fontFile, // Font family to use
                         0,         // index of font (in case there are more than one in the file)
                         &_face);   // font face handle

    if(_error == FT_Err_Unknown_File_Format) {
        throw std::exception("Failed to open font: unknown font format");
    }
    else if(_error) {
        throw std::exception("Failed to open font");
    }
}

FT_Face GLFont::getFaceHandle() {
    return _face;
}