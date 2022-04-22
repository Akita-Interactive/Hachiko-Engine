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

#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#include <imgui_internal.h>
#include <ImGuiFileDialog.h>

#include <il.h>
#include <ilu.h>
#include <physfs.h>
#include <ImGuizmo.h>
#include <IconsFontAwesome5.h>
#include <MathGeoLib.h>
#include <yaml-cpp/yaml.h>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/include/assimp/postprocess.h>

//project includes
#include "Globals.h"
#include "Gameplay.h"
#include "Application.h"
#include "modules/Module.h"
#include "RenderList.h"
#include "Program.h"
#include "Quadtree.h"
#include "Skybox.h"

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

#include "components/Component.h"
#include "importers/Importer.h"

#include "ui/Window.h"
#include "ui/ImGuiUtils.h"
#include "ui/editor/Theme.h"

#include "resources/Resource.h"

//TODO remove rapidjson from the project
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/error/en.h>
