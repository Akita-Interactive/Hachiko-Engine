#pragma once

#include <variant>

//event payloads created here because std::variant does not like forward declarations
#include "SelectionChangedEventPayload.h"
#include "FileAddedEventPayload.h"
#include "AssetsAddedEventPayload.h"
#include "MouseEventPayload.h"
#include "InputEventPayload.h"
#include "GameStateEventPayload.h"
#include "SceneLoadEventPayload.h"
#include "EditorHistoryEntryRestore.h"

namespace Hachiko
{
    //std::monostate is added just so we can have an empty std::variant
    using EventData = std::variant<std::monostate,
                                   SelectionChangedEventPayload, FileAddedEventPayload,
                                   AssetsAddedEventPayload, MouseEventPayload, InputEventPayload,
                                   GameStateEventPayload, SceneLoadEventPayload, EditorHistoryEntryRestore>;

    class Event
    {
    public:
        enum class Type
        {
            UNDEFINED = 0,
            SELECTION_CHANGED,
            TRANSFORM_CHANGED,
            FILE_ADDED,
            ASSETS_CHANGED,
            SCREEN_RESIZED,
            MOUSE_ACTION,
            INPUT,
            GAME_STATE,
            SCENE_LOADED,
            CREATE_EDITOR_HISTORY_ENTRY,
            RESTORE_EDITOR_HISTORY_ENTRY,
            COUNT
        };

        explicit Event(Type type);

        [[nodiscard]] Type GetType() const
        {
            return type;
        }

        template<typename T, typename... Args>
        void SetEventData(Args ...);

        template<typename T>
        T& GetEventData();

    private:
        Type type = Type::UNDEFINED;
        EventData data;
    };

    inline Event::Event(Type type) :
        type(type)
    {
        data = std::monostate{};
    }

    template<typename T, typename... Args>
    void Event::SetEventData(Args ... args)
    {
        data.emplace<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    T& Event::GetEventData()
    {
        return std::get<T>(data);
    }
} // namespace Hachiko
