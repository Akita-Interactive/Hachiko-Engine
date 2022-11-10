#include "scriptingUtil/gameplaypch.h"
#include "AssetsObstacle.h"
#include "components/ComponentObstacle.h"

Hachiko::Scripting::AssetsObstacle::AssetsObstacle(GameObject* game_object)
	: Script(game_object, "AssetsObstacle")
{}

void Hachiko::Scripting::AssetsObstacle::OnAwake()
{
	_obstacle = game_object->GetComponent<ComponentObstacle>();
	if (_obstacle != nullptr)
	{
		_obstacle->AddObstacle();
	}
	
	std::vector<ComponentObstacle*> obstacles = game_object->GetComponentsInDescendants<ComponentObstacle>();
	for (int i = 0; i < obstacles.size(); ++i)
	{
		obstacles[i]->AddObstacle();
	}
}
