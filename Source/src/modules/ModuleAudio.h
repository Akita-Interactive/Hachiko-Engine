#pragma once
#include "Module.h"

#include <AK/SoundEngine/IO/AkFilePackageLowLevelIOBlocking.h> // Sample low-level I/O implementation

namespace Hachiko
{
    class AudioPreferences;

    class ModuleAudio : public Module
    {
    public:
        ModuleAudio();
        ~ModuleAudio() override;

        bool Init() override;
        UpdateStatus Update(const float delta) override;
        bool CleanUp() override;

        void OptionsMenu();

    private:
        // We're using the default Low-Level I/O implementation that's part
        // of the SDK's sample code, with the file package extension
        CAkFilePackageLowLevelIOBlocking low_level_io;

        void SetGameObjectOutputBusVolume(AkGameObjectID emitter, AkGameObjectID listener, float value) const;
        void ReloadAssets();
    };
} // namespace Hachiko