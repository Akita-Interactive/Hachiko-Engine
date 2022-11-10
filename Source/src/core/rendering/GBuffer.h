#pragma once

namespace Hachiko
{
class GBuffer
{
public:
    GBuffer();
    ~GBuffer();

    void Generate();
    void Free();
    void Resize(int width, int height) const;
    void BlitDepth(unsigned int target_buffer_id, int widht, int height) const;
    void BindTextures() const;
    void UnbindTextures() const;
    void BindForReading() const;
    void BindForDrawing() const;
    void BindFogTextures() const;
    void UnbindFogTextures() const;
    void BindSSAOTextures() const;
    void UnbindSSAOTextures() const;
    [[nodiscard]] bool IsGenerated() const;

    [[nodiscard]] unsigned int GetEmissiveTexture() const;
    [[nodiscard]] static unsigned int GetEmissiveTextureType();
    [[nodiscard]] static unsigned int GetEmissiveTextureFormat();
    [[nodiscard]] static unsigned int GetEmissiveTextureInternalFormat();

private:
    unsigned _g_buffer = 0;
    unsigned _depth_stencil_buffer = 0;
    unsigned _diffuse_texture = 0;
    unsigned _specular_smoothness_texture = 0;
    unsigned _normal_texture = 0;
    unsigned _position_texture = 0;
    unsigned _emissive_texture = 0;
    unsigned _depth_texture = 0;
    bool _is_generated = false;
};

inline bool GBuffer::IsGenerated() const
{
    return _is_generated;
}

inline unsigned int GBuffer::GetEmissiveTexture() const
{
    return _emissive_texture;
}
} // namespace Hachiko