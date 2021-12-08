#ifndef ALPHA2_H
#define ALPHA2_H

#include <vector>
#include <unordered_map>

#include "GLFW/glfw3.h"

#include "Eigen/Core"
using namespace Eigen;

#include "nanovg/nanovg.h"

#define OWNING
#define BORROW

// ===========================

class EntityComponentSystem;
class Scene;
class SceneNode;

typedef int EntityId;

enum class ComponentType {
    Render,
    Transform,
    Shape,
    Input,
    Sound,
    Music
};

class Component {
public:
    virtual ~Component() { }
    virtual ComponentType GetType() = 0;
protected:
};

class System {
public:
    virtual ~System() { }

    virtual void Update(double dt) { }
};

class Entity {
public:
    Entity(EntityId id_) : id(id_) { }
    EntityId GetId() { return id; }
    BORROW Component* GetComponent(ComponentType type);
    bool HasA(ComponentType type);
    void Assign(ComponentType type, Component* component);

    template <typename T>
    T* GetComponent(ComponentType type) {
        Component* c = GetComponent(type);
        return (T*) c;
    }
protected:
    OWNING std::unordered_map<ComponentType, Component*> components;
    EntityId id;
};

class EntityComponentSystem : public System {
public:
    EntityComponentSystem() { }

    BORROW Entity* CreateEntity();

    BORROW Component* Assign(ComponentType type, Entity* e);

    BORROW std::vector<Entity*> GetAll();
    BORROW std::vector<Entity*> GetAllHaving(std::vector<ComponentType> types);
protected:
    EntityId GetNextId();
private:
    static int EntityIdGenerator;
    OWNING std::unordered_map<EntityId, Entity*> entities;

    OWNING Component* CreateComponent(ComponentType type);
};

/**
 * color, border, etc..
 * @brief The RenderComponent class
 */
class RenderComponent : public Component {
public:
    ComponentType GetType() override { return ComponentType::Render; }
    void Select(bool val) { selected = val; }
    bool IsSelected() { return selected; }
private:
    bool selected;
};

/**
 * @brief The TransformComponent class
 */
class TransformComponent : public Component {
public:
    ComponentType GetType() override { return ComponentType::Transform; }

    void move(int dx, int dy);
    void rotate(float deg);

    Vector3f GetPos() { return pos; }
    Vector3f GetScale() { return scale; }

    void SetPos(Vector3f pos_) { pos = pos_; }
    void SetScale(Vector3f scale_) { scale = scale_; }

protected:
    Vector3f pos;
    Vector3f rot;
    Vector3f scale;
};

/**
 * @brief The InputComponent class
 */
class InputComponent : public Component {
public:
    ComponentType GetType() override { return ComponentType::Input; }

};

/**
 * @brief The ShapeComponent class
 */
class ShapeComponent : public Component {
public:
    ComponentType GetType() override { return ComponentType::Shape; }
};

/**
 * @brief The RenderSystem class
 */
class RenderSystem : public System {
public:
    RenderSystem(GLFWwindow* window, EntityComponentSystem* ecs_, NVGcontext* nvg);
    void Update(double dt) override;
protected:
    BORROW GLFWwindow* window;
    BORROW NVGcontext* nvg;
    BORROW EntityComponentSystem* ecs;
};

/**
 * @brief The InputSystem class
 */
class InputSystem : public System {
public:
    InputSystem(EntityComponentSystem* ecs_, Scene* scene_)
        : ecs(ecs_), scene(scene_) { }

    void OnMouseDown(double x, double y) {
        mouse_down = true;
    }
    void OnMouseMove(double x, double y) {
        last_mouse_pos = Vector2d { x, y };
    }
    void OnMouseUp(double x, double y) {
        mouse_down = false;
    }
    void OnKeyDown(int key, int scancode, int action, int mods) {

    }
private:
    BORROW EntityComponentSystem* ecs;
    BORROW Scene* scene;
    bool mouse_down = false;
    Vector2d last_mouse_pos;
};

/**
 * @brief The SceneNode class
 */
class SceneNode {
public:
private:
    OWNING std::vector<SceneNode*> children;
};

class Scene {
public:
    Scene(SceneNode* root_) : root(root_) {}
private:
    OWNING SceneNode* root;
};

#endif // ALPHA2_H
