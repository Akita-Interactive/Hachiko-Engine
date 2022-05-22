#pragma once

// system includes
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_map>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <execution>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <tuple>
#include <ctime>

#include <thread>
#include <future>
#include <mutex>

#include <Windows.h>
#include <Psapi.h>

//third party includes
#include <SDL.h>
#include <glew.h>
#include <optick.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "ui/ImGuiUtils.h"

#include <imgui_internal.h>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#include <ImGuiFileDialog.h>

#include <imgui-node-editor-0.9/NodeEditor/Include/imgui_node_editor.h>
#include <imnodes.h>

#include <il.h>
#include <ilu.h>
#include <physfs.h>
#include <ImGuizmo.h>
#include <IconsFontAwesome5.h>
#include <MathGeoLib.h>
#include <yaml-cpp/yaml.h>

#include <AK/SoundEngine/Common/AkDefaultLowLevelIODispatcher.h>
#include <AK/SoundEngine/Common/AkFileLocationBase.h>
#include <AK/SoundEngine/Common/AkFilePackage.h>
#include <AK/SoundEngine/Common/AkFilePackageLUT.h>
#include <AK/SoundEngine/Common/AkMultipleFileLocation.h>
#include <AK/SoundEngine/IO/AkFileHelpers.h>
#include <AK/SoundEngine/IO/AkDefaultIOHookBlocking.h>
#include <AK/SoundEngine/IO/AkDefaultIOHookDeferred.h>
#include <AK/SoundEngine/IO/stdafx.h>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/include/assimp/postprocess.h>


//project includes
#include "Globals.h"
#include "Gameplay.h"
#include "Application.h"
#include "modules/Module.h"
#include "core/rendering/RenderList.h"
#include "core/rendering/Program.h"
#include "core/rendering/Quadtree.h"
#include "core/rendering/Skybox.h"

#include "core/GameObject.h"
#include "core/Scene.h"
#include "core/serialization/TypeConverter.h"
#include "core/serialization/Definitions.h"

#include "core/logging/Logger.h"
#include "core/logging/ConsoleLogger.h"
#include "core/logging/ImGuiLogger.h"

#include "core/preferences/Preferences.h"
#include "core/preferences/PreferenceManager.h"

#include "utils/JsonFormatterValue.h"
#include "utils/PathNode.h"
#include "utils/Timer.h"
#include "utils/UUID.h"
#include "utils/StringUtils.h"
#include "utils/FileSystem.h"
#include "utils/WindowsFileUtil.h"

#include "components/Component.h"
#include "importers/Importer.h"

#include "ui/Window.h"
#include "ui/editor/Theme.h"

#include "resources/Resource.h"

//TODO remove rapidjson from the project
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/error/en.h>
