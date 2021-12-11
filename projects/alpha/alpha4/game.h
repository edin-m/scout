#ifndef ALPHA2_H
#define ALPHA2_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>

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
#include "cute/cute_c2.h"

#include "core/math/bbox.h"

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

struct AABB {
    glm::vec2 min { INT_MAX, INT_MAX };
    glm::vec2 max { INT_MIN, INT_MIN };

    static AABB Create(Vertices& verts) {
        AABB aabb;
        for (auto& v : verts) {
            aabb.Expand(v);
        }
        return aabb;
    }

    void Expand(AABB& aabb) {
        Expand(aabb.min);
        Expand(aabb.max);
    }

    void Expand(glm::vec2 v) {
        min.x = std::min(min.x, v.x);
        min.y = std::min(min.y, v.y);
        max.x = std::max(max.x, v.x);
        max.y = std::max(max.y, v.y);
    }

    friend std::ostream& operator<<(std::ostream& os, AABB& aabb) {
        os << "AABB(" << aabb.min << " " << aabb.max;
        return os;
    }

    float GetWidth() { return max.x - min.x; }
    float GetHeight() { return max.y - min.y; }

    bool Contains(float x, float y) {
        if (x >= min.x && x <= max.x &&
                y >= min.y && y <= max.y) {
            return true;
        }
        return false;
    }
};


template<typename ... Args>
static std::string string_format( const std::string& format, Args ... args )
{
    int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    auto size = static_cast<size_t>( size_s );
    auto buf = std::make_unique<char[]>( size );
    std::snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}


static bool point_in_bounding_box(Vertices& verts, glm::vec2& p) {
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

    return true;
}

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



class Entity {
public:
    Entity(Entity* parent = nullptr) {
        if (parent) parent->AddChild(this);
        name = string_format("Entity_%d", ++EntityId);
    }

    glm::mat4 RecalculateLocalModelMatrix()
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
        local_mat = glm::translate(glm::mat4(1.0f), pos) *
                    roationMatrix *
                    glm::scale(glm::mat4(1.0f), scale);
        UpdateChildrenWorld();
        return local_mat;
    }

    void SetData(Vertices& verts) {
        data.clear();
        for (const auto& v : verts) {
            data.push_back(v);
        }
        // calc aabb
    }

    void SetPos(glm::vec3& v) {
        pos = v;
        RecalculateLocalModelMatrix();
    }

    void SetRotation(glm::vec3& rot) {
        this->eulerRot = rot;
        RecalculateLocalModelMatrix();
    }

    void Render(NVGcontext* nvg);

    friend std::ostream& operator<<(std::ostream& os, Entity*& e) {
        os << "{ " << e->GetName()
           << " ";
        for (const auto& v : e->data) {
            os << v << " ";
        }
        os << " }";
        return os;
    }

    std::string& GetName() { return name; }

    void SetWorld(glm::mat4& mat, glm::mat4& inv) {
        world_mat = mat;
        inv_world_mat = inv;
        UpdateChildrenWorld();
    }

    const glm::mat4& GetWorld() const { return  world_mat; }

    glm::vec2 ToLocalCoords(glm::vec2& global_pos) {
        glm::vec4 inv = inv_world_mat * glm::vec4(global_pos.xy, 0.0f, 1.0f);
        return inv.xy;
    }

    glm::vec2 ToGlobalCoords(glm::vec2& local_pos) {
        glm::vec4 pos = world_mat * glm::vec4(local_pos.xy, 0.0f, 1.0f);
        return pos.xy;
    }

    void SetColor(NVGcolor color) { this->color = color; }

    AABB GetAABB() {
        glm::mat4 mat = world_mat * local_mat;
        AABB aabb;
        for (auto& v : data) {
            glm::vec4 vp = mat * glm::vec4(v.xy, 0.0f, 1.0f);
            aabb.Expand(vp.xy);
        }
        for (auto& c : children) {
            aabb.Expand(c->GetAABB());
        }
        return aabb;
    }

private:
    void AddChild(Entity* e) {
        e->parent = this;
        glm::mat4 world = world_mat * local_mat;
        e->SetWorld(world, glm::inverse(world));
        children.push_back(e);
    }

    void UpdateChildrenWorld() {
        glm::mat4 updated = world_mat * local_mat;
        glm::mat4 inv_updated = glm::inverse(updated);
        for (auto& child : children) {
            child->SetWorld(updated, inv_updated);
        }
    }

    glm::vec3 pos = glm::vec3(0.0f);
    glm::vec3 eulerRot = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    glm::mat4 local_mat = glm::mat4(1.0f);
    glm::mat4 world_mat = glm::mat4(1.0f);
    glm::mat4 inv_world_mat = glm::mat4(1.0f);

    Entity* parent;
    std::vector<Entity*> children;

    std::vector<glm::vec2> data;

    std::string name;
    inline static int EntityId = 0;

    NVGcolor color = NVGcolor { 0, 0, 255, 255 };
};















#endif // ALPHA2_H
