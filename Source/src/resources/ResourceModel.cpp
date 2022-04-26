#include "core/hepch.h"
#include "ResourceModel.h"
#include "ResourceMesh.h"

Hachiko::ResourceModel::ResourceModel(UID uid)
	: Resource(uid, Type::MODEL)
{}

Hachiko::ResourceModel::~ResourceModel()
{
    for (auto child : child_nodes)
    {
        RELEASE(child);
    }
}
