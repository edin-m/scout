#ifndef ALPHA2_H
#define ALPHA2_H

#include <vector>
#include <unordered_map>

#include "GLFW/glfw3.h"

#include "Eigen/Core"
using namespace Eigen;

#include "nanovg/nanovg.h"

#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/matrix_access.hpp"
#include "glm/trigonometric.hpp"
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/io.hpp>

#include "cute/cute_math2d.h"

#define OWNING
#define BORROW

using Vertices = std::vector<glm::vec2>;

// ===========================

class EntityComponentSystem;
class Scene;
class SceneNode;
class Entity;

typedef int EntityId;

enum class ComponentType {
    Null,
    Render,
    Shape,
    Input,
    Sound,
    Music
};

static bool point_in_poly(Vertices& verts, glm::vec2& p)
{
    double minX = verts[0].x;
    double maxX = verts[0].x;
    double minY = verts[0].y;
    double maxY = verts[0].y;
    for (int i = 1 ; i < verts.size() ; i++) {
        glm::vec2& q = verts[i];
        minX = min(q.x, minX);
        maxX = max(q.x, maxX);
        minY = min(q.y, minY);
        maxY = max(q.y, maxY);
    }

    if (p.x < minX || p.x > maxX || p.y < minY || p.y > maxY) {
        return false;
    }

    bool in_poly = false;
    auto num_verts = verts.size();
    for (int i = 0, j = num_verts - 1; i < num_verts; j = i++) {
        double x1 = verts[i].x;
        double y1 = verts[i].y;
        double x2 = verts[j].x;
        double y2 = verts[j].y;

        if (((y1 > p.y) != (y2 > p.y)) &&
            (p.x < (x2 - x1) * (p.y - y1) / (y2 - y1) + x1))
            in_poly = !in_poly;
    }
    return in_poly;
}

/**
 * @brief The Transformation class
 */
class Transformation {
    friend class Entity;
    friend class RenderSystem;
public:
    Transformation() {
//        point_in_poly()
    }

//    void SetWorldPos(glm::vec2& vec) {
//        glm::vec2 local_coord = TranslateGlobalToLocal(vec);
//        local_pos = local_coord;
//        // update local mat
//    }

//    void SetLocalPos(glm::vec2& vec) {
//        local_pos = vec;
//    }

    glm::vec2 TranslateGlobalToLocal(glm::vec2& vec) {
        glm::mat4 inv = glm::inverse(world_mat);
        return inv * glm::vec4(vec.xy, 0.0f, 1.0f);
    }

//    glm::mat4 GetWorldMat() {
//        return world_mat;
//    }

    glm::mat4 GetLocalModelMatrix()
    {
        const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f),
                             glm::radians(eulerRot.x),
                             glm::vec3(1.0f, 0.0f, 0.0f));
        const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f),
                             glm::radians(eulerRot.y),
                             glm::vec3(0.0f, 1.0f, 0.0f));
        const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f),
                             glm::radians(eulerRot.z),
                             glm::vec3(0.0f, 0.0f, 1.0f));

        // Y * X * Z
        const glm::mat4 roationMatrix = transformY * transformX * transformZ;

        // translation * rotation * scale (also know as TRS matrix)
        return glm::translate(glm::mat4(1.0f), pos) *
                    roationMatrix *
                    glm::scale(glm::mat4(1.0f), scale);
    }

    glm::mat4 GetWorldMatrix() { return world_mat; }

    void SetPos(float x, float y) { pos.x = x; pos.y = y; }

//private:
    glm::vec3 pos = { 0.0f, 0.0f, 0.0f };
    glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
    glm::vec3 eulerRot = { 0.0f, 0.0f, 0.0f };

    glm::mat4 world_mat = glm::mat4(1.0f);
};

/**
 * @brief The Shape class
 */
class Shape {
    friend class Entity;
public:
    enum Type {
        Null,
        Box
    };

    Shape(Type type_ = Box)
        : type(type_)
    {
        data = {
            { 0, 0 },
            { 0, 100 },
            { 100, 100 },
            { 100, 0 }
        };
    }

    void SetPoints(std::vector<glm::vec2>& data) {
        this->data = data;
    }

    std::vector<glm::vec2>& GetPoints() {
        return data;
    }

    Type GetType() { return type; }

private:
    std::vector<glm::vec2> data;
    Type type;
};

/**
 * @brief The Entity class
 */
class Entity {
    friend class RenderSystem;
public:
    Entity(Entity* parent_ = nullptr)
        : parent(parent_)
    {
        if (parent) parent->AddChild(this);
    }

    Transformation& GetTransformation() { return transformation; }

    void SetShape(Shape::Type type) { shape.type = type; }

    void UpdateSelfAndChildTransform() {
        glm::mat4 world = glm::mat4(1.0f);
        if (parent) {
            world = parent->GetTransformation().GetWorldMatrix();
        }
        transformation.world_mat = world * transformation.GetLocalModelMatrix();

        for (auto&& child : children) {
            child->UpdateSelfAndChildTransform();
        }
    }

    Shape& GetShape() { return shape; }

    bool IsHit(float globalx, float globaly) {
        glm::vec2 local = transformation
                .TranslateGlobalToLocal(glm::vec2 { globalx, globaly });
        bool hit = point_in_poly(shape.GetPoints(), local);
        return hit;
    }

private:
    Entity* parent = nullptr;
    std::vector<Entity*> children;
    Transformation transformation;
    Shape shape;

