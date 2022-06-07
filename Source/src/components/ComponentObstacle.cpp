#include "core/hepch.h"

#include "ComponentObstacle.h"
#include "ComponentTransform.h"
#include "Application.h"

#include "modules/ModuleNavigation.h"
#include "resources/ResourceNavMesh.h"
#include "DetourTileCache/DetourTileCache.h"

Hachiko::ComponentObstacle::ComponentObstacle(GameObject* container) : Component(Type::OBSTACLE, container) {}

Hachiko::ComponentObstacle::~ComponentObstacle()
{
    RemoveObstacle();
}

void Hachiko::ComponentObstacle::Start()
{
    AddObstacle();
}

void Hachiko::ComponentObstacle::Stop()
{
    RemoveObstacle();
}

void Hachiko::ComponentObstacle::Update()
{
    if (dirty)
    {
        ++count_since_update;
        if (count_since_update > update_freq)
        {
            RefreshObstacle();
            dirty = false;
            count_since_update = 0;
        }
    }
}

void Hachiko::ComponentObstacle::OnTransformUpdated()
{
    Invalidate();
}

void Hachiko::ComponentObstacle::SetType(ObstacleType new_type)
{
    obstacle_type = new_type;
    Invalidate();
}

void Hachiko::ComponentObstacle::SetSize(const float3& new_size)
{
    size = new_size;
    Invalidate();
}

void Hachiko::ComponentObstacle::DrawGui()
{
    ImGui::PushID(this);
    if (ImGui::CollapsingHeader("Obstacle", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (!obstacle && ImGui::Button("Add to navmesh"))
        {
            AddObstacle();
        }
        if (obstacle && ImGui::Button("Remove from navmesh"))
        {
            RemoveObstacle();
        }

        if (obstacle_type == ObstacleType::DT_OBSTACLE_CYLINDER)
        {
            if (ImGui::DragFloat2("Radius, Height", size.ptr()))
            {
                SetSize(size);
            }
        }
        else
        {
            if (ImGui::DragFloat3("Size", size.ptr()))
            {
                SetSize(size);
            }
        }

        bool changed_type = false;
        ImGui::Text("Type");
        changed_type |= ImGui::RadioButton("Cylinder", (int*)&obstacle_type, static_cast<int>(ObstacleType::DT_OBSTACLE_CYLINDER));
        ImGui::SameLine();
        changed_type |= ImGui::RadioButton("Box", (int*)&obstacle_type, static_cast<int>(ObstacleType::DT_OBSTACLE_BOX));
        ImGui::SameLine();
        changed_type |= ImGui::RadioButton("Oriented Box", (int*)&obstacle_type, static_cast<int>(ObstacleType::DT_OBSTACLE_ORIENTED_BOX));

        if (changed_type)
        {
            SetType(obstacle_type); // Changes the obstacle type
        }

        ImGui::Text("Exists: %d", obstacle != nullptr);
    }
    ImGui::PopID();
}

void Hachiko::ComponentObstacle::AddObstacle()
{
    dtTileCache* tile_cache = App->navigation->GetTileCache();
    if (!tile_cache)
    {
        HE_LOG("Failed to add obstacle on unexisting tile cache");
        return;
    }

    // If obstacle is already set remove it before adding it again
    // Remove already contains the check
    RemoveObstacle();

    obstacle = new dtObstacleRef;

    ComponentTransform* transform = game_object->GetTransform();

    switch (obstacle_type)
    {
    case ObstacleType::DT_OBSTACLE_CYLINDER:
        // In this case we dont use size z, we only need height and radius
        // Look for draw gizmo option that seems to be used on some examples
        tile_cache->addObstacle(transform->GetGlobalPosition().ptr(), size.x, size.y, obstacle);
        break;
    case ObstacleType::DT_OBSTACLE_BOX:
        {
            // This case needs a scope for initializing
            AABB aabb;
            aabb.SetFromCenterAndSize(transform->GetGlobalPosition(), size);
            tile_cache->addBoxObstacle(aabb.minPoint.ptr(), aabb.maxPoint.ptr(), obstacle);
            break;
        }
    case ObstacleType::DT_OBSTACLE_ORIENTED_BOX:
        // Method expects half the desired size of the box to be passed
        // Expects y rotation in radians and says box cant be rotated in y
        tile_cache->addBoxObstacle(transform->GetGlobalPosition().ptr(), (size / 2.f).ptr(), transform->GetGlobalRotation().ToEulerXYX().y, obstacle);
        break;
    }

    if (!obstacle)
    {
        HE_LOG("Failed to add obstacle with existing navmesh and tile cache");
    }
}

void Hachiko::ComponentObstacle::RemoveObstacle()
{
    if (!obstacle)
    {
        // No need to remove an obstacle that is already not being used
        return;
    }

    dtTileCache* tile_cache = App->navigation->GetTileCache();
    if (!tile_cache)
    {
        HE_LOG("Failed to remove obstacle from unexisting tile cache");
        return;
    }

    tile_cache->removeObstacle(*obstacle);
    RELEASE(obstacle);
}

void Hachiko::ComponentObstacle::RefreshObstacle()
{
    if (obstacle)
    {
        // Refesh the obstacle position if it exists
        AddObstacle();
    }
}

void Hachiko::ComponentObstacle::Save(YAML::Node& node) const
{
    node.SetTag("obstacle");
    node[OBSTACLE_TYPE] = static_cast<unsigned int>(obstacle_type);
    node[OBSTACLE_SIZE] = size;
}

void Hachiko::ComponentObstacle::Load(const YAML::Node& node)
{
    SetType(static_cast<ObstacleType>(node[OBSTACLE_TYPE].as<unsigned int>()));
    SetSize(node[OBSTACLE_SIZE].as<float3>());
}