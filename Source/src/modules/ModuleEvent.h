#pragma once

#include <functional>
#include <map>

#include "Module.h"

#include "events/Event.h"

namespace Hachiko
{
    class ModuleEvent : public Module
    {
    public:
        void Publish(Event& evt);
        void Publish(Event::Type type);
        void Subscribe(Event::Type type, std::function<void(Event&)>& subscriber, UID id = 0);
        bool Unsubscribe(Event::Type type, UID id);

        bool CleanUp() override;

    private:
        std::multimap<Event::Type, std::pair<UID, std::function<void(Event&)>>> subscribers{};
    };
}
