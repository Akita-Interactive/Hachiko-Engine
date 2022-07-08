#include "core/hepch.h"
#include "GBuffer.h"

Hachiko::GBuffer::GBuffer() 
	: _g_buffer(0)
	, _diffuse_texture(0)
	, _specular_smoothness_texture(0)
	, _normal_texture(0)
	, _position_texture(0)
	, _emissive_texture(0)
	, _depth_texture(0)
    , _is_generated(false) 
{
}

Hachiko::GBuffer::~GBuffer() 
{
    Free();
}

void Hachiko::GBuffer::Generate() 
{
    if (_is_generated)
    {
        return;
    }

    // Default texture sizes, these will be resized whenever Resize is called:
    constexpr const unsigned int default_width = 800;
    constexpr const unsigned int default_height = 600;

    // Generate and bind G buffer that will be used for deferred rendering:
    glGenFramebuffers(1, &_g_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _g_buffer);
    
    // Generate diffuse color buffer as texture:
    glGenTextures(1, &_diffuse_texture);
    glBindTexture(GL_TEXTURE_2D, _diffuse_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, default_width, default_height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _diffuse_texture, 0);    

    // Generate specular & smoothness color buffer as texture:
    glGenTextures(1, &_specular_smoothness_texture);
    glBindTexture(GL_TEXTURE_2D, _specular_smoothness_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, default_width, default_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _specular_smoothness_texture, 0);

    // Generate normal color buffer as texture:
    // This can also be encoded using RGB8 like normal maps, and it would save us some memory.
    // Also if we do this improvement, we need to map [-1,1] range to [0,1] range in the shader. (*0.5+0.5)
    // TODO: Do this.
    glGenTextures(1, &_normal_texture);
    glBindTexture(GL_TEXTURE_2D, _normal_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, default_width, default_height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _normal_texture, 0);
    
    // Generate position color buffer as texture:
    glGenTextures(1, &_position_texture);
    glBindTexture(GL_TEXTURE_2D, _position_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, default_width, default_height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, _position_texture, 0);
    
    // Generate emissive color buffer as texture:
    glGenTextures(1, &_emissive_texture);
    glBindTexture(GL_TEXTURE_2D, _emissive_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, default_width, default_height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, _emissive_texture, 0);  

    // Generate depth color buffer as texture:
    glGenTextures(1, &_depth_texture);
    glBindTexture(GL_TEXTURE_2D, _depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, default_width, default_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depth_texture, 0);
    
    // Pass the color attachments we're gonna use for g_buffer frame buffer to opengl:
    constexpr size_t attachments_length = 5;

    unsigned int attachments[attachments_length] = {
        GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, 
        GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4
    };

    glDrawBuffers(attachments_length, attachments);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        HE_ERROR("An error occured while creating G Buffer, status: 0x%x\n", status);
        return;
    }

    _is_generated = true;
}

void Hachiko::GBuffer::Free() 
{
    if (!_is_generated)
    {
        return;
    }

    _is_generated = false;

    glDeleteTextures(1, &_diffuse_texture);
    glDeleteTextures(1, &_specular_smoothness_texture);
    glDeleteTextures(1, &_normal_texture);
    glDeleteTextures(1, &_position_texture);
    glDeleteTextures(1, &_emissive_texture);
    glDeleteTextures(1, &_depth_texture);

    glDeleteBuffers(1, &_g_buffer);
}

void Hachiko::GBuffer::Resize(int width, int height) const 
{
    // Diffuse color texture stored in g buffer:
    glBindTexture(GL_TEXTURE_2D, _diffuse_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_FLOAT, NULL);

    // Specular color (rgb) and smoothness color (a) textures stored in g buffer:
    glBindTexture(GL_TEXTURE_2D, _specular_smoothness_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    // Normal color texture stored in g buffer:
    glBindTexture(GL_TEXTURE_2D, _normal_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);

    // Position color stored in g buffer:
    glBindTexture(GL_TEXTURE_2D, _position_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);

    // Emissive color stored in g buffer:
    glBindTexture(GL_TEXTURE_2D, _emissive_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_FLOAT, NULL);

    // Depth color stored in g buffer:
    glBindTexture(GL_TEXTURE_2D, _depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
}

void Hachiko::GBuffer::BlitDepth(unsigned int target_buffer_id, int width, int height) const 
{
    BindForReading();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target_buffer_id);

    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    
    glBindFramebuffer(GL_FRAMEBUFFER, target_buffer_id);
}

void Hachiko::GBuffer::BindTextures() const 
{
    // Bind diffuse texture:
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _diffuse_texture);
    // Bind specular & smoothness texture:
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _specular_smoothness_texture);
    // Bind normal texture:
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _normal_texture);
    // Bind position texture:
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, _position_texture);
    // Bind emissive texture:
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, _emissive_texture);
}

void Hachiko::GBuffer::UnbindTextures() const 
{
    // Bind diffuse texture:
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    // Bind specular & smoothness texture:
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    // Bind normal texture:
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
    // Bind position texture:
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, 0);
    // Bind emissive texture:
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Hachiko::GBuffer::BindForReading() const 
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, _g_buffer);
}

void Hachiko::GBuffer::BindForDrawing() const 
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _g_buffer);
}