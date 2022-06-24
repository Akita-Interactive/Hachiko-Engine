#include "core/hepch.h"
#include "ComponentAgent.h"

#include "modules/ModuleSceneManager.h"
#include "modules/ModuleNavigation.h"
#include "modules/ModuleEvent.h"

#include "resources/ResourceNavMesh.h"

#include "components/ComponentTransform.h"


Hachiko::ComponentAgent::ComponentAgent(GameObject* container) : Component(Type::AGENT, container)
{
}

Hachiko::ComponentAgent::~ComponentAgent()
{
    if (agent_id != -1)
    {
        RemoveFromCrowd();
    }    
}

void Hachiko::ComponentAgent::Update()
{
    // Is Game running?
    // ...

    if (agent_id == -1)
    {
        // No agent no update
        return;
    }

    // Move agent through NavMesh
    if (!is_player)
    {
        const dtCrowdAgent* dt_agent = App->navigation->GetCrowd()->getAgent(agent_id);
        float3 agent_position(dt_agent->npos[0], dt_agent->npos[1], dt_agent->npos[2]);
        game_object->GetTransform()->SetGlobalPosition(agent_position);
    }
    else
    {
        dtCrowdAgent* dt_agent = App->navigation->GetEditableAgent(agent_id);
        const float3& position = game_object->GetTransform()->GetGlobalPosition();
        dt_agent->npos[0] = position.x;
        dt_agent->npos[1] = position.y;
        dt_agent->npos[2] = position.z;
    }
}

void Hachiko::ComponentAgent::Start()
{
    AddToCrowd();
}

void Hachiko::ComponentAgent::Stop()
{
    RemoveFromCrowd();
}

void DebugAgentInfo(const dtCrowdAgent* ag)
{
    if (!ag)
    {
        ImGui::Text("No agent in navmesh");
        return;
    }
    ImGui::Text("Active %d", ag->active);
    ImGui::Text("State %d", ag->state);
    ImGui::Text("Parial Path %d", ag->partial);
    ImGui::Text("N neighbors %d", ag->nneis);
    ImGui::Text("Desired Speed %d", ag->desiredSpeed);
    ImGui::Text("Current Pos %.2f, %.2f, %.2f", ag->npos[0], ag->npos[1], ag->npos[2]);
    ImGui::Text("Displacement %.2f, %.2f, %.2f", ag->disp[0], ag->disp[1], ag->disp[2]);
    ImGui::Text("Desired Velocity %.2f, %.2f, %.2f", ag->dvel[0], ag->dvel[1], ag->dvel[2]);
    ImGui::Text("Obstacle Adjusted Velocity %.2f, %.2f, %.2f", ag->nvel[0], ag->nvel[1], ag->nvel[2]);
    ImGui::Text("Acc Constrained Velocity %.2f, %.2f, %.2f", ag->vel[0], ag->vel[1], ag->vel[2]);
    ImGui::Separator();
    ImGui::Text("Target Info");
    ImGui::Text("Target State %d", ag->targetState);
    ImGui::Text("Target targetRef %d", ag->targetState);
    ImGui::Text("Target Pos %.2f, %.2f, %.2f", ag->targetPos[0], ag->targetPos[1], ag->targetPos[2]);
    ImGui::Text("Replaning %d", ag->targetReplan);
    ImGui::Text("Replan Time %.5f", ag->targetReplanTime);


}

void Hachiko::ComponentAgent::SetTargetPosition(const float3& target_pos)
{
    if (agent_id == -1)  return;

    dtNavMeshQuery* navQuery = App->navigation->GetNavQuery();
    dtCrowd* crowd = App->navigation->GetCrowd();
    const dtQueryFilter* filter = crowd->getFilter(0);
    const dtCrowdAgent* ag = App->navigation->GetAgent(agent_id);

    if (!ag || !ag->active)
    {
        return;
    }

    if (use_pathfinder)
    {
        float3 corrected_target = target_pos;
        navQuery->findNearestPoly(target_pos.ptr(), closest_point_half_range.ptr(), filter, &target_poly, corrected_target.ptr());
        if (target_poly == 0)
        {
            // HE_LOG("Failed to find valid target position");
            return;
        }
        target_position = corrected_target;
        crowd->requestMoveTarget(agent_id, target_poly, target_position.ptr());
    }
    else
    {
        float3 new_dir = (target_pos - float3(ag->npos)).Normalized();
        float3 new_vel = new_dir * max_speed;
        crowd->requestMoveVelocity(agent_id, new_vel.ptr());
    }
}

