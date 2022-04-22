#include "core/hepch.h"
#include "Program.h"

Hachiko::Program::Program(unsigned vtx_shader, unsigned frg_shader) :
    id(glCreateProgram())
{
    glAttachShader(id, vtx_shader);
    glAttachShader(id, frg_shader);
    glLinkProgram(id);
    int res;
    glGetProgramiv(id, GL_LINK_STATUS, &res);
    if (res == GL_FALSE)
    {
        int len = 0;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &len);
        if (len > 0)
        {
            int written = 0;
            const auto info = static_cast<char*>(malloc(len));
            glGetProgramInfoLog(id, len, &written, info);
            // HE_LOG("Program Log Info: %s", info);
            free(info);
        }
        // Set id to 0 to indicate that it failed
        id = 0;
    }
}

void Hachiko::Program::Activate() const
{
    glUseProgram(id);
}

void Hachiko::Program::Deactivate()
{
    glUseProgram(0);
}

void Hachiko::Program::CleanUp() const
{
    glDeleteProgram(id);
}

void Hachiko::Program::BindUniformFloat4x4(const char* name, const float* data, bool transpose) const
{
    glUniformMatrix4fv(glGetUniformLocation(id, name), 1, transpose, data);
}

void Hachiko::Program::BindUniformFloat3(const char* name, const float* data) const
{
    glUniform3fv(glGetUniformLocation(id, name), 1, data);
}

void Hachiko::Program::BindUniformFloat4(const char* name, const float* data) const
{
    glUniform4fv(glGetUniformLocation(id, name), 1, data);
}

void Hachiko::Program::BindUniformFloat(const char* name, const float* data) const
{
    glUniform1fv(glGetUniformLocation(id, name), 1, data);
}

void Hachiko::Program::BindUniformBool(const char* name, bool value) const
{
    glUniform1i(glGetUniformLocation(id, name), value);
}

void Hachiko::Program::BindUniformInts(const char* name, unsigned size, const int* data) const
{
    glUniform1iv(glGetUniformLocation(id, name), size, data);
}
