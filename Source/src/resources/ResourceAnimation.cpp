#pragma once
#include "core/hepch.h"

#include "ResourceAnimation.h"

Hachiko::ResourceAnimation::ResourceAnimation(UID id) : Resource(id, Type::ANIMATION) {}

Hachiko::ResourceAnimation::~ResourceAnimation() {}

const Hachiko::ResourceAnimation::Channel* Hachiko::ResourceAnimation::GetChannel(const std::string& name) const
{
    auto it = channels.find(name);
    return it != channels.end() ? &it->second : nullptr;
}

void Hachiko::ResourceAnimation::AddNode(const std::string& name) {}

void Hachiko::ResourceAnimation::RemoveNode(unsigned int index) {}