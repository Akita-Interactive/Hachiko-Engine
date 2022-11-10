#include "core/hepch.h"
#include "ComponentTransform.h"

#include "modules/ModuleEvent.h"
#include "ui/widgets/DragFloat3.h"

/**     CONSTRUCTORS    **/

Hachiko::ComponentTransform::ComponentTransform(GameObject* container) 
    : Component(Type::TRANSFORM, container) 
    , dirty(true)
    , changed(false)
    , matrix(float4x4::identity)
    , matrix_local(float4x4::identity)
    , local_position(float3::zero)
    , local_scale(float3::zero)
    , local_rotation(Quat::identity)
    , local_rotation_euler(float3::zero)
    , right(float3::unitX)
    , up(float3::unitY)
    , front(float3::unitZ)
{
}

Hachiko::ComponentTransform::ComponentTransform(GameObject* container, const float3& new_position_local, const Quat& new_rotation_local, const float3& new_scale_local) 
    : ComponentTransform(container)
{
    SetLocalTransform(new_position_local, new_rotation_local, new_scale_local);
    UpdateTransform();
}

Hachiko::ComponentTransform::ComponentTransform(GameObject* container, const float4x4& new_transform_local) 
    : ComponentTransform(container)
{
    SetLocalTransform(new_transform_local);
    UpdateTransform();
}

void Hachiko::ComponentTransform::OnTransformUpdated()
{
    changed = false;
}

inline Quat Hachiko::ComponentTransform::SimulateLookAt(const float3& direction)
{
    float3 right_temp = float3::unitY.Cross(direction).Normalized();

    float3 up_temp = direction.Cross(right_temp).Normalized();

    Quat orientation = Quat::LookAt(float3::unitZ, direction, float3::unitY, float3::unitY);

    return orientation;
}

void Hachiko::ComponentTransform::LookAtTarget(const float3& target)
{
    const float3 direction = (target - GetGlobalPosition()).Normalized();

    SetGlobalRotation(SimulateLookAt(direction));
}

void Hachiko::ComponentTransform::LookAtDirection(const float3& direction) 
{
    SetGlobalRotation(SimulateLookAt(direction));
}

/**     SETTERS     **/

void Hachiko::ComponentTransform::SetLocalScale(const float3& new_scale) 
{
    local_scale = new_scale;
    Invalidate();
}

void Hachiko::ComponentTransform::SetLocalTransform(const float4x4& new_matrix)
{
    new_matrix.Decompose(local_position, local_rotation, local_scale);
    local_rotation_euler = RadToDeg(local_rotation.ToEulerXYZ());
    Invalidate();
}

void Hachiko::ComponentTransform::SetGlobalTransform(const float4x4& new_transform)
{
    // Use parent global transform to get local transform
    // Sub methods already invalidate
    if (game_object->parent)
    {
        float4x4 parent_transform = game_object->parent->GetTransform()->GetGlobalMatrix();
        parent_transform.Inverse();
        SetLocalTransform(parent_transform * new_transform);
    }
    else
    {
        SetLocalTransform(new_transform);
    }        
}

void Hachiko::ComponentTransform::SetGlobalTransform(const float3& new_position, const Quat& new_rotation, const float3& new_scale)
{
    SetGlobalTransform(float4x4::FromTRS(new_position, new_rotation, new_scale));
}

void Hachiko::ComponentTransform::SetLocalTransform(const float3& new_local_position, const Quat& new_local_rotation, const float3& new_local_scale)
{
    SetLocalTransform(float4x4::FromTRS(new_local_position, new_local_rotation, new_local_scale));
}

void Hachiko::ComponentTransform::SetLocalPosition(const float3& new_position) 
{
    local_position = new_position;
    Invalidate();
}

void Hachiko::ComponentTransform::SetLocalRotation(const Quat& new_rotation) 
{
    local_rotation = new_rotation;
    local_rotation_euler = RadToDeg(local_rotation.ToEulerXYZ());
    Invalidate();
}

