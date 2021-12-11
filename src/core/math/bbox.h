#ifndef BBOX_H
#define BBOX_H

#include <ostream>
#include <vector>

#include "glm/glm.hpp"

#include "cute/cute_c2.h"

class BBox {
public:
    BBox() { }

    BBox(glm::vec2& p1, glm::vec2& p2) {
        Extend(p1, p2);
    }

    BBox(std::vector<glm::vec2>& vertices) {
        for (auto& v : vertices) {
            Extend(v);
        }
    }

    void Extend(glm::vec2& p) {
        min.x = std::min(p.x, min.x);
        min.y = std::min(p.y, min.y);
        max.x = std::max(p.x, max.x);
        max.y = std::max(p.y, max.y);
    }

    void Extend(const BBox& box) {
        Extend(box.GetMin());
        Extend(box.GetMax());
    }

    void Extend(glm::vec2& p1, glm::vec2& p2) {
        Extend(p1);
        Extend(p2);
    }

    glm::vec2 GetMin() const { return min; }
    glm::vec2 GetMax() const { return max; }

    bool Contains(glm::vec2& p) {
        if (p.x >= min.x && p.x <= max.x &&
                p.y >= min.y && p.y <= max.y) {
            return true;
        }
        return false;
    }

    BBox ConvertedToSpace(glm::mat4& mat) {
        glm::vec4 min = mat * glm::vec4(GetMin().xy, 0.0f, 1.0f);
        glm::vec4 max = mat * glm::vec4(GetMax().xy, 0.0f, 1.0f);
        auto p1 = glm::vec2(min.xy), p2 = glm::vec2(max.xy);
        BBox b(p1, p2);
        return b;
    }

    friend std::ostream& operator<<(std::ostream& ostream, BBox& bbox) {
        ostream
                << "BBox (" << bbox.min.x << " " << bbox.min.y << ") "
                << "(" << bbox.max.x << " " << bbox.max.y << ")";

        return ostream;
    }

private:
    glm::vec2 min { INT_MAX, INT_MAX };
    glm::vec2 max { INT_MIN, INT_MIN };
};

#endif // BBOX_H
