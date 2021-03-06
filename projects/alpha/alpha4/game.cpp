#include "game.h"

#include <iostream>

#include "GLFW/glfw3.h"





void Entity::Render(NVGcontext* nvg)
{
    glm::mat4& world = world_mat * local_mat;

    nvgSave(nvg);

    if (data.size() > 0) {
        nvgFillColor(nvg, color);
        nvgBeginPath(nvg);

        glm::vec4 first = world * glm::vec4(data[0].xy, 0.0f, 1.0f);
        nvgMoveTo(nvg, first.x, first.y);
        for (int i = 0; i < data.size(); i++) {
            int idx = (i+1)%data.size();
            glm::vec2& d = data[idx];
            glm::vec4 v = glm::vec4(d.xy, 0.0f, 1.0f);
            glm::vec4 wpos = world * v;
            nvgLineTo(nvg, wpos.x, wpos.y);
        }

        nvgClosePath(nvg);
        nvgFill(nvg);

        AABB aabb = GetAABB();
        nvgStrokeColor(nvg, nvgRGB(0, 0, 0));

        float x = aabb.min.x, y = aabb.min.y;
        float w = aabb.GetWidth(), h = aabb.GetHeight();

        nvgBeginPath(nvg);

        nvgRect(nvg, x, y, w, h);
        nvgClosePath(nvg);

        nvgStroke(nvg);
    }

    nvgRestore(nvg);

    for (auto& e : children) {
        e->Render(nvg);
    }

}

void Entity::RenderModifiers(NVGcontext* nvg)
{
    nvgSave(nvg);

    AABB aabb = GetAABB();

    nvgReset(nvg);

    NVGcolor c = nvgRGBA(255, 0, 0, 155);
    nvgStrokeColor(nvg, c);
    nvgFillColor(nvg, c);

    nvgBeginPath(nvg);

    auto& tl = aabb.GetTopLeft();
    nvgCircle(nvg, tl.x, tl.y, 5.0f);

    auto& tr = aabb.GetTopRight();
    nvgCircle(nvg, tr.x, tr.y, 5.0f);

    auto& bl = aabb.GetBottomLeft();
    nvgCircle(nvg, bl.x, bl.y, 5.0f);

    auto& br = aabb.GetBottomRight();
    nvgCircle(nvg, br.x, br.y, 5.0f);

    nvgFill(nvg);

    nvgRestore(nvg);
}
