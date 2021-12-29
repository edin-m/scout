#ifndef RENDERER_MATH_H
#define RENDERER_MATH_H

#define GLM_SWIZZLE
#define GLM_FORCE_SWIZZLE
#define GLM_SWIZZLE_XYZW
#define GLM_SWIZZLE_STQP
#define GLM_SWIZZLE_RGBA

#include "glm/glm.hpp"
#include "glm/gtc/round.hpp"
#include "glm/gtx/vec_swizzle.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/fast_square_root.hpp"
#include "glm/gtx/fast_trigonometry.hpp"
#include "glm/gtx/fast_exponential.hpp"
#include "glm/gtx/intersect.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_cross_product.hpp"
#include "glm/gtx/matrix_interpolation.hpp"
#include "glm/gtx/matrix_operation.hpp"
#include "glm/gtx/matrix_query.hpp"
#include "glm/gtx/normalize_dot.hpp"
#include "glm/gtx/orthonormalize.hpp"
#include "glm/gtx/projection.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "glm/gtx/polar_coordinates.hpp"
#include "glm/gtx/functions.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/gtc/color_space.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/extended_min_max.hpp"
#include "glm/gtx/perpendicular.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/rotate_normalized_axis.hpp"
#include "glm/gtx/string_cast.hpp"

#include <vector>
#include <algorithm>

using Color = glm::uvec4;

struct Ray {
    Ray(glm::vec3& pos_, glm::vec3& dir_)
        : origin(pos_), dir(glm::normalize(dir_))
    {

    }

    glm::vec3 origin;
    glm::vec3 dir;
    float distance = 99999.9f;
};

struct Light {
    virtual bool IsVisible() { return true; }
};

struct Shape {
    virtual bool IsHit(const Ray& ray) { return false; }
};

struct Sphere : public Shape {
    Sphere(glm::vec3& center_, float radius_)
        : center(center_), radius(radius_)
    {

    }

    bool IsHit(const Ray& ray) {
        glm::vec3 out_pos;
        glm::vec3 out_normal;
        bool result = glm::intersectRaySphere(
                    ray.origin, ray.dir, center, radius, out_pos, out_normal);
        return result;
    }

    glm::vec3 center;
    float radius;
};

class Scene {
public:
    void AddItem(Shape* shape) {
        shapes.push_back(shape);
    }

    Shape* FindShape(const Ray& ray) {
        for (auto& shape : shapes) {
            if (shape->IsHit(ray)) {
                return shape;
            }
        }
        return nullptr;
    }

private:
    std::vector<Shape*> shapes;
};

#endif // RENDERER_MATH_H
