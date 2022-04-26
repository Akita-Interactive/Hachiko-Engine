#include "core/hepch.h"
#include "ResourceTexture.h"

Hachiko::ResourceTexture::ResourceTexture()
	: Resource(Resource::Type::TEXTURE)
{}

Hachiko::ResourceTexture::~ResourceTexture() 
{
    if (id != 0)
        glDeleteTextures(1, &id);
    delete[] data;
}

void Hachiko::ResourceTexture::GenerateBuffer() 
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
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
    ImGui::Image(reinterpret_cast<void*>(id), ImVec2(100, 100));

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

    // TODO: select config
    ImGui::Text("Min filter: ");
    ImGui::SameLine();
    ImGui::Text(std::to_string(min_filter).c_str());
    // TODO: select config
    ImGui::Text("Mag filter: ");
    ImGui::SameLine();
    ImGui::Text(std::to_string(mag_filter).c_str());
    // TODO: select config
    ImGui::Text("BPP: ");
    ImGui::SameLine();
    ImGui::Text(std::to_string(bpp).c_str());
}
