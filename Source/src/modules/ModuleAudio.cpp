#include "core/hepch.h"
#include "ModuleAudio.h"

#include <AK/SoundEngine/Common/AkMemoryMgr.h> // Memory Manager interface
#include <AK/SoundEngine/Common/AkModule.h> // Default memory manager
#include <AK/SoundEngine/Common/IAkStreamMgr.h> // Streaming Manager
#include <AK/Tools/Common/AkPlatformFuncs.h> // Thread defines
#include <AK/SoundEngine/Common/AkSoundEngine.h> // Sound engine
#include <AK/MusicEngine/Common/AkMusicEngine.h> // Music Engine
#include <AK/SpatialAudio/Common/AkSpatialAudio.h> // Spatial Audio

// Include for communication between Wwise and the game -- Not needed in the release version
#ifndef AK_OPTIMIZED
#include <AK/Comm/AkCommunication.h>
#endif // AK_OPTIMIZED

// Bank file names
constexpr wchar_t* BANKNAME_INIT = L"Init.bnk";
constexpr wchar_t* BANKNAME_FOOT = L"Footsteps.bnk";
constexpr wchar_t* BANKNAME_BGM = L"BackgroundMusic.bnk";
constexpr wchar_t* BANKNAME_ATTACKS = L"Attacks.bnk";
constexpr wchar_t* BANKNAME_ENVIROMENT = L"Enviroment.bnk";
constexpr wchar_t* BANKNAME_UI = L"UI.bnk";
constexpr wchar_t* BANKNAME_BOSS = L"Boss.bnk";
constexpr wchar_t* BANKNAME_CINEMATICS = L"Cinematics.bnk";
#include <../../Game/ErimosWwise/GeneratedSoundBanks/Wwise_IDs.h> // IDs generated by Wwise

Hachiko::ModuleAudio::ModuleAudio() = default;

Hachiko::ModuleAudio::~ModuleAudio() = default;

bool Hachiko::ModuleAudio::Init()
{
    HE_LOG("INITIALIZING MODULE: AUDIO");
    HE_LOG("Initializing Wwise");
    AkMemSettings memSettings;

    AK::MemoryMgr::GetDefaultSettings(memSettings);

    if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
    {
        assert(!"Could not create the memory manager.");
        return false;
    }
    HE_LOG("Wwise: Memory manager complete");

    AkStreamMgrSettings stmSettings;
    AK::StreamMgr::GetDefaultSettings(stmSettings);

    // Customize the Stream Manager settings here.
    if (!AK::StreamMgr::Create(stmSettings))
    {
        assert(!"Could not create the Streaming Manager");
        return false;
    }
    HE_LOG("Wwise: Streaming Manager complete");

    // Create a streaming device with blocking low-level I/O handshaking.
    // Note that you can override the default low-level I/O module with your own.

    AkDeviceSettings deviceSettings;
    AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);

    // Customize the streaming device settings here.
    // CAkFilePackageLowLevelIOBlocking::Init() creates a streaming device
    // in the Stream Manager, and registers itself as the File Location Resolver.

    if (low_level_io.Init(deviceSettings) != AK_Success)
    {
        assert(!"Could not create the streaming device and Low-Level I/O system");
        return false;
    }
    HE_LOG("Wwise: Streaming device and Low-Level I/O system complete");

    // Create the Sound Engine
    // Using default initialization parameters
    AkInitSettings initSettings;
    AkPlatformInitSettings platformInitSettings;
    AK::SoundEngine::GetDefaultInitSettings(initSettings);
    AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

    if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
    {
        assert(!"Could not initialize the Sound Engine.");
        return false;
    }
    HE_LOG("Wwise: Sound Engine complete");

    // Initialize the music engine
    // Using default initialization parameters
    AkMusicSettings musicInit;
    AK::MusicEngine::GetDefaultInitSettings(musicInit);

    if (AK::MusicEngine::Init(&musicInit) != AK_Success)
    {
        assert(!"Could not initialize the Music Engine.");
        return false;
    }
    HE_LOG("Wwise: Music Engine complete");

    // Initialize Spatial Audio
    // Using default initialization parameters
    AkSpatialAudioInitSettings settings; // The constructor fills AkSpatialAudioInitSettings with the recommended default settings.

    if (AK::SpatialAudio::Init(settings) != AK_Success)
    {
        assert(!"Could not initialize the Spatial Audio.");
        return false;
    }
    HE_LOG("Wwise: Spatial Audio complete");

