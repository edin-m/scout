#include "game.h"

int EntityComponentSystem::EntityIdGenerator = 0;

Entity* EntityComponentSystem::CreateEntity()
{
    Entity* entity = new Entity(GetNextId());
    entities[entity->GetId()] = entity;
    return entity;
}

std::vector<Entity*> EntityComponentSystem::GetAll()
{
    std::vector<Entity*> out;
    for (auto it = entities.begin(); it != entities.end(); ++it) {
        out.push_back(it->second);
    }
    return out;
}

EntityId EntityComponentSystem::GetNextId()
{
    EntityIdGenerator += 1;
    return EntityIdGenerator;
}

Component* Entity::GetComponent(ComponentType type)
{
    if (components.count(type) == 0)
        return nullptr;
    return components[type];
}


RenderSystem::RenderSystem(EntityComponentSystem *ecs_)
    : ecs(ecs_)
{

}

void RenderSystem::update(double dt)
{

}
