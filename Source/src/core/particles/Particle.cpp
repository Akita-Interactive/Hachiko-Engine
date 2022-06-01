#include "core/hepch.h"
#include "Particle.h"

#include "modules/ModuleRender.h"
#include "components/ComponentCamera.h"

using namespace Hachiko;

void Particle::Draw(ComponentCamera* camera, Program* program)
{
    glActiveTexture(GL_TEXTURE0);
    int glTexture = 0;
    //has_texture = 0;
    //if (texture != nullptr)
    //{
    //    glTexture = texture->GetImageId();
    //    Hachiko::ModuleTexture::Bind(glTexture, static_cast<int>(Hachiko::ModuleProgram::TextureSlots::DIFFUSE));
    //    has_texture = 1;
    //}

    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    if (render_mode == ParticleRenderMode::PARTICLE_ADDITIVE)
    {
        glBlendFunc(GL_ONE, GL_ONE);
    }
    else
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    float4x4 model_matrix = float4x4::identity;
    //GetOrientationMatrix(camera, model_matrix);
    program->BindUniformFloat4x4("model", model_matrix.ptr());
    program->BindUniformFloat4x4("view", &camera->GetViewMatrix()[0][0]);
    program->BindUniformFloat4x4("proj", &camera->GetProjectionMatrix()[0][0]);

    //program->BindUniformFloat("x_factor", &x_factor);
    //program->BindUniformFloat("y_factor", &y_factor);
    //program->BindUniformFloat("current_frame", &current_frame);
    //program->BindUniformFloat2("animation_index", animation_index.ptr());
    float4 color = float4::one;
    //if (has_color_gradient)
    //{
    //    gradient->getColorAt(color_frame, color.ptr());
    //}

    program->BindUniformFloat4("input_color", color.ptr());
    //program->BindUniformInts("has_texture", 1, &has_texture);

    //int flip_x = has_flip_x ? 1 : 0;
    //int flip_y = has_flip_y ? 1 : 0;
    //program->BindUniformInts("flip_x", 1, &flip_x);
    //program->BindUniformInts("flip_y", 1, &flip_y);

    glBindVertexArray(App->renderer->GetParticleVao());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // Clear
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    return;
}
