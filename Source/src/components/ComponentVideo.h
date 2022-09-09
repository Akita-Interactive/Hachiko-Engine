#pragma once

#include "Component.h"

class AVFormatContext;
class AVCodecContext;
class AVPacket;
class AVFrame;
class SwsContext;
class AVRational;

namespace Hachiko
{
    class ResourceVideo;

    enum class VideoState
    {
        PLAYING = 0,
        PAUSED,
        STOPPED
    };

    class ComponentVideo : public Component
    {
    public:
        ComponentVideo(GameObject* container);
        ~ComponentVideo() override;

        void Start() override;
        void Update() override;
        void DrawGui() override;
        void Draw(ComponentCamera* camera, Program* program);

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

        HACHIKO_API void Play();
        HACHIKO_API void Pause();
        HACHIKO_API void Stop();
        HACHIKO_API void Restart();
        HACHIKO_API bool IsPlaying() const;

    private:
        bool in_scene = false;
        bool projected = false;
        bool loop = false;
        bool able_to_capture = false;
        bool flip_vertical = false;
        unsigned int frame_texture = 0;
        double time = 0.0f;
        float fps = 1.0f;

        VideoState state = VideoState::PAUSED;
        ResourceVideo* video = nullptr;

        // LibAV internal state
        AVFormatContext* format_ctx = nullptr; // Video file context.
        AVCodecContext* video_codec_ctx = nullptr; // Video Decoder context.
        AVCodecContext* audio_codec_ctx = nullptr; // Audio decoder context.
        AVPacket* av_packet = nullptr; // Data packet. This is sent to de decoders to obtain a frame of any type (video or audio).
        AVFrame* av_frame = nullptr; // Frame Data. This is what a decoder returns after decoding a packet.
        SwsContext* scaler_ctx = nullptr; // Used for converting the frame data to RGB format.
        AVRational time_base = {0, 0}; // Used to obtain the FrameTime -> Used to sync video and audio.

        // LibAV external Video data
        int video_stream_index = -1; // Video data stream inside file.
        int frame_width = 0; 
        int frame_height = 0; // Size of video frame.
        uint8_t* frame_data = nullptr; // Buffer for the frame texture data.
        float video_frame_time = 0; // Time in seconds when the next audio frame must appear.

        void DisplayControls();

        void PublishIntoScene();
        void DetachFromScene();

        void FlipImage();
        void BindFrameToGLTexture() const;
        void ReadNextFrame();
        void SetProjectionMatrices(const ComponentCamera* camera, const Program* program);

        void AddVideo();
        void OpenVideo();
        void RemoveVideoResource();
        void FreeVideoReader();
        void CleanFrameBuffer();
        int Reset();
    };
} // namespace Hachiko