#include <scripting/Script.h>

namespace Hachiko
{

class GameObject;
class ComponentPointLight;

namespace Scripting
{
class BlinkingLight : public Script
{
	SERIALIZATION_METHODS(false)

public:
	BlinkingLight(Hachiko::GameObject* game_object);
	~BlinkingLight() override = default;

	void OnAwake() override;
	void OnUpdate() override;

private:

	SERIALIZE_FIELD(bool, _use_perlin_noise);
	SERIALIZE_FIELD(float, _min_radius);
	SERIALIZE_FIELD(float, _max_radius);
	SERIALIZE_FIELD(float, _radius_amplitude);
	SERIALIZE_FIELD(float, _min_duration);
	SERIALIZE_FIELD(float, _max_duration);
	SERIALIZE_FIELD(float, _duration_amplitude);
	SERIALIZE_FIELD(float, _min_intensity);
	SERIALIZE_FIELD(float, _max_intensity);
	SERIALIZE_FIELD(float, _intensity_amplitude);


	// No needs to tweak these fields from the editor.
	// These are used by the script to lerp, and generated via perlin noise.
	SERIALIZE_FIELD(float, _starting_intensity);
	SERIALIZE_FIELD(float, _starting_radius);
	SERIALIZE_FIELD(float, _current_intensity);
	SERIALIZE_FIELD(float, _current_duration);
	SERIALIZE_FIELD(float, _current_radius);
	SERIALIZE_FIELD(float, _next_duration);
	SERIALIZE_FIELD(float, _next_intensity);
	SERIALIZE_FIELD(float, _next_radius);
	
	Hachiko::ComponentPointLight* _point_light;
	PerlinNoise1D _perlin_noise;
};
} // namespace Scripting
} // namespace Hachiko