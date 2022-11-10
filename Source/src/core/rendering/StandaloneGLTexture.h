#pragma once

namespace Hachiko
{
    class StandaloneGLTexture
    {
    public:
        StandaloneGLTexture(
            int width, 
            int height, 
            int internal_format, 
            int border, 
            int format, 
            unsigned int type, 
            int min_filter, 
            int mag_filter, 
            bool generate_depth = false);
        ~StandaloneGLTexture();

        void Bind() const;
        void BindBuffer(bool resize_viewport = false) const;
        void BindForReading(unsigned int location = 0) const;
        void Unbind() const;
        void Resize(int width, int height);
        
        [[nodiscard]] unsigned int GetTextureId() const;
        [[nodiscard]] unsigned int GetFrameBufferId() const;
        void GetSize(unsigned int& width, unsigned int& height) const;

        // These should be defined properly if we want to use this class in
        // move and/or copy required contexts such as stl containers etc:
        StandaloneGLTexture(const StandaloneGLTexture& other) = delete;
        StandaloneGLTexture(StandaloneGLTexture&& other) = delete;
        StandaloneGLTexture operator=(StandaloneGLTexture& other) = delete;
        StandaloneGLTexture operator=(StandaloneGLTexture&& other) = delete;

    private:
        unsigned int _texture_id;
        unsigned int _depth_id;
        unsigned int _frame_buffer_id;
        int _width;
        int _height;
        int _internal_format;
        int _format;
        unsigned int _type;
        int _border;
        int _min_filter;
        int _mag_filter;
        bool _has_depth;
    };
}

