#include "core/hepch.h"
#include "ResourceMaterial.h"
#include "modules/ModuleResources.h"
#include "modules/ModuleSceneManager.h"
#include "importers/MaterialImporter.h"

Hachiko::ResourceMaterial::ResourceMaterial(UID uid) : Resource(uid, Type::MATERIAL)
{
    App->scene_manager->GetActiveScene();
}

Hachiko::ResourceMaterial::~ResourceMaterial() {}

void Hachiko::ResourceMaterial::DrawGui()
{
    static const ImGuiTreeNodeFlags texture_flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
    ImGui::Text("Material: %s", name.c_str());

    ImGuiUtils::Combo("Material Type", material_types, is_metallic);
    if (!is_metallic)
    {
        ImGui::TextDisabled("Keep in mind that the specular workflow is not fully suported!");
    }

    bool was_transparent = is_transparent;
    ImGuiUtils::Combo("Transparency", transparency, is_transparent);

    if (was_transparent != is_transparent)
    {
        // If the transparency property of a material is changed, notify
        // scene to arrange batching again:
        App->scene_manager->RebuildBatches();
    }

    if (ImGui::TreeNodeEx((void*)&diffuse, texture_flags, "Diffuse"))
    {
        if (diffuse != nullptr)
        {
            ImGui::Image(reinterpret_cast<void*>(diffuse->GetImageId()), ImVec2(80, 80));
            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::Text("%dx%d", diffuse->width, diffuse->height);
            ImGui::Text("Path: %s", diffuse->path.c_str());

            if (ImGui::CollapsingHeader("Resource Diffuse Texture")) 
            {
                diffuse->DrawGui();
            }

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
    if (is_metallic)
    {
        if (ImGui::TreeNodeEx((void*)&metalness, texture_flags, "Metalness"))
        {
            if (metalness != nullptr)
            {
                ImGui::Image(reinterpret_cast<void*>(metalness->GetImageId()), ImVec2(80, 80));
                ImGui::SameLine();
                ImGui::BeginGroup();
                ImGui::Text("%dx%d", metalness->width, metalness->height);
                ImGui::Text("Path: %s", metalness->path.c_str());

                if (ImGui::CollapsingHeader("Resource Metalness Texture"))
                {
                    metalness->DrawGui();
                }

                RemoveTexture(ResourceTexture::Type::METALNESS);

                ImGui::EndGroup();
            }
            else
            {
                ImGui::SliderFloat("Metalness", &metalness_value, 0.0f, 1.0f, "%.2f", 1.0f);
                AddTexture(ResourceTexture::Type::METALNESS);
            }
            ImGui::SliderFloat("Smoothness", &smoothness, 0.0f, 1.0f, "%.2f", 1.0f);

            alpha_channels[1] = material_types[is_metallic];
            ImGuiUtils::Combo("Alpha", alpha_channels, smoothness_alpha);

            ImGui::TreePop();
        }
    }
    else
    {
        if (ImGui::TreeNodeEx((void*)&specular, texture_flags, "Specular"))
        {
            if (specular != nullptr)
            {
                ImGui::Image(reinterpret_cast<void*>(specular->GetImageId()), ImVec2(80, 80));
                ImGui::SameLine();
                ImGui::BeginGroup();
                ImGui::Text("%dx%d", specular->width, specular->height);
                ImGui::Text("Path: %s", specular->path.c_str());

                // TODO: textue configuration (maybe delegate to the ResourceTexture)
                if (ImGui::CollapsingHeader("Resource Specular Texture"))
                {
                    specular->DrawGui();
                }

                RemoveTexture(ResourceTexture::Type::SPECULAR);

                ImGui::EndGroup();
            }
            else
            {
                ImGui::ColorEdit4("Specular color", &specular_color[0]);
                AddTexture(ResourceTexture::Type::SPECULAR);
            }
            ImGui::SliderFloat("Smoothness", &smoothness, 0.0f, 1.0f, "%.2f", 1.0f);

            alpha_channels[1] = material_types[is_metallic];
            ImGuiUtils::Combo("Alpha", alpha_channels, smoothness_alpha);

            ImGui::TreePop();
        }
    }
    if (ImGui::TreeNodeEx((void*)&normal, texture_flags, "Normal"))
    {
        if (normal != nullptr)
        {
            ImGui::Image(reinterpret_cast<void*>(normal->GetImageId()), ImVec2(80, 80));
            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::Text("%dx%d", normal->width, normal->height);
            ImGui::Text("Path: %s", normal->path.c_str());

            // TODO: textue configuration (maybe delegate to the ResourceTexture)
            if (ImGui::CollapsingHeader("Resource Normal Texture"))
            {
                normal->DrawGui();
            }

            RemoveTexture(ResourceTexture::Type::NORMALS);

            ImGui::EndGroup();
        }
        else
        {
            AddTexture(ResourceTexture::Type::NORMALS);
        }
        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx((void*)&emissive, texture_flags, "Emissive"))
    {
        if (emissive != nullptr)
        {
            ImGui::Image(reinterpret_cast<void*>(emissive->GetImageId()), ImVec2(80, 80));
            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::Text("%dx%d", emissive->width, emissive->height);
            ImGui::Text("Path: %s", emissive->path.c_str());

            // TODO: textue configuration (maybe delegate to the ResourceTexture)
            if (ImGui::CollapsingHeader("Resource Emissive Texture"))
            {
                emissive->DrawGui();
            }

            ImGui::ColorEdit4("Emissive color", &emissive_color[0]);
            RemoveTexture(ResourceTexture::Type::EMISSIVE);

            ImGui::EndGroup();
        }
        else
        {
            ImGui::ColorEdit4("Emissive color", &emissive_color[0]);
            AddTexture(ResourceTexture::Type::EMISSIVE);
        }
        ImGui::TreePop();
    }
    if (ImGui::SmallButton("Save"))
    {
        UpdateMaterial();
    }
}

void Hachiko::ResourceMaterial::SetTexture(ResourceTexture* res, ResourceTexture::Type type)
{
    switch (type)
    {
    case ResourceTexture::Type::DIFFUSE:
        //App->resources->ReleaseResource(diffuse);
        diffuse = res;
        break;
    case ResourceTexture::Type::SPECULAR:
        //App->resources->ReleaseResource(specular);
        specular = res;
        break;
    case ResourceTexture::Type::NORMALS:
        //App->resources->ReleaseResource(normal);
        normal = res;
        break;
    case ResourceTexture::Type::METALNESS:
        //App->resources->ReleaseResource(metalness);
        metalness = res;
        break;
    case ResourceTexture::Type::EMISSIVE:
        //App->resources->ReleaseResource(emissive);
        emissive = res;
        break;
    }
}

std::string Hachiko::ResourceMaterial::TypeToString(ResourceTexture::Type type)
{
    switch (type)
    {
    case ResourceTexture::Type::DIFFUSE:
        return "Diffuse";
    case ResourceTexture::Type::SPECULAR:
        return "Specular";
    case ResourceTexture::Type::NORMALS:
        return "Normals";
    case ResourceTexture::Type::METALNESS:
        return "Metalness";
    case ResourceTexture::Type::EMISSIVE:
        return "Emissive";
    }
}

void Hachiko::ResourceMaterial::AddTexture(ResourceTexture::Type type)
{
    const std::string title = StringUtils::Concat("Select texture ", TypeToString(type)) + "##" + name;
    const char* filters = "Image files{.png,.tif,.jpg,.tga}";

    if (ImGui::Button(StringUtils::Concat(TypeToString(type).c_str(), " Texture").c_str()))
    {
        ImGuiFileDialog::Instance()->OpenDialog(title.c_str(),
                                                "Select Texture",
                                                filters,
                                                "./assets/textures/",
                                                1,
                                                nullptr,
                                                ImGuiFileDialogFlags_DontShowHiddenFiles | ImGuiFileDialogFlags_DisableCreateDirectoryButton | ImGuiFileDialogFlags_HideColumnType
                                                    | ImGuiFileDialogFlags_HideColumnDate);
    }

    if (ImGuiFileDialog::Instance()->Display(title.c_str()))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string texture_path = ImGuiFileDialog::Instance()->GetFilePathName();
            texture_path.append(META_EXTENSION);

            YAML::Node texture_node = YAML::LoadFile(texture_path);
            ResourceTexture* res = static_cast<ResourceTexture*>(App->resources->GetResource(Resource::Type::TEXTURE, texture_node[RESOURCES][0][RESOURCE_ID].as<UID>()));

            if (res != nullptr)
            {
                SetTexture(res, type);
                UpdateMaterial();
            }
            else
            {
                HE_ERROR("Failed when loading the texture.");
            }
        }

        ImGuiFileDialog::Instance()->Close();
    }
}

void Hachiko::ResourceMaterial::RemoveTexture(ResourceTexture::Type type)
{
    if (ImGui::Button(StringUtils::Concat("Remove ", TypeToString(type).c_str()).c_str()))
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
        case ResourceTexture::Type::METALNESS:
            metalness = nullptr;
            break;
        case ResourceTexture::Type::EMISSIVE:
            emissive = nullptr;
            break;
        }
        UpdateMaterial();
    }
}

void Hachiko::ResourceMaterial::UpdateMaterial()
{
    MaterialImporter material_importer;
    material_importer.Save(GetID(), this);

    App->scene_manager->GetActiveScene()->RebuildBatches();
}
