#include "core/hepch.h"
#include "ResourceTexture.h"
#include "modules/ModuleSceneManager.h"
#include "modules/ModuleResources.h"
#include "importers/TextureImporter.h"

#include "core/Scene.h"

constexpr size_t wrap_modes_count = 4;
constexpr unsigned int wrap_modes_enum[wrap_modes_count] 
{
    GL_REPEAT, 
    GL_MIRRORED_REPEAT, 
    GL_CLAMP, 
    GL_CLAMP_TO_BORDER
};
constexpr const char* wrap_modes_string[wrap_modes_count] 
{
    "Repeat", 
    "Mirrored Repeat", 
    "Clamp", 
    "Clamp To Border"
};

constexpr size_t min_filters_count = 6;
constexpr size_t mag_filters_count = 2;
constexpr unsigned int filters_enum[min_filters_count] 
{
    GL_LINEAR, // Be sure this is zeroeth index
    GL_NEAREST, // Be sure this is at the first index
    GL_LINEAR_MIPMAP_NEAREST,
    GL_NEAREST_MIPMAP_NEAREST,
    GL_NEAREST_MIPMAP_LINEAR,
    GL_LINEAR_MIPMAP_LINEAR
};
constexpr const char* filters_string[min_filters_count] 
{
    "Linear",
    "Nearest",
    "Linear Mipmap Nearest",
    "Nearest Mipmap Nearest",
    "Nearest Mipmap Linear",
    "Linear Mipmap Linear"
};

Hachiko::ResourceTexture::ResourceTexture()
	: Resource(Resource::Type::TEXTURE)
{
    App->scene_manager->RebuildBatches();
}

Hachiko::ResourceTexture::ResourceTexture(UID id) 
    : Resource(id, Resource::Type::TEXTURE)
{
    App->scene_manager->RebuildBatches();
}

Hachiko::ResourceTexture::~ResourceTexture() 
{
    if (texture_id != 0)
        glDeleteTextures(1, &texture_id);
    delete[] data;
}

void Hachiko::ResourceTexture::GenerateBuffer() 
{
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

    glTexImage2D(GL_TEXTURE_2D, 0, bpp, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Hachiko::ResourceTexture::DrawGui() 
{
    ImGui::Text(path.c_str());
    ImGui::Image(reinterpret_cast<void*>(texture_id), ImVec2(100, 100));

    ImGui::Text("Width: ");
    ImGui::SameLine();
    ImGui::Text(std::to_string(width).c_str());
    ImGui::Text("Height: ");
    ImGui::SameLine();
    ImGui::Text(std::to_string(height).c_str());
    ImGui::Text("Format: ");
    ImGui::SameLine();
    ImGui::Text(std::to_string(format).c_str());
    ImGui::Text("BPP: ");
    ImGui::SameLine();
    ImGui::Text(std::to_string(bpp).c_str());

    ImGui::NewLine();

    // Wrap Mode Configuration:
    ImGui::TextWrapped("Wrapping");
    ImGui::Separator();
    unsigned int previous_wrap = wrap;
    ImGui::PushID(&wrap);
    for (size_t i = 0; i < wrap_modes_count; ++i)
    {
        if (ImGui::RadioButton(wrap_modes_string[i], wrap == wrap_modes_enum[i]))
        {
            wrap = wrap_modes_enum[i];
        }
    }
    ImGui::PopID();

    ImGui::NewLine();

    // Filter Mode Configuration:
    ImGui::TextWrapped("Filtering");
    ImGui::Separator();
    
    ImGui::TextWrapped("Min Filtering");
    unsigned int previous_min = min_filter;
    ImGui::PushID(&min_filter);
    for (size_t i = 0; i < min_filters_count; ++i)
    {
        if (ImGui::RadioButton(filters_string[i], min_filter == filters_enum[i]))
        {
            min_filter = filters_enum[i];
        }
    }
    ImGui::PopID();

    ImGui::NewLine();

    ImGui::TextWrapped("Mag Filtering");
    unsigned int previous_mag = mag_filter;
    ImGui::PushID(&mag_filter);
    for (size_t i = 0; i < mag_filters_count; ++i)
    {
        if (ImGui::RadioButton(filters_string[i], mag_filter == filters_enum[i]))
        {
            mag_filter = filters_enum[i];
        }
    }
    ImGui::PopID();

    ImGui::NewLine();

    if (wrap != previous_wrap || previous_min != min_filter || previous_mag != mag_filter)
    {
        // Notify Batch Manager that there is a change so it can rebuild the batches:
        App->scene_manager->RebuildBatches();
        // Save the texture if there is a change:
        TextureImporter texture_importer;
        texture_importer.SaveTextureAsset(this);
    }
}
