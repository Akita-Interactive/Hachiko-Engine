#pragma once
#include "utils/Cache.h"

namespace Hachiko
{
    class Program
    {
    public:
        Program(unsigned vtx_shader, unsigned frg_shader);

        void Activate() const;
        static void Deactivate();
        void CleanUp() const;

        //TODO centralize uniform binding in ModuleProgram with a cleaner API
        void BindUniformFloat4x4(const char* name, const float* data, bool transpose = true) const;
        void BindUniformFloat3(const char* name, const float* data) const;
        void BindUniformFloat4(const char* name, const float* data) const;
        void BindUniformFloat(const char* name, const float* data) const;
        void BindUniformBool(const char* name, bool value) const;
        void BindUniformInts(const char* name, unsigned size, const int* data) const;

        [[nodiscard]] unsigned GetId() const
        {
            return id;
        }

    private:
        unsigned id = 0;
        std::unique_ptr<Cache<const char*, int>> uniforms_cache;

        //handy function to manage store/retrieve from cache
        int GetUniformLocation(const char*) const;
    };

}
