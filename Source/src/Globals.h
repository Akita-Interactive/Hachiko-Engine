#pragma once
#include "core/logging/ConsoleLogger.h"

inline Hachiko::ConsoleLogger logger;

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define HE_LOG(format, ...) logger.Log(Hachiko::LogLevel::Debug, __FILENAME__, __LINE__, format, __VA_ARGS__)
#define HE_ERROR(format, ...) logger.Log(Hachiko::LogLevel::Error, __FILENAME__, __LINE__, format, __VA_ARGS__)

#define HACHIKO_PI 3.14159265358979323846
constexpr float TO_RAD = static_cast<float>(HACHIKO_PI) / 180.0f;
constexpr float TO_DEG = 180.0f / static_cast<float>(HACHIKO_PI);

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
#define ASSETS_FOLDER_SCENES "assets/scenes/"
#define ASSETS_FOLDER_TEXTURES "assets/textures/"
#define ASSETS_FOLDER_FONTS "assets/fonts/"

#define SETTINGS_FOLDER "settings"
#define SETTINGS_FILE_PATH "settings/he.cfg"

#define LIBRARY_FOLDER "library/"
#define LIBRARY_TEXTURES_FOLDER "library/textures/"
#define LIBRARY_MESH_FOLDER "library/meshes/"
#define LIBRARY_SCENE_FOLDER "library/scenes/"
#define LIBRARY_MATERIAL_FOLDER "library/materials/"
#define LIBRARY_MODEL_FOLDER "library/models/"
#define LIBRARY_SHADERS_FOLDER "library/shaders/"
#define LIBRARY_FONTS_FOLDER "library/fonts/"

#define JPG_TEXTURE_EXTENSION ".jpg"
#define PNG_TEXTURE_EXTENSION ".png"

#define MATERIAL_EXTENSION ".mat"
#define SCENE_EXTENSION ".scene"
#define PREFAB_EXTENSION ".prefab"
#define META_EXTENSION ".meta"

#define FRAGMENT_SHADER_EXTENSION ".frag"
#define VERTEX_SHADER_EXTENSION ".vert"
#define DEFAULT_SHADER_EXTENSION ".glsl"

#define MODEL_EXTENSION ".fbx"

#define SKYBOX_EXTENSION ".hdr"

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