void Hachiko::ComponentTransform::SetLocalRotationEuler(const float3& new_rotation_euler)
{
    local_rotation_euler = new_rotation_euler;
    float3 rotation = DegToRad(new_rotation_euler);    
    local_rotation = Quat::FromEulerXYZ(rotation.x, rotation.y, rotation.z).Normalized();
    Invalidate();
}

void Hachiko::ComponentTransform::SetGlobalRotationAxis(float3 x, float3 y, float3 z)
{
    matrix.SetCol3(0, x);
    matrix.SetCol3(1, y);
    matrix.SetCol3(2, z);

    SetGlobalTransform(matrix);
}

void Hachiko::ComponentTransform::SetGlobalPosition(const float3& new_position)
{
    position = new_position;
    // This already invalidates the transform
    SetGlobalTransform(position, rotation, scale);
}

void Hachiko::ComponentTransform::SetGlobalScale(const float3& new_scale)
{
    scale = new_scale;
    // This already invalidates the transform
    SetGlobalTransform(position, rotation, scale);
}

void Hachiko::ComponentTransform::SetGlobalRotation(const Quat& new_rotation)
{
    rotation = new_rotation;
    rotation_euler = RadToDeg(rotation.ToEulerXYZ());
    SetGlobalTransform(position, rotation, scale);
}

void Hachiko::ComponentTransform::SetGlobalRotationEuler(const float3& new_rotation_euler)
{
    rotation_euler = new_rotation_euler;
    float3 rad_rotation = DegToRad(new_rotation_euler);
    rotation = Quat::FromEulerXYZ(rad_rotation.x, rad_rotation.y, rad_rotation.z).Normalized();
    SetGlobalTransform(position, rotation, scale);
}

float3 Hachiko::ComponentTransform::AsConsistentEulerAngles(const float3& euler_angles) const
{
    float3 consistent_euler_angles = euler_angles;
    FixEulerAngleSymbol(consistent_euler_angles.x);
    FixEulerAngleSymbol(consistent_euler_angles.y);
    FixEulerAngleSymbol(consistent_euler_angles.z);
    return consistent_euler_angles;
}

void Hachiko::ComponentTransform::FixEulerAngleSymbol(float& euler_angle) const
{
    if (euler_angle == -180.f || euler_angle == -0.f)
    {
        euler_angle = abs(euler_angle);
    }
}


/***    GETTERS     ***/

const float4x4& Hachiko::ComponentTransform::GetLocalMatrix()
{
    UpdateTransform();
    return matrix_local;
}

const float3& Hachiko::ComponentTransform::GetGlobalPosition()
{
    UpdateTransform();
    return position;
}

const float3& Hachiko::ComponentTransform::GetGlobalScale()
{
    UpdateTransform();
    return scale;
}

const Quat& Hachiko::ComponentTransform::GetGlobalRotation()
{
    UpdateTransform();
    return rotation;
}

const float3& Hachiko::ComponentTransform::GetGlobalRotationEuler()
{
    UpdateTransform();
    return rotation_euler;
}

const float3& Hachiko::ComponentTransform::GetLocalPosition()
{
    UpdateTransform();
    return local_position;
}

const float3& Hachiko::ComponentTransform::GetLocalScale()
{
    UpdateTransform();
    return local_scale;
}

const Quat& Hachiko::ComponentTransform::GetLocalRotation()
{
    UpdateTransform();
    return local_rotation;
}

const float3& Hachiko::ComponentTransform::GetLocalRotationEuler()
{
    UpdateTransform();
    return local_rotation_euler;
}

const float3& Hachiko::ComponentTransform::GetFront()
{
    UpdateTransform();
    return front;
}

const float3& Hachiko::ComponentTransform::GetUp()
{
    UpdateTransform();
    return up;
}

const float3& Hachiko::ComponentTransform::GetRight()
{
    UpdateTransform();
    return right;
}

const float4x4& Hachiko::ComponentTransform::GetGlobalMatrix()
{
    UpdateTransform();
    return matrix;
}

/***    OPERATION METHODS   ***/

