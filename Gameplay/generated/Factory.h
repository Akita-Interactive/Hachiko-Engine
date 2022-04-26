#pragma once

#include "scriptingUtil/framework.h"

namespace Hachiko
{
class GameObject;

namespace Scripting
{
class Script;
}
}

extern "C" GAMEPLAY_API Hachiko::Scripting::Script* __cdecl 
InstantiateScript(Hachiko::GameObject* game_object, 
	const std::string & script_name);