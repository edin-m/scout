#include "game.h"

#include <iostream>

#include "GLFW/glfw3.h"





void Entity::Render(NVGcontext* nvg)
{
    glm::mat4& world = world_mat;

    NVGcolor color = NVGcolor { 0, 0, 255, 255 };
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
}
