#include "core/hepch.h"
#include "StandaloneGLTexture.h"

Hachiko::StandaloneGLTexture::StandaloneGLTexture(
    int width, 
    int height, 
    int internal_format, 
    int border, 
    int format, 
    unsigned int type, 
    int min_filter, 
    int mag_filter, 
    bool generate_depth) 
    : _width(width) 
    , _height(height) 
    , _internal_format(internal_format)
    , _format(format)
    , _type(type)
    , _border(border)
    , _min_filter(min_filter)
    , _mag_filter(mag_filter)
    , _has_depth(generate_depth)
{
    // Generate and bind frame buffer:
    _frame_buffer_id = 0;
    glGenFramebuffers(1, &_frame_buffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, _frame_buffer_id);
    // Generate texture:
    _texture_id = 0;
    glGenTextures(1, &_texture_id);
    glBindTexture(GL_TEXTURE_2D, _texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, _internal_format, _width, _height, _border, _format, _type, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _mag_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture_id, 0);

    // Generate and bind _depth_id as the depth buffer of the frame buffer:
    _depth_id = 0;
    if (_has_depth)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture_id, 0);
        glGenRenderbuffers(1, &_depth_id);
        glBindRenderbuffer(GL_RENDERBUFFER, _depth_id);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _depth_id);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        HE_ERROR("Framebuffer incomplete.");
    }

    // Unbind the frame buffer:
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Hachiko::StandaloneGLTexture::~StandaloneGLTexture()
{
    // Free texture:
    glDeleteTextures(1, &_texture_id);
    // Free depth:
    if (_has_depth)
    {
        glDeleteTextures(1, &_depth_id);
    }

    // Free framebuffer:
    glDeleteBuffers(1, &_frame_buffer_id);
}

void Hachiko::StandaloneGLTexture::Bind() const 
{
    glBindTexture(GL_TEXTURE_2D, _texture_id);
}

void Hachiko::StandaloneGLTexture::BindBuffer(bool resize_viewport) const 
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, _frame_buffer_id);
    glViewport(0, 0, _width, _height);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Hachiko::StandaloneGLTexture::BindForReading(unsigned int location) const 
{
    glActiveTexture(GL_TEXTURE0 + location);
    Bind();
}

void Hachiko::StandaloneGLTexture::Unbind() const 
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Hachiko::StandaloneGLTexture::Resize(int width, int height) 
{
    _width = width;
    _height = height;

    glBindTexture(GL_TEXTURE_2D, _texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, _internal_format, 
        _width, _height, _border, _format, _type, NULL);

    if (_has_depth)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, _depth_id);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

unsigned int Hachiko::StandaloneGLTexture::GetTextureId() const
{
    return _texture_id;
}

unsigned int Hachiko::StandaloneGLTexture::GetFrameBufferId() const
{
    return _frame_buffer_id;
}

void Hachiko::StandaloneGLTexture::GetSize(unsigned int& width, unsigned int& height) const 
{
    width = _width;
    height = _height;
}