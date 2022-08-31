#pragma once

#include "components/Component.h"
#include "Globals.h"

typedef unsigned int dtObstacleRef;

namespace Hachiko
{
    class GameObject;

    class ComponentTransform2D;

    class HACHIKO_API ComponentObstacle : public Component
    {
    public:

        enum ObstacleType
        {
            DT_OBSTACLE_CYLINDER,
            DT_OBSTACLE_BOX, // AABB
            DT_OBSTACLE_ORIENTED_BOX, // OBB
        };


        ComponentObstacle(GameObject* container);
        ~ComponentObstacle() override;
        
        void Stop() override; // Call RemoveObstacle
        virtual void Update() override; // Updates the obstacle (if dirty) every x ticks to prevent tile cache collapse
        virtual void OnTransformUpdated() override; // Sets the obstacle as dirty        

        void AddObstacle(); // Adds the obstacle if tile cache exists
        void RemoveObstacle(); // Removes the obstacle if it exists in tile cache

        bool IsInNavMesh() 
        {
            return obstacle != nullptr;
        }
        
        void SetSize(const float3& new_size); // Changes the obstacle size
        // Reminder: Cylinder will ignore z value
        float3 GetSize() const{ return size; }

        void SetType(ObstacleType new_type); // Changes the obstacle type

        void DrawGui() override;  

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;
              
    private:
        void Invalidate()
        {
            dirty = true;
        }
        void RefreshObstacle();
        static const int update_freq = 25;
        int count_since_update = 0;
        bool dirty = true;        
        dtObstacleRef* obstacle = nullptr;	
        ObstacleType obstacle_type = DT_OBSTACLE_CYLINDER;
        float3 size = float3::one;
    };
} // namespace Hachiko
