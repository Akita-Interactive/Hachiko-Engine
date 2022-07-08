#include <scripting/Script.h>
#include "entities/player/PlayerController.h"

namespace Hachiko
{

class GameObject;
class ComponentBillboard;

namespace Scripting
{
class CombatVisualEffectsPool : public Script
{
	SERIALIZATION_METHODS(false);
public:
	CombatVisualEffectsPool(GameObject* game_object);
	~CombatVisualEffectsPool() = default;

	void OnAwake() override;
	void OnUpdate() override;

	void PlayPlayerAttackEffect(PlayerController::WeaponUsed weapon_type, int attack_index, float3 position);
	ComponentBillboard* GetCurrentBillboard() const;
	
private:
	std::vector<ComponentBillboard*> _billboards;
	SERIALIZE_FIELD(int, _current_billboard_index);
};
} // namespace Scripting
} // namespace Hachiko