void Hachiko::ComponentAgent::SetRadius(float new_radius)
{
    radius = new_radius;

    dtCrowdAgent* agent = App->navigation->GetEditableAgent(agent_id);
    
    if (!agent)
    {
        return;
    }
    
    agent->params.radius = radius;
}

void Hachiko::ComponentAgent::SetMaxSpeed(float new_max_speed)
{
    max_speed = new_max_speed;

    dtCrowdAgent* agent = App->navigation->GetEditableAgent(agent_id);
    
    if (!agent)
    {
        return;
    }
    
    agent->params.maxSpeed = max_speed;
}

void Hachiko::ComponentAgent::SetMaxAcceleration(float new_max_acceleration)
{
    max_acceleration = new_max_acceleration;

    dtCrowdAgent* agent = App->navigation->GetEditableAgent(agent_id);
    
    if (!agent)
    {
        return;
    }
    
    agent->params.maxAcceleration = max_acceleration;
}

void Hachiko::ComponentAgent::SetObstacleAvoidance(bool obstacle_avoidance)
{
    avoid_obstacles = obstacle_avoidance;

    dtCrowdAgent* agent = App->navigation->GetEditableAgent(agent_id);
    
    if (!agent)
    {
        return;
    }
    
    if (avoid_obstacles)
    {
        agent->params.updateFlags |= DT_CROWD_OBSTACLE_AVOIDANCE;
    }
    else
    {
        agent->params.updateFlags ^= DT_CROWD_OBSTACLE_AVOIDANCE;
    }
}

void Hachiko::ComponentAgent::SetAsPlayer(bool new_is_player) 
{
    is_player = new_is_player;
}

void Hachiko::ComponentAgent::AddToCrowd()
{
    ResourceNavMesh* navMesh = App->navigation->GetNavMesh();
    if (agent_id != -1)
    {
        HE_LOG("Error: Tried to add an agent that was already in crowd");
        return;
    }

    // PARAMS INIT
    dtCrowdAgentParams ap;
    memset(&ap, 0, sizeof(ap));
    ap.radius = radius;
    ap.height = navMesh->build_params.agent_height;
    ap.maxAcceleration = max_acceleration;
    ap.maxSpeed = max_speed;
    ap.collisionQueryRange = ap.radius * 12.0f;
    ap.pathOptimizationRange = ap.radius * 30.0f;
    ap.updateFlags = 0;

    ap.updateFlags |= DT_CROWD_ANTICIPATE_TURNS;
    ap.updateFlags |= DT_CROWD_OPTIMIZE_VIS;
    ap.updateFlags |= DT_CROWD_OPTIMIZE_TOPO;
    ap.updateFlags |= DT_CROWD_SEPARATION;
    
    if (avoid_obstacles) 
    {
        ap.updateFlags |= DT_CROWD_OBSTACLE_AVOIDANCE;
    }

    ap.obstacleAvoidanceType = 3;
    ap.separationWeight = 2;

    // Add to Crowd
    agent_id = navMesh->GetCrowd()->addAgent(game_object->GetTransform()->GetGlobalMatrix().TranslatePart().ptr(), &ap);
}

void Hachiko::ComponentAgent::RemoveFromCrowd()
{
    if (agent_id < 0)
    {
        HE_LOG("Error: Tried to remove an agent that was not in crowd");
        return;
    }

    ResourceNavMesh* navMesh = App->navigation->GetNavMesh();
    if (!navMesh)
    {
        agent_id = -1;
        return;
    }
        

    dtCrowd* crowd = navMesh->GetCrowd();
    if (crowd)
    {
        crowd->removeAgent(agent_id);
        agent_id = -1;
    }
}

