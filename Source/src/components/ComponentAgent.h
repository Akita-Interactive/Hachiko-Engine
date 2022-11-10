#pragma once


namespace Hachiko
{
    class HACHIKO_API ComponentAgent : public Component
	{

	public:
		ComponentAgent(GameObject* container);
		~ComponentAgent() override;

		void Update() override;
        void Start() override;
        void Stop() override;
		
		void SetTargetPosition(const float3& target_pos);
        void SetRadius(float new_radius);
		void SetMaxSpeed(float new_max_speed);
		void SetMaxAcceleration(float new_max_acceleration);
		void SetObstacleAvoidance(bool obstacle_avoidance);
        void SetAsPlayer(bool is_player);


		[[nodiscard]] int GetAgentIndex() const { return agent_id; }
		[[nodiscard]] float GetMaxSpeed() const { return max_speed; }
        [[nodiscard]] float GetRadius() const{ return radius; }
		[[nodiscard]] float GetMaxAcceleration() const { return max_acceleration; }
		[[nodiscard]] bool IsAvoidingObstacles() const { return avoid_obstacles; }
		[[nodiscard]] bool CanReachTarget() const;

		// Returns the max_speed or max_speed_scaled based on time_scale_mode.
		[[nodiscard]] float GetMaxSpeedBasedOnTimeScaleMode() const;
		// Returns the max_acceleration or max_acceleration_scaled based on
		// time_scale_mode.
        [[nodiscard]] float GetMaxAccelerationBasedOnTimeScaleMode() const;

		void AddToCrowd();
		void RemoveFromCrowd();

		void MoveToNearestNavmeshPoint();		
		
		void Save(YAML::Node& node) const override;
		void Load(const YAML::Node& node) override;

		void DrawGui() override;
    private:
		// Refreshes the speed and acceleration used by recast & detour. Called
		// when the time scale changes and the time scale mode of the component
		// changes:
        void RefreshSpeedAndAcceleration();

    private:
        float3 closest_point_half_range = float3(10.f, 10.f, 10.f);
        float3 target_position = float3::zero;
		unsigned int target_poly = 0;
		int agent_id = -1;

		float radius = 0.5f;

		float max_speed = 5.0f;
		float max_acceleration = 2.0f;

        float max_speed_scaled = max_speed * Time::GetTimeScale();
        float max_acceleration_scaled = max_acceleration * Time::GetTimeScale();

		bool avoid_obstacles = false;
		bool use_pathfinder = true;

		bool show_debug_info = false;

		bool is_player = false;
	};
}


