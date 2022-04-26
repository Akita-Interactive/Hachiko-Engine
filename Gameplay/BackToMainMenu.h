#pragma once
#include <scripting/Script.h>

namespace Hachiko
{
class ComponentButton;
class GameObject;

namespace Scripting
{
class BackToMainMenu : public Script
{
	SERIALIZATION_METHODS(false)

public:
	BackToMainMenu(GameObject* game_object);
	~BackToMainMenu() override = default;

	void OnAwake() override;
	void OnStart() override;
	void OnUpdate() override;

private:
	SERIALIZE_FIELD(ComponentButton*, _button_back);

};
} // namespace Scripting
} // namespace Hachiko