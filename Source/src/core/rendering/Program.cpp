#include "core/hepch.h"
#include "Program.h"

Hachiko::Program::Program(unsigned vtx_shader, unsigned frg_shader) :
    id(glCreateProgram()),
    uniforms_cache(std::make_unique<Cache<const char*, int>>(0))
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
    uniforms_cache->Clear();
}

void Hachiko::Program::BindUniformFloat4x4(const char* name, const float* data, bool transpose) const
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, transpose, data);
}

void Hachiko::Program::BindUniformFloat3(const char* name, const float* data) const
{
    glUniform3fv(GetUniformLocation(name), 1, data);
}

void Hachiko::Program::BindUniformFloat4(const char* name, const float* data) const
{
    glUniform4fv(GetUniformLocation(name), 1, data);
}

void Hachiko::Program::BindUniformFloat(const char* name, const float* data) const
{
    glUniform1fv(GetUniformLocation(name), 1, data);
}

void Hachiko::Program::BindUniformBool(const char* name, bool value) const
{
    glUniform1i(GetUniformLocation(name), value);
}

void Hachiko::Program::BindUniformInts(const char* name, unsigned size, const int* data) const
{
    glUniform1iv(GetUniformLocation(name), size, data);
}

int Hachiko::Program::GetUniformLocation(const char* key) const
{
    if(const auto value = uniforms_cache->Get(key) != uniforms_cache->NotFound())
    {
        return value;
    }
    const auto value = glGetUniformLocation(id, key);
    // assert(value == -1 && "The uniform does not exist in the program");
    uniforms_cache->Set(key, value);
    return value;
}
