#include "core/hepch.h"
#include "ResourceMaterial.h"
#include "modules/ModuleResources.h"
#include "importers/MaterialImporter.h"

Hachiko::ResourceMaterial::ResourceMaterial(UID uid) :
    Resource(uid, Type::MATERIAL) {}

Hachiko::ResourceMaterial::~ResourceMaterial()
{}

void Hachiko::ResourceMaterial::DrawGui() 
{
    static const ImGuiTreeNodeFlags texture_flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
    ImGui::Text("Name", name, 64);
    if (ImGui::TreeNodeEx((void*)&diffuse, texture_flags, "Diffuse"))
    {
        if (diffuse != nullptr)
        {
            ImGui::Image(reinterpret_cast<void*>(diffuse->GetId()), ImVec2(80, 80));
            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::Text("%dx%d", diffuse->width, diffuse->height);
            ImGui::Text("Path: %s", diffuse->path.c_str());

            // TODO: textue configuration (maybe delegate to the ResourceTexture)

            RemoveTexture(ResourceTexture::Type::DIFFUSE);

            ImGui::EndGroup();
        }
        else
        {
            ImGui::ColorEdit4("Diffuse color", &diffuse_color[0]);
            AddTexture(ResourceTexture::Type::DIFFUSE);
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx((void*)&specular, texture_flags, "Specular"))
    {
        if (specular != nullptr)
        {
            ImGui::Image(reinterpret_cast<void*>(specular->GetId()), ImVec2(80, 80));
            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::Text("%dx%d", specular->width, specular->height);
            ImGui::Text("Path: %s", specular->path.c_str());

            // TODO: textue configuration (maybe delegate to the ResourceTexture)

            RemoveTexture(ResourceTexture::Type::SPECULAR);

            ImGui::EndGroup();
        }
        else
        {
            ImGui::ColorEdit4("Specular color", &specular_color[0]);
            AddTexture(ResourceTexture::Type::SPECULAR);
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx((void*)&normal, texture_flags, "Normal"))
    {
        if (normal != nullptr)
        {
            ImGui::Image(reinterpret_cast<void*>(normal->GetId()), ImVec2(80, 80));
            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::Text("%dx%d", normal->width, normal->height);
            ImGui::Text("Path: %s", normal->path.c_str());

            // TODO: textue configuration (maybe delegate to the ResourceTexture)

            RemoveTexture(ResourceTexture::Type::NORMALS);

            ImGui::EndGroup();
        }
        else
        {
            AddTexture(ResourceTexture::Type::NORMALS);
        }
        ImGui::TreePop();
    }
}

void Hachiko::ResourceMaterial::AddTexture(ResourceTexture::Type type)
{
    const std::string title = StringUtils::Concat("Select texture ", TypeToString(type));
    ResourceTexture* res = nullptr;

    if (ImGui::Button(StringUtils::Concat(TypeToString(type).c_str(), " Texture").c_str()))
    {
        ImGuiFileDialog::Instance()->OpenDialog(
            title.c_str(),
            "Select Texture",
            ".*",
            "./library/textures/",
            1,
            nullptr,
              ImGuiFileDialogFlags_DontShowHiddenFiles 
            | ImGuiFileDialogFlags_DisableCreateDirectoryButton 
            | ImGuiFileDialogFlags_HideColumnType
            | ImGuiFileDialogFlags_HideColumnDate);
    }
    
    if (ImGuiFileDialog::Instance()->Display(title.c_str()))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            const std::filesystem::path texture_path = ImGuiFileDialog::Instance()->GetFilePathName().c_str();
                        
            res = App->resources->GetTexture(texture_path.filename().replace_extension().string().c_str());
        }

        ImGuiFileDialog::Instance()->Close();
    }
    if (res != nullptr)
    {
        SetTexture(res, type);
        UpdateMaterial();
    }
}

void Hachiko::ResourceMaterial::RemoveTexture(ResourceTexture::Type type)
{
    if (ImGui::Button(StringUtils::Concat("Remove " , TypeToString(type).c_str()).c_str()))
    {
        switch (type)
        {
        case ResourceTexture::Type::DIFFUSE:
            diffuse = nullptr;
            break;
        case ResourceTexture::Type::SPECULAR:
            specular = nullptr;
            break;
        case ResourceTexture::Type::NORMALS:
            normal = nullptr;
            break;
        }
    }
}

void Hachiko::ResourceMaterial::UpdateMaterial()
{
    MaterialImporter material_importer;
    material_importer.Save(this);
}