void Hachiko::ComponentTransform::UpdateTransform()
{
    if (dirty)
    {
        matrix_local = float4x4::FromTRS(local_position, local_rotation, local_scale);

        if (game_object->parent)
        {
            ComponentTransform* parent_transform = game_object->parent->GetTransform();
            parent_transform->UpdateTransform();
            matrix = parent_transform->matrix * matrix_local;
        }
        else
        {
            matrix = matrix_local;
        }

        // Update global matrix related variables
        right = matrix.WorldX().Normalized();
        up = matrix.WorldY().Normalized();
        front = matrix.WorldZ().Normalized();

        matrix.Decompose(position, rotation, scale);
        rotation.Normalize();
        rotation_euler = RadToDeg(rotation.ToEulerXYZ());

        changed = true;
        dirty = false;
    }   
}

void Hachiko::ComponentTransform::Invalidate()
{
    const std::vector<GameObject*>& children = game_object->children;
    
    dirty = true;
    for (GameObject* child : children)
    {
        child->GetTransform()->Invalidate();
    }
}

/**     SERIALIZATION    **/

void Hachiko::ComponentTransform::Save(YAML::Node& node) const
{
    node.SetTag("transform");
    node[TRANSFORM_POSITION] = local_position;
    node[TRANSFORM_ROTATION] = AsConsistentEulerAngles(local_rotation_euler);
    node[TRANSFORM_SCALE] = local_scale;
}

void Hachiko::ComponentTransform::Load(const YAML::Node& node)
{
    SetLocalPosition(node[TRANSFORM_POSITION].as<float3>());
    // Retrocompatibility: allow loading quaternions, decide based on amount of values
    if (node[TRANSFORM_ROTATION].size() > 3)
    {
        SetLocalRotation(node[TRANSFORM_ROTATION].as<Quat>());
    }
    else
    {
        SetLocalRotationEuler(node[TRANSFORM_ROTATION].as<float3>());
    }
    SetLocalScale(node[TRANSFORM_SCALE].as<float3>());
    SetLocalTransform(local_position, local_rotation, local_scale);
}

/**     GUI     **/

void Hachiko::ComponentTransform::DrawGui()
{
    static bool locked_scale = true;
    static bool debug_transforms = false;

    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
    {
        float3 position_local_editor = local_position;
        float3 scale_local_editor = local_scale;
        float3 rotation_local_editor = local_rotation_euler;

        if(Widgets::DragFloat3("Position",position_local_editor))
        {
            SetLocalPosition(position_local_editor);
        }

        if(Widgets::DragFloat3("Rotation",rotation_local_editor))
        {
            SetLocalRotationEuler(rotation_local_editor);
        }

        Widgets::DragFloat3Config scale_config;
        scale_config.min = float3(0.001f);
        if(DragFloat3("Scale",scale_local_editor, &scale_config))
        {
            SetLocalScale(scale_local_editor);
        }

        Widgets::Checkbox("Debug info", &debug_transforms);
        if (debug_transforms)
        {
            float3 position_editor = position;
            float3 rotation_editor = rotation_euler;
            float3 scale_editor = scale;

            ImGui::Separator();
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
            ImGui::TextWrapped("Global transform");
            ImGui::PopStyleColor();

            if (Widgets::DragFloat3("Position", position_editor))
            {
                SetGlobalPosition(position_editor);
            }

            if (Widgets::DragFloat3("Rotation", rotation_editor))
            {
                SetGlobalRotationEuler(rotation_editor);
            }

            if (DragFloat3("Scale", scale_editor, &scale_config))
            {
                SetGlobalScale(scale_editor);
            }

            ImGui::Separator();
            ImGui::TextWrapped("Local");
            for (int r = 0; r < 4; ++r)
            {
                const auto row = matrix_local.Row(r);
                ImGui::TextWrapped("%.2f, %.2f, %.2f, %.2f", row.x, row.y, row.z, row.w);
            }

            ImGui::Separator();
            ImGui::TextWrapped("Global");
            for (int r = 0; r < 4; ++r)
            {
                const auto row = matrix.Row(r);
                ImGui::TextWrapped("%.2f, %.2f, %.2f, %.2f", row.x, row.y, row.z, row.w);
            }
        }
    }
}