void Hachiko::ComponentAgent::MoveToNearestNavmeshPoint()
{ 
    // Only callable when agent is not in navmesh at the moment, we can add logic to refresh its navmesh status later
    assert(agent_id == -1);

    
    ResourceNavMesh* navMesh = App->navigation->GetNavMesh();
    dtCrowd* crowd = navMesh->GetCrowd();
    dtNavMeshQuery* navQuery = navMesh->GetQuery();
    
    ComponentTransform* transform = game_object->GetTransform();
    
    const dtQueryFilter* filter = crowd->getFilter(0);

    float3 corrected_position = transform->GetGlobalPosition();
    navQuery->findNearestPoly(transform->GetGlobalPosition().ptr(), closest_point_half_range.ptr(), filter, &target_poly, corrected_position.ptr());
    if (target_poly != 0)
    {
        transform->SetGlobalPosition(corrected_position);
    }    
}

void Hachiko::ComponentAgent::DrawGui()
{
    ImGui::PushID(this);
    if (ImGuiUtils::CollapsingHeader(game_object, this, "Agent Component"))
    {
        if (ImGui::Checkbox("Player agent", &is_player))
        {
            SetAsPlayer(is_player);
        }

        if (!is_player)
        {
            if (agent_id != -1)
            {
                if (ImGui::Button("Remove From Navmesh"))
                {
                    RemoveFromCrowd();
                }
            }
            else
            {
                if (ImGui::Button("Add To Namesh"))
                {
                    AddToCrowd();
                }

                if (ImGui::Button("Move To Nearest Navmesh Point"))
                {
                    MoveToNearestNavmeshPoint();
                }
            }

            ImGui::DragFloat3("Target Position", target_position.ptr(), 1.0f, -inf, inf);
            if (ImGui::Button("Feed position"))
            {
                SetTargetPosition(target_position);
            }

            if (ImGui::DragFloat("Radius", &radius, 0.1f, 100.0f))
            {
                SetRadius(radius);
            }

            if (ImGui::DragFloat("Max Speed", &max_speed, 1.0f, 0.0f))
            {
                SetMaxSpeed(max_speed);
            }
            if (ImGui::DragFloat("Max Acceleration", &max_acceleration, 1.0f, 0.0f))
            {
                SetMaxAcceleration(max_acceleration);
            }
            if (ImGui::Checkbox("Use Pathfinding", &use_pathfinder))
            {
                // Updates target wih new pathfinding value
                SetTargetPosition(target_position);
            }
            if (ImGui::Checkbox("Avoid obstacles (Pathfinding)", &avoid_obstacles))
            {
                SetObstacleAvoidance(avoid_obstacles);
            }

            ImGui::Checkbox("Debug info", &show_debug_info);
            if (show_debug_info)
            {
                DebugAgentInfo(App->navigation->GetCrowd()->getAgent(agent_id));
            }
        }
    }
    ImGui::PopID();
}

void Hachiko::ComponentAgent::Save(YAML::Node& node) const
{
    node.SetTag("agent");
    node[MAX_SPEED] = max_speed;
    node[MAX_ACCELERATION] = max_acceleration;
    node[AVOID_OBSTACLES] = avoid_obstacles;
    node[AGENT_RADIUS] = radius;
}

void Hachiko::ComponentAgent::Load(const YAML::Node& node)
{
    max_speed = node[MAX_SPEED].as<float>();
    max_acceleration = node[MAX_ACCELERATION].as<float>();
    avoid_obstacles = node[AVOID_OBSTACLES].as<bool>();
    
    float radius = node[AGENT_RADIUS].IsDefined() ? node[AGENT_RADIUS].as<float>() : 0.5f;
    SetRadius(radius);
}