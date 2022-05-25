#include "core/hepch.h"
#include "Skybox.h"

#include "modules/ModuleProgram.h"
#include "components/ComponentCamera.h"
#include "modules/ModuleResources.h"
#include "resources/ResourceTexture.h"

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
}

void Hachiko::Skybox::Draw(ComponentCamera* camera) const
{
    // Use for optimized version (draw at the end) glDepthFunc(GL_LEQUAL);
    OPTICK_CATEGORY("Draw", Optick::Category::Rendering);

    glDepthFunc(GL_ALWAYS);
    Program* program = App->program->GetSkyboxProgram();
    program->Activate();
    // Draw skybox
    glBindVertexArray(vao);

    //const int texture_slots = 0;
    //App->program->GetMainProgram()->BindUniformInts("skybox", 1, &texture_slots);

    GLint skybox_binding = glGetUniformLocation(program->GetId(), "skybox");
    glUniform1i(skybox_binding, 0);

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

void Hachiko::Skybox::CreateBuffers()
{
    constexpr float vertices[]
        = {
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
