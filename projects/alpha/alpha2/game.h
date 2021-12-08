#ifndef ALPHA2_H
#define ALPHA2_H

#include <vector>
#include <unordered_map>

#include "Eigen/Core"
using namespace Eigen;

#include "nanovg/nanovg.h"

#define OWNS
#define BORROWS

typedef int EntityId;

enum ComponentType {
    Render,
    Transform,
    Shape,
    Input,
    Sound,
    Music
};

class Component {
public:
    virtual ~Component() {}
    virtual ComponentType GetType() = 0;
protected:
};

class System {
public:
    virtual ~System() { }

    virtual void update(double dt) { }
};

class Entity {
public:
    Entity(EntityId id_) : id(id_) { }
    EntityId GetId() { return id; }
    Component* GetComponent(ComponentType type);
protected:
    OWNS std::unordered_map<ComponentType, Component*> components;
    EntityId id;
};

class EntityComponentSystem : public System {
public:
    EntityComponentSystem() {}

    Entity* CreateEntity();

    BORROWS std::vector<Entity*> GetAll();

protected:
    EntityId GetNextId();
private:
    static int EntityIdGenerator;
    OWNS std::unordered_map<EntityId, Entity*> entities;
};

class RenderComponent : public Component {
public:
    ComponentType GetType() override { return Render; }
};

class TransformComponent : public Component {
public:
    ComponentType GetType() override { return ComponentType::Transform; }
protected:
    Vector3f pos;
    Vector3f rot;
    Vector3f scale;
};

class InputComponent : public Component {
public:
    ComponentType GetType() override { return Input; }
};

class ShapeComponent : public Component {
public:
    ComponentType GetType() override { return Shape; }
};

class RenderSystem : public System {
public:
    RenderSystem(EntityComponentSystem* ecs_);
    void update(double dt) override;
protected:
     OWNS NVGcontext* vg;
     BORROWS EntityComponentSystem* ecs;
};

class InputSystem : public System {
public:
    void update(double dt) override {}
};

#endif // ALPHA2_H