    void AddChild(Entity* e) {
        e->parent = this;
        children.push_back(e);
    }
};


class GraphicsScene : public Entity {
public:

};





class InputSystem {
public:
    InputSystem(Entity* world_) : world(world_) { }

    void Update();

    void OnMouseDown(double x, double y) { }
    void OnMouseUp(double x, double y) { }
    void OnMouseMove(double x, double y) { }
    void OnKeyDown(int key, int scancode, int action, int mods) { }

private:
    Entity* world;
};


class RenderSystem {
public:
    RenderSystem(GLFWwindow* window_, NVGcontext* nvg_, Entity* world_)
        : window(window_), nvg(nvg_), world(world_) { }
    void Render();
private:
    GLFWwindow* window;
    Entity* world;
    NVGcontext* nvg;

    void RenderEntity(Entity* entity);
};

















/**
 * @brief The Component class
 */
class Component {
public:
    virtual ~Component() { }
    virtual ComponentType GetType() = 0;
protected:
};

/**
 * @brief The System class
 */
class System {
public:
    virtual ~System() { }

    virtual void Update(double dt) { }
};

///**
// * @brief The Entity class
// */
//class Entity {
//public:
//    Entity(Entity* parent_) : parent(parent_) { }
//    BORROW Component* GetComponent(ComponentType type);
//    bool HasA(ComponentType type);
//    void Assign(ComponentType type, Component* component);

//    template <typename T>
//    T* GetComponent(ComponentType type) {
//        Component* c = GetComponent(type);
//        return (T*) c;
//    }

//    Transformation& GetTransformation();
//protected:
//    OWNING std::unordered_map<ComponentType, Component*> components;
//    OWNING std::vector<Entity*> entities;
//    Transformation transformation;
//};

//class EntityComponentSystem : public System {
//public:
//    EntityComponentSystem() { }

////    BORROW Entity* CreateEntity();

//    BORROW Component* Assign(ComponentType type, Entity* e);

//    BORROW std::vector<Entity*> GetAll();
//    BORROW std::vector<Entity*> FindHavingAll(std::vector<ComponentType> types);
//protected:
//    EntityId GetNextId();
//private:
//    static inline int EntityIdGenerator = 0;
//    OWNING std::unordered_map<EntityId, Entity*> entities;

//    OWNING Component* CreateComponent(ComponentType type);
//};

///**
// * color, border, etc..
// * @brief The RenderComponent class
// */
//class RenderComponent : public Component {
//public:
//    ComponentType GetType() override { return ComponentType::Render; }
//    void Select(bool val) { selected = val; }
//    bool IsSelected() { return selected; }
//private:
//    bool selected;
//};

/////**
//// * @brief The TransformComponent class
//// */
////class TransformComponent : public Component {
////public:
////    ComponentType GetType() override { return ComponentType::Transform; }

////    void move(int dx, int dy);
////    void rotate(float deg);

////    Vector3f GetPos() { return pos; }
////    Vector3f GetScale() { return scale; }

////    void SetPos(Vector3f pos_) { pos = pos_; }
////    void SetScale(Vector3f scale_) { scale = scale_; }

////protected:
////    Vector3f pos;
////    Vector3f rot;
////    Vector3f scale;
////};

///**
// * @brief The InputComponent class
// */
//class InputComponent : public Component {
//public:
//    ComponentType GetType() override { return ComponentType::Input; }

//};

///**
// * @brief The ShapeComponent class
// */
//class ShapeComponent : public Component {
//public:
//    ComponentType GetType() override { return ComponentType::Shape; }
//};

///**
// * @brief The RenderSystem class
// */
//class RenderSystem : public System {
//public:
//    RenderSystem(GLFWwindow* window, EntityComponentSystem* ecs_, NVGcontext* nvg);
//    void Update(double dt) override;
//protected:
//    BORROW GLFWwindow* window;
//    BORROW NVGcontext* nvg;
//    BORROW EntityComponentSystem* ecs;
//};

///**
// * @brief The InputSystem class
// */
//class InputSystem : public System {
//public:
//    InputSystem(EntityComponentSystem* ecs_)
//        : ecs(ecs_) { }

//    void OnMouseDown(double x, double y) {
//        mouse_down = true;
//    }
//    void OnMouseMove(double x, double y) {
//        last_mouse_pos = Vector2d { x, y };
//    }
//    void OnMouseUp(double x, double y) {
//        mouse_down = false;
//    }
//    void OnKeyDown(int key, int scancode, int action, int mods) {

//    }
//private:
//    BORROW EntityComponentSystem* ecs;
//    bool mouse_down = false;
//    Vector2d last_mouse_pos;
//};

///**
// * @brief The SceneNode class
// */
//class SceneNode {
//public:
//    SceneNode(Entity* entity_, SceneNode* parent_ = nullptr)
//        : entity(entity_), parent(parent_) { }

//    void AddChild(SceneNode* child) {
//        child->parent = this;
//        children.push_back(child);
//    }

//protected:
//    OWNING std::vector<SceneNode*> children;
//    BORROW SceneNode* parent = nullptr;
//    BORROW Entity* entity = nullptr;
//};

///**
// * @brief The Scene class
// */
//class SceneRoot : public SceneNode {
//public:
//    SceneRoot(Entity* entity_, SceneNode* parent_ = nullptr)
//        : SceneNode(entity_, parent_) { }
//};

#endif // ALPHA2_H
