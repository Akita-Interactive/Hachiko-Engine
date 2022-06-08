#include "core/hepch.h"
#include "ModuleEvent.h"

void Hachiko::ModuleEvent::Publish(Event& evt)
{
    for (auto it = subscribers.begin(); it != subscribers.end(); ++it)
    {
        if(it->first == evt.GetType())
        {
            it->second.second(evt);
        }
    }
}

void Hachiko::ModuleEvent::Publish(Event::Type type)
{
    Event evt(type);
    Publish(evt);
}

void Hachiko::ModuleEvent::Subscribe(Event::Type type, std::function<void(Event&)>& subscriber, UID id)
{
    subscribers.insert(std::make_pair(type, std::make_pair(id, subscriber)));
}

bool Hachiko::ModuleEvent::Unsubscribe(Event::Type type, UID id)
{
    for (auto iterator = subscribers.begin(); iterator != subscribers.end(); ++iterator)
    {
        if(iterator->first == type && iterator->second.first == id)
        {
            subscribers.erase(iterator);
            return true;
        }
    }
    return false;
}

bool Hachiko::ModuleEvent::CleanUp()
{
    subscribers.clear();
    return true;
}