#ifndef AK_OPTIMIZED
    // Initialize communications (not in release build!)
    AkCommSettings commSettings;
    AK::Comm::GetDefaultInitSettings(commSettings);

    if (AK::Comm::Init(commSettings) != AK_Success)
    {
        assert(!"Could not initialize communication.");
        return false;
    }
    HE_LOG("Wwise: Communications initialized");
#endif // AK_OPTIMIZED

    low_level_io.SetBasePath(AKTEXT("ErimosWwise/GeneratedSoundBanks/Windows"));

    AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(US)"));

    AkBankID bankID; // Not used. These banks can be unloaded with their file name.

    AKRESULT eResult = AK::SoundEngine::LoadBank(BANKNAME_INIT, bankID);
    assert(eResult == AK_Success);

    eResult = AK::SoundEngine::LoadBank(BANKNAME_FOOT, bankID);
    assert(eResult == AK_Success);

    eResult = AK::SoundEngine::LoadBank(BANKNAME_BGM, bankID);
    assert(eResult == AK_Success);

    eResult = AK::SoundEngine::LoadBank(BANKNAME_ATTACKS, bankID);
    assert(eResult == AK_Success);

    eResult = AK::SoundEngine::LoadBank(BANKNAME_ENVIROMENT, bankID);
    assert(eResult == AK_Success);

    eResult = AK::SoundEngine::LoadBank(BANKNAME_UI, bankID);
    assert(eResult == AK_Success);

    eResult = AK::SoundEngine::LoadBank(BANKNAME_BOSS, bankID);
    assert(eResult == AK_Success);

    eResult = AK::SoundEngine::LoadBank(BANKNAME_CINEMATICS, bankID);
    assert(eResult == AK_Success);

    return true;
}

UpdateStatus Hachiko::ModuleAudio::Update(const float delta)
{
    AK::SoundEngine::RenderAudio();
    return UpdateStatus::UPDATE_CONTINUE;
}

bool Hachiko::ModuleAudio::CleanUp()
{
#ifndef AK_OPTIMIZED
    // Terminate Communication Services
    AK::Comm::Term();
#endif // AK_OPTIMIZED

    // Terminate Spatial Audio
    //AK::SpatialAudio::Term();

    // Terminate the music engine
    AK::MusicEngine::Term();

    // Terminate the sound engine
    AK::SoundEngine::Term();

    // Terminate the streaming device and streaming manager
    // CAkFilePackageLowLevelIOBlocking::Term() destroys its associated streaming device
    // that lives in the Stream Manager, and unregisters itself as the File Location Resolver.
    low_level_io.Term();

    if (AK::IAkStreamMgr::Get())
    {
        AK::IAkStreamMgr::Get()->Destroy();
    }

    // Terminate the Memory Manager
    AK::MemoryMgr::Term();

    return true;
}

void Hachiko::ModuleAudio::OptionsMenu()
{
    static Widgets::SliderFloatConfig cfg;
    cfg.max = 10.0f;
    cfg.format = "%.f";

    if (ImGui::Button("Reload sound banks", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
    {
        ReloadAssets();
    }
}

void Hachiko::ModuleAudio::SetGameObjectOutputBusVolume(AkGameObjectID emitter, AkGameObjectID listener, float value) const 
{
    AK::SoundEngine::SetGameObjectOutputBusVolume(emitter, listener, value);
}

void Hachiko::ModuleAudio::ReloadAssets()
{
    CleanUp();
    Init();
}
