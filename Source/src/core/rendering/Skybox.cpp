#include "core/hepch.h"
#include "Skybox.h"

#include "modules/ModuleProgram.h"
#include "components/ComponentCamera.h"

Hachiko::Skybox::Skybox()
{
    const char* paths[6] = {"Assets/Skybox/skybox_right.png",
                            "Assets/Skybox/skybox_left.png",
                            "Assets/Skybox/skybox_top.png",
                            "Assets/Skybox/skybox_bottom.png",
                            "Assets/Skybox/skybox_center.png",
                            "Assets/Skybox/skybox_back.png"};
    texture = ModuleTexture::LoadCubeMap(paths);

    constexpr float vertices[] = {
        // positions          
        -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f};

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
}

Hachiko::Skybox::~Skybox()
{
    glDeleteBuffers(1, &vbo);
    glDeleteTextures(1, &texture.id);
    texture.loaded = false;
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
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    Program::Deactivate();
    glDepthFunc(GL_LESS);
}
