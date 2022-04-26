#pragma once

#include "glew.h"
#include "ft2build.h"
#include FT_FREETYPE_H

class GLFont {
public:
    GLFont(const char* fontFile, FT_Library& ft);
    ~GLFont();

    void setFontFile(const char* fontFile, FT_Library& ft);

    FT_Face getFaceHandle();

private:
    char* _fontFile;
    FT_Error _error;
    FT_Face _face;

};

