#ifndef RENDERER1_H
#define RENDERER1_H

#define OUT
#define IN
#define INOUT

#include "renderer_math.h"

Scene* scene;

int todata(int row, int col, int width, int height) {
    return row * width * 4 + col * 4;
}

Color RenderScene(int row, int col, int w, int h);

void render(INOUT char* out, int w, int h)
{
    scene = new Scene();
    scene->AddItem(new Sphere(glm::vec3(0, 0, 20), 200.0f));

    Ray camera(glm::vec3 { 0, 0, -5 }, glm::vec3 { 0, 0, 1});

    // int row = i*image_width*4;
    //

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            int pos = todata(j, i, w, h);
            Color color = RenderScene(i, j, w, h);
            out[pos + 0] = color.r;
            out[pos + 1] = color.g;
            out[pos + 2] = color.b;
            out[pos + 3] = color.a;
        }
    }
}

Color RenderScene(int x, int y, int w, int h)
{
    // ndc space [0,1]
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays?http://www.scratchapixel.com/lessons/3d-basic-rendering/computing-pixel-coordinates-of-3d-point?
    float ndcx = (x + 0.5) / w;
    float ndcy = (y + 0.5) / y;

    // screen space [-1,1]
    float nscx = 2*ndcx - 1;
    float nscy = 1 - 2*ndcy;

    // correct for aspect ratio
    float aspectr = w / (float) h;
    float pcx = nscx * aspectr;
    float pcy = nscy;

    Color bg_color { 100, 100, 100, 255 };
    Color color = bg_color;
    if (x == 100) {
        color.r = 255;
    }
    else if (y == 100) {
        color.g = 255;
    }

    glm::vec3 pos = { x, y, 0 };
    glm::vec3 dir = { 0, 0, 1 };
    Ray ray(pos, dir);

    Shape* shape = scene->FindShape(ray);
    if (shape != nullptr) {
        color.b = 255;
    }

    // create a ray from camera and plane + lens
    // hit test the scene
    // recurse the rays
    // shade the ray
    // return color

    return color;
}

#endif // RENDERER1_H
