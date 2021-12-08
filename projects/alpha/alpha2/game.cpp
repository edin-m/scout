#include "game.h"

#include <iostream>

#include "GLFW/glfw3.h"

#include <Eigen/Geometry>

int EntityComponentSystem::EntityIdGenerator = 0;

Entity* EntityComponentSystem::CreateEntity()
{
    Entity* entity = new Entity(GetNextId());
    entities[entity->GetId()] = entity;
    return entity;
}

Component* EntityComponentSystem::Assign(ComponentType type, Entity *e)
{
    if (e->HasA(type)) {
        return e->GetComponent(type);
    }

    Component* component = CreateComponent(type);
    e->Assign(type, component);
    return component;
}

Component* EntityComponentSystem::CreateComponent(ComponentType type)
{
    switch (type) {
    case ComponentType::Input:
        return new InputComponent();
    case ComponentType::Render:
        return new RenderComponent();
    case ComponentType::Transform:
        return new TransformComponent();
    case ComponentType::Shape:
        return new ShapeComponent();
    default:
        break;
    }
    return nullptr;
}

std::vector<Entity*> EntityComponentSystem::GetAll()
{
    std::vector<Entity*> out;
    for (auto it = entities.begin(); it != entities.end(); ++it) {
        out.push_back(it->second);
    }
    return out;
}

std::vector<Entity*> EntityComponentSystem::GetAllHaving(std::vector<ComponentType> types)
{
    std::vector<Entity*> out;
    for (auto it = entities.begin(); it != entities.end(); ++it) {
        Entity* e = it->second;
        for (int i = 0; i < types.size(); i++) {
            ComponentType type = types.at(i);
            if (e->HasA(type)) {
                out.push_back(e);
            }
        }
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

bool Entity::HasA(ComponentType type)
{
    return components.count(type) > 0;
}

void Entity::Assign(ComponentType type, Component *component)
{
    components[type] = component;
}


RenderSystem::RenderSystem(GLFWwindow* window_, EntityComponentSystem *ecs_, NVGcontext* nvg_)
    : window(window_)
    , ecs(ecs_)
    , nvg(nvg_)
{

}

void RenderSystem::Update(double dt)
{
    float pxRatio;
    double mx, my;
    int winWidth, winHeight, fbWidth, fbHeight;
    bool premult = false;
    glfwGetCursorPos(window, &mx, &my);
    glfwGetWindowSize(window, &winWidth, &winHeight);
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    // Calculate pixel ration for hi-dpi devices.
    pxRatio = (float)fbWidth / (float)winWidth;
    glViewport(0, 0, fbWidth, fbHeight);

    nvgBeginFrame(nvg, winWidth, winHeight, pxRatio);
    nvgSave(nvg);


    auto entities = ecs->GetAllHaving(
                { ComponentType::Render
                  , ComponentType::Shape
                  , ComponentType::Transform
                });
    for (int i = 0; i < entities.size(); i++) {
        Entity* e = entities.at(i);

        auto tc = e->GetComponent<TransformComponent>(ComponentType::Transform);
        Vector3f pos = tc->GetPos();
        Vector3f scale = tc->GetScale();

        auto rc = e->GetComponent<RenderComponent>(ComponentType::Render);

        NVGcolor color = NVGcolor { 0, 0, 255, 255 };
        if (rc->IsSelected()) {
            color = NVGcolor { 255, 0, 0, 255 };
        }

        nvgBeginPath(nvg);
        nvgFillColor(nvg, color);
        nvgRect(nvg, pos[0], pos[1], scale[0], scale[1]);
        nvgFill(nvg);
    }

    nvgRestore(nvg);
    nvgEndFrame(nvg);
}

void TransformComponent::move(int dx, int dy)
{
    float ddx = dx;
    float ddy = dy;
    pos += Vector3f { ddx, ddy, 0 };
}
