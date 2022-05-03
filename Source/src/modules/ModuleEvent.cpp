#include "core/hepch.h"
#include "ModuleEvent.h"

void Hachiko::ModuleEvent::Publish(Event& event)
{
    auto [start, end] = subscribers.equal_range(event.GetType());
    for (auto i = start; i != end; ++i)
    {
        i->second(event);
    }
}

void Hachiko::ModuleEvent::Publish(Event::Type type)
{
    Event evt(type);
    Publish(evt);
}

void Hachiko::ModuleEvent::Subscribe(Event::Type type, std::function<void(Event&)>& subscriber)
{
    subscribers.insert(std::make_pair(type, subscriber));
}
