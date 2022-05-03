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
        void Publish(Event& event);
        void Publish(Event::Type type);
        void Subscribe(Event::Type type, std::function<void(Event&)>& subscriber);

    private:
        std::multimap<Event::Type, std::function<void(Event&)>> subscribers{};
    };
}
