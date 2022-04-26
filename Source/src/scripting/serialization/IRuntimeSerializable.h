#include "HachikoApiDefine.h"
#include "scripting/serialization/SerializedField.h"
#include <unordered_map>
#include <string>

namespace Hachiko::Scripting
{
class HACHIKO_API IRuntimeSerializable
{
public:
    virtual void SerializeTo (
        std::unordered_map<std::string, SerializedField>& serialized_fields
    ) = 0;

    virtual void DeserializeFrom(
        std::unordered_map<std::string, SerializedField>& serialized_fields
    ) = 0;

    virtual void ShowInEditor() {};
};
} // namespace Hachiko::Scripting