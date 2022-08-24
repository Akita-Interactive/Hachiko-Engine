#include "core/hepch.h"
#include "Skybox.h"

#include "modules/ModuleProgram.h"
#include "components/ComponentCamera.h"
#include "modules/ModuleResources.h"
#include "modules/ModuleRender.h"
#include "resources/ResourceTexture.h"
#include "resources/ResourceSkybox.h"

#include "MathGeoLib.h"

Hachiko::Skybox::Skybox()
{
    cube = ModuleTexture::LoadCubeMap(cube);
    CreateBuffers();
}

Hachiko::Skybox::Skybox(TextureCube new_cube) : cube(new_cube)
{
    cube = ModuleTexture::LoadCubeMap(cube);
    CreateBuffers();
}

Hachiko::Skybox::~Skybox()
{
    glDeleteBuffers(1, &vbo);
    ReleaseCubemap();

    glDeleteTextures(1, &diffuse_ibl_id);
    glDeleteTextures(1, &prefiltered_ibl_id);
    glDeleteTextures(1, &environment_brdf_id);
}

void Hachiko::Skybox::Draw(ComponentCamera* camera) const
{
    // Use for optimized version (draw at the end) glDepthFunc(GL_LEQUAL);
    OPTICK_CATEGORY("Draw", Optick::Category::Rendering);
    
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0);

    Program* program = App->program->GetSkyboxProgram();
    program->Activate();
    // Draw skybox
    glBindVertexArray(vao);

    program->BindUniformInt("skybox", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cube.id);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    Program::Deactivate();
    
    glDepthFunc(GL_LESS);
}

void Hachiko::Skybox::ReleaseCubemap()
{
    glDeleteTextures(1, &cube.id);
    for (unsigned i = 0; i < static_cast<unsigned>(TextureCube::Side::COUNT); ++i)
    {
        App->resources->ReleaseResource(cube.resources[i]);
    }
    cube.loaded = false;
}

void Hachiko::Skybox::DrawImGui() 
{
    ImGui::PushID(this);

    ImGui::Text("Skybox");

    ImGui::TextDisabled("Keep in mind that all images \nneed to have the same format!");

    if (ImGui::CollapsingHeader("Resources"))
    {
        SelectSkyboxTexture(TextureCube::Side::RIGHT);
        SelectSkyboxTexture(TextureCube::Side::LEFT);
        SelectSkyboxTexture(TextureCube::Side::BOTTOM);
        SelectSkyboxTexture(TextureCube::Side::TOP);
        SelectSkyboxTexture(TextureCube::Side::CENTER);
        SelectSkyboxTexture(TextureCube::Side::BACK);
    }

    ImGui::Checkbox("Activate IBL", &activate_ibl);
    if (ImGui::Button("Build precomputed IBL"))
    {
        BuildIBL();
    }

    if (default_ibl == 0)
    {
        if (ImGui::Button("Diffuse (debug)"))
        {
            default_ibl = cube.id;
            cube.id = diffuse_ibl_id;
        }
        ImGui::SameLine();
        if (ImGui::Button("Prefiltered (debug)"))
        {
            default_ibl = cube.id;
            cube.id = prefiltered_ibl_id;
        }
    }
    else
    {
        if (ImGui::Button("Reset (debug)"))
        {
            cube.id = default_ibl;
            default_ibl = 0;
        }
    }

    ImGui::PopID();
}

void Hachiko::Skybox::BindImageBasedLightingUniforms(Program* program) const 
{
    if (activate_ibl)
    {
        program->BindUniformUInt("activate_IBL", 1);

        if (App->renderer->IsDeferred())
        {
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_CUBE_MAP, diffuse_ibl_id);
            //program->BindUniformInt("diffuseIBL", 7);

            glActiveTexture(GL_TEXTURE8);
            glBindTexture(GL_TEXTURE_CUBE_MAP, prefiltered_ibl_id);
            //program->BindUniformInt("prefilteredIBL", 8);

            glActiveTexture(GL_TEXTURE9);
            glBindTexture(GL_TEXTURE_2D, environment_brdf_id);
            //program->BindUniformInt("environmentBRDF", 9);
        }
        else
        {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, diffuse_ibl_id);
            program->BindUniformInt("diffuseIBL", 1);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_CUBE_MAP, prefiltered_ibl_id);
            program->BindUniformInt("prefilteredIBL", 2);

            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, environment_brdf_id);
            program->BindUniformInt("environmentBRDF", 3);
        }

        program->BindUniformUInt("prefilteredIBL_numLevels", prefiltered_ibl_number_of_levels);
    }
    else
    {
        program->BindUniformUInt("activate_IBL", 0);
    }
}

const std::string TextureCubeSideToString(Hachiko::TextureCube::Side cube_side)
{
    switch (cube_side)
    {
    case Hachiko::TextureCube::Side::RIGHT:
        return "Right";
    case Hachiko::TextureCube::Side::LEFT:
        return "Left";
    case Hachiko::TextureCube::Side::BOTTOM:
        return "Bottom";
    case Hachiko::TextureCube::Side::TOP:
        return "Top";
    case Hachiko::TextureCube::Side::CENTER:
        return "Center";
    case Hachiko::TextureCube::Side::BACK:
        return "Back";
    }
}

