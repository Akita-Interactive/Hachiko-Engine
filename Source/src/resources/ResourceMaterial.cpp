#include "core/hepch.h"
#include "ResourceMaterial.h"
#include "modules/ModuleResources.h"
#include "modules/ModuleSceneManager.h"
#include "importers/MaterialImporter.h"

Hachiko::ResourceMaterial::ResourceMaterial(UID uid) :
    Resource(uid, Type::MATERIAL)
{
    App->scene_manager->GetActiveScene();
}

Hachiko::ResourceMaterial::~ResourceMaterial()
{
}

void Hachiko::ResourceMaterial::DrawGui()
{
    static const ImGuiTreeNodeFlags texture_flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
    Widgets::Label("Material", name);

    Widgets::Combo("Material type", &is_metallic, material_types, IM_ARRAYSIZE(material_types));
    if (!is_metallic)
    {
        ImGui::TextDisabled("Keep in mind that the specular workflow is not fully suported!");
    }

    bool was_transparent = is_transparent;
    Widgets::Combo("Transparency", &is_transparent, transparency, IM_ARRAYSIZE(transparency));

    if (was_transparent != is_transparent)
    {
        // If the transparency property of a material is changed, notify
        // scene to arrange batching again:
        App->scene_manager->RebuildBatches();
    }

    if (ImGui::TreeNodeEx(&diffuse, texture_flags, "Diffuse"))
    {
        if (diffuse != nullptr)
        {
            ImGui::Image(reinterpret_cast<void*>(diffuse->GetImageId()), ImVec2(80, 80));
            ImGui::BeginGroup();
            Widgets::Label("Dimensions", StringUtils::Format("%dx%d", diffuse->width, diffuse->height));
            Widgets::Label("Path", diffuse->path);

            if (ImGui::CollapsingHeader("Resource diffuse texture"))
            {
                diffuse->DrawGui();
            }

            // TODO: textue configuration (maybe delegate to the ResourceTexture)

            RemoveTexture(ResourceTexture::Type::DIFFUSE);

            ImGui::EndGroup();
        }
        else
        {
            Widgets::ColorEdit4("Diffuse color", diffuse_color);
            AddTexture(ResourceTexture::Type::DIFFUSE);
        }
        ImGui::TreePop();
    }
    if (is_metallic)
    {
        if (ImGui::TreeNodeEx(&metalness, texture_flags, "Metalness"))
        {
            if (metalness != nullptr)
            {
                ImGui::Image(reinterpret_cast<void*>(metalness->GetImageId()), ImVec2(80, 80));
                ImGui::BeginGroup();
                Widgets::Label("Dimensions", StringUtils::Format("%dx%d", metalness->width, metalness->height));
                Widgets::Label("Path", metalness->path);

                if (ImGui::CollapsingHeader("Resource metalness texture"))
                {
                    metalness->DrawGui();
                }

                RemoveTexture(ResourceTexture::Type::METALNESS);

                ImGui::EndGroup();
            }
            else
            {
                Widgets::SliderFloat("Metalness", metalness_value);
                AddTexture(ResourceTexture::Type::METALNESS);
                Widgets::SliderFloat("Smoothness", smoothness);
            }

            ImGui::TreePop();
        }
    }
    else
    {
        if (ImGui::TreeNodeEx(&specular, texture_flags, "Specular"))
        {
            if (specular != nullptr)
            {
                ImGui::Image(reinterpret_cast<void*>(specular->GetImageId()), ImVec2(80, 80));
                ImGui::BeginGroup();
                Widgets::Label("Dimensions", StringUtils::Format("%dx%d", specular->width, specular->height));
                Widgets::Label("Path", specular->path);

                // TODO: textue configuration (maybe delegate to the ResourceTexture)
                if (ImGui::CollapsingHeader("Resource specular texture"))
                {
                    specular->DrawGui();
                }

                RemoveTexture(ResourceTexture::Type::SPECULAR);

                ImGui::EndGroup();
            }
            else
            {
                Widgets::ColorEdit4("Specular color", specular_color);
                AddTexture(ResourceTexture::Type::SPECULAR);
                Widgets::SliderFloat("Smoothness", smoothness);
            }

            ImGui::TreePop();
        }
    }
    if (ImGui::TreeNodeEx((void*)&normal, texture_flags, "Normal"))
    {
        if (normal != nullptr)
        {
            ImGui::Image(reinterpret_cast<void*>(normal->GetImageId()), ImVec2(80, 80));
            ImGui::BeginGroup();
            Widgets::Label("Dimensions", StringUtils::Format("%dx%d", normal->width, normal->height));
            Widgets::Label("Path", normal->path);

            // TODO: textue configuration (maybe delegate to the ResourceTexture)
            if (ImGui::CollapsingHeader("Resource normal texture"))
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
            ImGui::BeginGroup();
            Widgets::Label("Dimensions", StringUtils::Format("%dx%d", emissive->width, emissive->height));
            Widgets::Label("Path", emissive->path);

            // TODO: textue configuration (maybe delegate to the ResourceTexture)
            if (ImGui::CollapsingHeader("Resource Emissive Texture"))
            {
                emissive->DrawGui();
            }

            Widgets::ColorEdit4("Emissive color", emissive_color);
            RemoveTexture(ResourceTexture::Type::EMISSIVE);

            ImGui::EndGroup();
        }
        else
        {
            Widgets::ColorEdit4("Emissive color", emissive_color);
            AddTexture(ResourceTexture::Type::EMISSIVE);
        }
        ImGui::TreePop();
    }
    if (ImGui:: Button("Save", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
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

    if (ImGui::Button(StringUtils::Concat(TypeToString(type).c_str(), " texture").c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
    {
        const char* filters = "Image files{.png,.tif,.jpg,.tga}";
        ImGuiFileDialog::Instance()->OpenDialog(title,
                                                "Select Texture",
                                                filters,
                                                "./assets/textures/",
                                                1,
                                                nullptr,
                                                ImGuiFileDialogFlags_DontShowHiddenFiles | ImGuiFileDialogFlags_DisableCreateDirectoryButton | ImGuiFileDialogFlags_HideColumnType
                                                | ImGuiFileDialogFlags_HideColumnDate);
    }

    if (ImGuiFileDialog::Instance()->Display(title))
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
    if (ImGui::Button(StringUtils::Concat("Remove ", TypeToString(type)).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
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
