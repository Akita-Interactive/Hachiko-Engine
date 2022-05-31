#pragma once
#include "core/logging/ConsoleLogger.h"

inline Hachiko::ConsoleLogger logger;

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define HE_LOG(format, ...) logger.Log(Hachiko::LogLevel::Debug, __FILENAME__, __LINE__, format, __VA_ARGS__)
#define HE_ERROR(format, ...) logger.Log(Hachiko::LogLevel::Error, __FILENAME__, __LINE__, format, __VA_ARGS__)

#define HACHIKO_PI 3.14159265358979323846
constexpr float TO_RAD = static_cast<float>(HACHIKO_PI) / 180.0f;
constexpr float TO_DEG = 180.0f / static_cast<float>(HACHIKO_PI);

#define CREATE_HISTORY_ENTRY_AFTER_EDIT()                               \
    if (ImGui::IsItemDeactivatedAfterEdit())                            \
    {                                                                   \
        App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);  \
    }

enum class UpdateStatus
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR,
};

// Deletes a buffer
#define RELEASE(x)        \
	{                     \
		if ((x) != nullptr) \
		{                 \
			delete (x);     \
			(x) = nullptr;  \
		}                 \
	}

// Array buffer deletes
#define RELEASE_ARRAY(x)  \
	{                     \
		if ((x) != nullptr) \
		{                 \
			delete[] (x);   \
			(x) = nullptr;  \
		}                 \
	}

// Configuration -----------
#define WINDOWED_RATIO 0.75f
#define FULLSCREEN false
#define RESIZABLE true
#define TITLE "Hachiko"
#define ENGINE_VERSION "0.2"
#define GLSL_VERSION "#version 460"

#define FPS_LOG_SIZE 100

// File System config -----------
#define VERSION "0.1"
#define ASSETS_FOLDER "assets/"
#define LIBRARY_FOLDER "library/"
#define SHADERS_FOLDER "shaders/"

#define SETTINGS_FOLDER "settings"
#define SETTINGS_FILE_PATH "settings/he.cfg"

#define ASSETS_FOLDER_SCENE ASSETS_FOLDER "scenes/"
#define ASSETS_FOLDER_MODEL ASSETS_FOLDER "models/"
#define ASSETS_FOLDER_TEXTURE ASSETS_FOLDER "textures/"
#define ASSETS_FOLDER_VIDEO ASSETS_FOLDER "video/"
#define ASSETS_FOLDER_MATERIAL ASSETS_FOLDER "materials/"
#define ASSETS_FOLDER_SKYBOX ASSETS_FOLDER "skybox/"
#define ASSETS_FOLDER_FONT ASSETS_FOLDER "fonts/"
#define ASSETS_FOLDER_PREFAB ASSETS_FOLDER "prefabs/"
#define GENERATED_FOLDER "generated/"
#define ASSETS_FOLDER_GENERATED_PREFAB ASSETS_FOLDER_PREFAB GENERATED_FOLDER
#define ASSETS_FOLDER_STATE_MACHINE ASSETS_FOLDER "state_machine/"

#define LIBRARY_FOLDER_SCENE LIBRARY_FOLDER "scenes/"
#define LIBRARY_FOLDER_MESH LIBRARY_FOLDER "meshes/"
#define LIBRARY_FOLDER_TEXTURE LIBRARY_FOLDER "textures/"
#define LIBRARY_FOLDER_VIDEO LIBRARY_FOLDER "video/"
#define LIBRARY_FOLDER_MATERIAL LIBRARY_FOLDER "materials/"
#define LIBRARY_FOLDER_ANIMATION LIBRARY_FOLDER "animations/"
#define LIBRARY_FOLDER_SKYBOX LIBRARY_FOLDER "skybox/"
#define LIBRARY_FOLDER_FONT LIBRARY_FOLDER "fonts/"
#define LIBRARY_FOLDER_NAVMESH LIBRARY_FOLDER "navmesh/"
#define LIBRARY_FOLDER_PREFAB LIBRARY_FOLDER "prefabs/"
#define LIBRARY_FOLDER_STATE_MACHINE LIBRARY_FOLDER "state_machine/"

#define AUXILIAR_NODE "_$AssimpFbx$_"

// Defering for file system https://stackoverflow.com/questions/32432450/what-is-standard-defer-finalizer-implementation-in-c
#ifndef defer
struct DeferDummy {};

template<class F>
struct Deferrer
{
	F f;

	~Deferrer()
	{
		f();
	}
};

template<class F>
Deferrer<F> operator*(DeferDummy, F f)
{
	return {f};
}

#define DEFER_(LINE) zz_defer##LINE
#define DEFER(LINE) DEFER_(LINE)
#define defer auto DEFER(__LINE__) = DeferDummy {}* [&]()
#endif // defer