void Hachiko::Skybox::SelectSkyboxTexture(TextureCube::Side cube_side)
{
    const std::string title = Hachiko::StringUtils::Concat("Select texture ", TextureCubeSideToString(cube_side));
    const char* filters = "Image files{.png,.tif,.jpg,.tga}";

    if (ImGui::Button(Hachiko::StringUtils::Concat(TextureCubeSideToString(cube_side).c_str(), " Texture").c_str()))
    {
        ImGuiFileDialog::Instance()->OpenDialog(title.c_str(),
                                                "Select Texture",
                                                filters,
                                                "./assets/skybox/",
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
            Hachiko::UID res = texture_node[RESOURCES][0][RESOURCE_ID].as<Hachiko::UID>();

            if (res)
            {
                ChangeCubeMapSide(res, cube_side);
            }
            else
            {
                HE_ERROR("Failed when loading the texture.");
            }
        }

        ImGuiFileDialog::Instance()->Close();
    }
}

void Hachiko::Skybox::CreateBuffers()
{
    constexpr float vertices[] = {
        // positions          

        // Back
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,

        // Left
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        // Right
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        // Front
        1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
     

        // Top
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,

        // Bottom
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
}

void Hachiko::Skybox::ChangeCubeMapSide(UID texture_uid, TextureCube::Side cube_side)
{
    unsigned side_number = static_cast<unsigned>(cube_side);
    ReleaseCubemap();
    cube.uids[side_number] = texture_uid;
    cube = ModuleTexture::LoadCubeMap(cube);
}

void Hachiko::Skybox::BuildIBL() 
{
    glDeleteTextures(1, &diffuse_ibl_id);
    glDeleteTextures(1, &prefiltered_ibl_id);
    glDeleteTextures(1, &environment_brdf_id);

    GenerateDiffuseIBL();
    GeneratePrefilteredIBL();
    GenerateEnvironmentBRDF();

    ibl_built = true;
}

void Hachiko::Skybox::GenerateDiffuseIBL()
{
    // Use for optimized version (draw at the end) glDepthFunc(GL_LEQUAL);
    OPTICK_CATEGORY("GenerateDiffuseIBL", Optick::Category::Rendering);
    
    if (!cube.loaded)
    {
        HE_ERROR("There is not a skybox loaded");
        return;
    }

    // Delete last irradiance cubemap
    glDeleteTextures(1, &diffuse_ibl_id);
    diffuse_ibl_id = 0;

    // Initialize variables
    const float3 front[6] = {float3::unitX, -float3::unitX, float3::unitY, -float3::unitY, float3::unitZ, -float3::unitZ};
    const float3 up[6] = {-float3::unitY, -float3::unitY, float3::unitZ, -float3::unitZ, -float3::unitY, -float3::unitY};
    Frustum frustum;
    frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
    frustum.SetPerspective(math::pi / 2.0f, math::pi / 2.0f);
    frustum.SetViewPlaneDistances(0.1f, 100.0f);

    // Activate shader and deactivate the depth mask
    glDepthFunc(GL_ALWAYS);
    glDepthMask(false);
    Program* program = App->program->GetDiffuseIBLProgram();
    program->Activate();
    
    glViewport(0, 0, cube.resources[0]->height, cube.resources[0]->width);

    unsigned frame_buffer;
    glGenFramebuffers(1, &frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

    // Generate irradiance cubemap
    glGenTextures(1, &diffuse_ibl_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, diffuse_ibl_id);
    for (unsigned i = 0; i < 6; ++i)
    {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
            0, 
            GL_RGB32F, 
            cube.resources[i]->width, 
            cube.resources[i]->height, 
            0, 
            GL_RGB, 
            GL_UNSIGNED_BYTE, 
            0
        );
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (unsigned i = 0; i < 6; ++i)
    {
        unsigned attachment = GL_COLOR_ATTACHMENT0;
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, diffuse_ibl_id, 0);
        glDrawBuffers(1, &attachment);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            HE_LOG("Error creating frame buffer");
        }

        frustum.SetFrame(float3::zero, front[i], up[i]);
        App->program->UpdateCamera(frustum);

        // Draw skybox
        glBindVertexArray(vao);
        program->BindUniformInt("skybox", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cube.id);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // Delete and unbind the frame buffer, and unbind the skybox vao
    glDeleteFramebuffers(1, &frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(0);

    // Deactivate shader and set depth mask to default
    Program::Deactivate();
    glDepthFunc(GL_LESS);
    glDepthMask(true);

    float2 fb_size = App->renderer->GetFrameBufferSize();
    glViewport(0, 0, fb_size.y, fb_size.x);
}

void Hachiko::Skybox::GeneratePrefilteredIBL()
{
    // Use for optimized version (draw at the end) glDepthFunc(GL_LEQUAL);
    OPTICK_CATEGORY("GeneratePrefilteredIBL", Optick::Category::Rendering);

    if (!cube.loaded)
    {
        HE_ERROR("There is not a skybox loaded");
        return;
    }
    unsigned width = cube.resources[0]->width;
    unsigned height = cube.resources[0]->height;

    // Delete last irradiance cubemap
    glDeleteTextures(1, &prefiltered_ibl_id);
    prefiltered_ibl_id = 0;

    // Initialize variables
    const float3 front[6] = {float3::unitX, -float3::unitX, float3::unitY, -float3::unitY, float3::unitZ, -float3::unitZ};
    const float3 up[6] = {-float3::unitY, -float3::unitY, float3::unitZ, -float3::unitZ, -float3::unitY, -float3::unitY};
    Frustum frustum;
    frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
    frustum.SetPerspective(math::pi / 2.0f, math::pi / 2.0f);
    frustum.SetViewPlaneDistances(0.1f, 100.0f);

    // Activate shader and deactivate the depth mask
    glDepthFunc(GL_ALWAYS);
    glDepthMask(false);
    Program* program = App->program->GetPrefilteredIBLProgram();
    program->Activate();

    unsigned frame_buffer;
    glGenFramebuffers(1, &frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

    // Generate irradiance cubemap
    glGenTextures(1, &prefiltered_ibl_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefiltered_ibl_id);
    for (unsigned i = 0; i < 6; ++i)
    {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            GL_RGB32F,
            cube.resources[i]->width,
            cube.resources[i]->height,
            0,
            GL_RGB, 
            GL_UNSIGNED_BYTE,
            0
        );
    }
    prefiltered_ibl_number_of_levels = int(log(float(cube.resources[0]->width)) / log(2));
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, prefiltered_ibl_number_of_levels);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    for (int roughness = 0; roughness < prefiltered_ibl_number_of_levels; ++roughness)
    {
        glViewport(0, 0, height, width); // IMPORTANT TODO: MOVE TO ADAPT FOR THE MIPMAP

        // Render each cube plane
        for (unsigned i = 0; i < 6; ++i)
        {
            unsigned attachment = GL_COLOR_ATTACHMENT0;
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefiltered_ibl_id, roughness);
            glDrawBuffers(1, &attachment);

            // Draw UnitCube using prefiltered environment map shader and roughness
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                HE_LOG("Error creating frame buffer");
            }

            frustum.SetFrame(float3::zero, front[i], up[i]);
            App->program->UpdateCamera(frustum);

            float auxRoughness = float(roughness) / float(prefiltered_ibl_number_of_levels - 1);
            program->BindUniformFloat("roughness", &auxRoughness);

            // Draw skybox
            glBindVertexArray(vao);
            program->BindUniformInt("skybox", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cube.id);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        width = width >> 1;
        height = height >> 1;
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // Delete and unbind the frame buffer, and unbind the skybox vao
    glDeleteFramebuffers(1, &frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(0);

    // Deactivate shader and set depth mask to default
    Program::Deactivate();
    glDepthFunc(GL_LESS);
    glDepthMask(true);

    float2 fb_size = App->renderer->GetFrameBufferSize();
    glViewport(0, 0, fb_size.y, fb_size.x);
}

void Hachiko::Skybox::GenerateEnvironmentBRDF() 
{
    // Use for optimized version (draw at the end) glDepthFunc(GL_LEQUAL);
    OPTICK_CATEGORY("GenerateEnvironmentBRDF", Optick::Category::Rendering);

    if (!cube.loaded)
    {
        HE_ERROR("There is not a skybox loaded");
        return;
    }
    const unsigned width = 512;
    const unsigned height = 512;

    // Delete last enviromentBRDF
    glDeleteTextures(1, &environment_brdf_id);
    environment_brdf_id = 0;

    // Activate shader and deactivate the depth mask
    glDepthFunc(GL_ALWAYS);
    glDepthMask(false);
    Program* program = App->program->GetEnvironmentBRDFProgram();
    program->Activate();

    glViewport(0, 0, height, width);

    unsigned frame_buffer;
    glGenFramebuffers(1, &frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

    // Generate irradiance cubemap
    glGenTextures(1, &environment_brdf_id);
    glBindTexture(GL_TEXTURE_2D, environment_brdf_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RG, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    unsigned attachment = GL_COLOR_ATTACHMENT0;
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, environment_brdf_id, 0);
    glDrawBuffers(1, &attachment);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        HE_LOG("Error creating frame buffer");
    }
    glDrawArrays(GL_TRIANGLES, 0, 3);
    

    glBindTexture(GL_TEXTURE_2D, 0);

    // Delete and unbind the frame buffer, and unbind the skybox vao
    glDeleteFramebuffers(1, &frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(0);

    // Deactivate shader and set depth mask to default
    Program::Deactivate();
    glDepthFunc(GL_LESS);
    glDepthMask(true);

    float2 fb_size = App->renderer->GetFrameBufferSize();
    glViewport(0, 0, fb_size.y, fb_size.x);
}
