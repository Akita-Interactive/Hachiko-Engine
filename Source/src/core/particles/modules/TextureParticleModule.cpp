#include "core/hepch.h"
#include "TextureParticleModule.h"
#include "resources/ResourceTexture.h"
#include "modules/ModuleResources.h"

Hachiko::TextureParticleModule::TextureParticleModule(const std::string& name) : ParticleModule(name, false)
{
}

void Hachiko::TextureParticleModule::Update(std::vector<Particle>& particles)
{
    for (auto& particle : particles)
    {
        if (!particle.IsActive())
        {
            continue;
        }

        UpdateTextureOverTime(particle);
    }
}

void Hachiko::TextureParticleModule::DrawGui()
{
    ImGui::Checkbox("Loop##animation_loop", &animation_loop);
    static const ImGuiTreeNodeFlags texture_flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

    if (ImGui::TreeNodeEx((void*)&texture, texture_flags, "Texture"))
    {
        if (texture != nullptr)
        {
            ImGui::Image(reinterpret_cast<void*>(texture->GetImageId()), ImVec2(80, 80));
            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::Text("%dx%d", texture->width, texture->height);
            ImGui::Text("Path: %s", texture->path.c_str());
            if (ImGui::Button("Remove Texture##remove_texture"))
            {
                RemoveTexture();
            }

            ImGui::EndGroup();
        }
        else
        {
            AddTexture();
        }
        ImGui::TreePop();
    }
    if (ImGui::DragScalar("Xtiles", ImGuiDataType_U32, &x_tiles))
    {
        if (x_tiles)
        {
            x_factor = 1 / (float)x_tiles;
            HE_LOG("x_factor: %f", x_factor);
        }
    }
    if (ImGui::DragScalar("Ytiles", ImGuiDataType_U32, &y_tiles))
    {
        if (y_tiles)
        {
            y_factor = 1 / (float)y_tiles;
            HE_LOG("y_factor: %f", y_factor);
        }
    }

    //ImGui::DragInt("Cycles##animation_cycles", &animation_cycles, 1, 1, inf);
    ImGui::Checkbox("FlipX##animation_loop", &has_flip_x);
    ImGui::SameLine();
    ImGui::Checkbox("FlipY##animation_loop", &has_flip_y);
}

void Hachiko::TextureParticleModule::Save(YAML::Node& node) const 
{
}

void Hachiko::TextureParticleModule::Load(const YAML::Node& node)
{}

void Hachiko::TextureParticleModule::UpdateTextureOverTime(Particle& particle)
{
    particle.texture = this->texture;
    particle.has_texture = this->has_texture;
}

void Hachiko::TextureParticleModule::AddTexture()
{
    const std::string title = "Select billboard texture ";
    std::string texture_path;
    ResourceTexture* res = nullptr;

    if (ImGui::Button("Add Texture"))
    {
        ImGuiFileDialog::Instance()->OpenDialog(
            title.c_str(), "Select Texture", ".png,.tif,.jpg,.tga", "./assets/textures/", 1, nullptr,
            ImGuiFileDialogFlags_DontShowHiddenFiles | ImGuiFileDialogFlags_DisableCreateDirectoryButton |
            ImGuiFileDialogFlags_HideColumnType | ImGuiFileDialogFlags_HideColumnDate);
    }

    if (ImGuiFileDialog::Instance()->Display(title.c_str()))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            texture_path = ImGuiFileDialog::Instance()->GetFilePathName();
        }

        ImGuiFileDialog::Instance()->Close();
    }

    texture_path.append(META_EXTENSION);
    if (!std::filesystem::exists(texture_path.c_str()))
    {
        return;
    }

    YAML::Node texture_node = YAML::LoadFile(texture_path);
    res = static_cast<ResourceTexture*>(App->resources->GetResource(Resource::Type::TEXTURE, texture_node[RESOURCES][0][RESOURCE_ID].as<UID>()));

    if (res != nullptr)
    {
        texture = res;
        textureID = texture->GetID();
        has_texture = 1;
    }
}

void Hachiko::TextureParticleModule::RemoveTexture()
{
    texture = nullptr;
    textureID = 0;
    has_texture = 0;
}
