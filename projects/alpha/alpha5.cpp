/*
    src/example3.cpp -- C++ version of an example application that shows
    how to use nanogui in an application with an already created and managed
    glfw context.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

// GLFW
//
#include "glad/glad.h"

#include <GLFW/glfw3.h>


#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include <stdio.h>

#define NANOGUI_GLAD
#include <nanogui/nanogui.h>

#include <iostream>
#include <thread>
#include <chrono>

#include "nanovg/nanovg.h"

#include "alpha5/game.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace nanogui;

enum test_enum {
    Item1 = 0,
    Item2,
    Item3
};

bool bvar = true;
int ivar = 12345678;
double dvar = 3.1415926;
float fvar = (float)dvar;
std::string strval = "A string";
test_enum enumval = Item2;
Color colval(0.5f, 0.5f, 0.7f, 1.f);

Screen *screen = nullptr;

#define TRANSFORM(A) ((TransformComponent*)A)

int main(int /* argc */, char ** /* argv */) {

    glfwInit();

    glfwSetTime(0);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    // Create a GLFWwindow object
    GLFWwindow* window = glfwCreateWindow(1400, 800, "example3", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

#if defined(NANOGUI_GLAD)
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        throw std::runtime_error("Could not initialize GLAD!");
    glGetError(); // pull and ignore unhandled errors like GL_INVALID_ENUM
#endif

    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
//    ImGui::StyleColorsClassic();
//    ImGui::StyleColorsLight(ew);

    const char* glsl_version = "#version 130";
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);




    glClearColor(0.2f, 0.25f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Create a nanogui screen and pass the glfw pointer to initialize
    screen = new Screen();
    screen->initialize(window, true);
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSwapInterval(0);
    glfwSwapBuffers(window);

    // Create nanogui gui
    bool enabled = true;
    FormHelper *gui = new FormHelper(screen);
    ref<Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Form helper example");
    gui->addGroup("Basic types");
    gui->addVariable("bool", bvar)->setTooltip("Test tooltip.");
    gui->addVariable("string", strval);

    gui->addGroup("Validating fields");
    gui->addVariable("int", ivar)->setSpinnable(true);
    gui->addVariable("float", fvar)->setTooltip("Test.");
    gui->addVariable("double", dvar)->setSpinnable(true);

    gui->addGroup("Complex types");
    gui->addVariable("Enumeration", enumval, enabled)->setItems({ "Item 1", "Item 2", "Item 3" });
    gui->addVariable("Color", colval)
       ->setFinalCallback([](const Color &c) {
             std::cout << "ColorPicker Final Callback: ["
                       << c.r() << ", "
                       << c.g() << ", "
                       << c.b() << ", "
                       << c.w() << "]" << std::endl;
         });

    gui->addGroup("Other widgets");
    gui->addButton("A button", []() { std::cout << "Button pressed." << std::endl; })->setTooltip("Testing a much longer tooltip, that will wrap around to new lines multiple times.");;

    screen->setVisible(true);
    screen->performLayout();
//    nanoguiWindow->center();








    NVGcontext* vg = screen->nvgContext();



    float pxRatio;
    double mx, my;
    int winWidth, winHeight, fbWidth, fbHeight;
    bool premult = false;







    glfwSetCursorPosCallback(window,
            [](GLFWwindow *win, double x, double y) {
            screen->cursorPosCallbackEvent(x, y);

            double xpos, ypos;
            glfwGetCursorPos(win, &xpos, &ypos);

//            isys->OnMouseMove(xpos, ypos);
        }
    );

    glfwSetMouseButtonCallback(window,
        [](GLFWwindow *win, int button, int action, int modifiers) {
            screen->mouseButtonCallbackEvent(button, action, modifiers);

            double xpos, ypos;
            glfwGetCursorPos(win, &xpos, &ypos);

            if (action == GLFW_PRESS) {
//                isys->OnMouseDown(xpos, ypos);
            }
            else if (action == GLFW_RELEASE) {
//                isys->OnMouseUp(xpos, ypos);
            }
        }
    );

    glfwSetKeyCallback(window,
        [](GLFWwindow *win, int key, int scancode, int action, int mods) {
        ImGui_ImplGlfw_KeyCallback(win, key, scancode, action, mods);
            screen->keyCallbackEvent(key, scancode, action, mods);
            if (key == GLFW_KEY_Q) {
                std::exit(0);
            }
            if (action == GLFW_PRESS) {
//                isys->OnKeyDown(key, scancode, action, mods);
            }
        }
    );

    glfwSetCharCallback(window,
        [](GLFWwindow *win, unsigned int codepoint) {
        ImGui_ImplGlfw_CharCallback(win, codepoint);
            screen->charCallbackEvent(codepoint);
        }
    );

    glfwSetDropCallback(window,
        [](GLFWwindow *, int count, const char **filenames) {
            screen->dropCallbackEvent(count, filenames);
        }
    );

    glfwSetScrollCallback(window,
        [](GLFWwindow* win, double x, double y) {
            ImGui_ImplGlfw_ScrollCallback(win, x, y);
            screen->scrollCallbackEvent(x, y);
       }
    );

    glfwSetFramebufferSizeCallback(window,
        [](GLFWwindow *, int width, int height) {
            screen->resizeCallbackEvent(width, height);
        }
    );

    bool show_demo_window = true;
    bool show_another_window = false;


    Vertices data = {
        { -70, -40 },
        { -70, 40 },
        { 70, 40 },
        { 70, -40 },
    };


    Entity* world = new Entity();
    world->SetData(data);
    world->SetPos(glm::vec3 { 100, 100, 0 });
    world->SetRotation(glm::vec3 { 0, 0, 15 });
    world->SetColor(nvgRGBA(50, 255, 255, 255));


    Entity* e2 = new Entity(world);
    e2->SetData(data);
    e2->SetPos(glm::vec3 { 100, 100, 0 });
    e2->SetRotation(glm::vec3 { 0, 0, 0 });
    e2->SetColor(nvgRGBA(155, 155, 50, 255));


    Entity* e3 = new Entity(world);
    e3->SetData(data);
    e3->SetPos(glm::vec3 { 200, 200, 0 });
    e3->SetColor(nvgRGBA(255, 155, 155, 155));


    Entity* e4 = new Entity(e2);
    e4->SetData(data);
    e4->SetPos(glm::vec3 { 150, -20, 0 });
    e4->SetColor(nvgRGBA(200, 255, 200, 255));



    std::cout << world->GetWorld() << std::endl;
    std::cout << world->GetAABB() << std::endl;



    std::string text_input;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    // Game loop
    while (!glfwWindowShouldClose(window)) {
        // Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();





        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        static Entity* selectedEntity = nullptr;




        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            Point mousep = { xpos, ypos };
            ImGui::Text("xpos %d, ypos %d", (int)xpos, (int)ypos);




            {
                glm::vec2 local = world->ToLocalCoords(glm::vec2 { xpos, ypos });
                ImGui::Text("world local coords %.2f %.2f hit: %d %d", local.x, local.y,
                            world->HitTestAABB(mousep),
                            world->HitTest(mousep));
            }


            {
                glm::vec2 local = e2->ToLocalCoords(glm::vec2 { xpos, ypos });
                ImGui::Text("e2 local coords %.2f %.2f aabbhit:%d %d", local.x, local.y,
                            e2->HitTestAABB(mousep),
                            e2->HitTest(mousep));
            }


            {
                glm::vec2 local = e3->ToLocalCoords(glm::vec2 { xpos, ypos });
                ImGui::Text("e3 local coords %.2f %.2f aabb hit:%d %d", local.x, local.y,
                            e3->HitTestAABB(mousep),
                            e3->HitTest(mousep));
            }


            {
                glm::vec2 local = e4->ToLocalCoords(glm::vec2 { xpos, ypos });
                ImGui::Text("e4 local coords %.2f %.2f", local.x, local.y);
                glm::vec2 global = e4->ToWorldCoords(local);
                ImGui::Text("e4 global coords %.2f %.2f; aabb hit:%d %d", global.x, global.y,
                            e4->HitTestAABB(mousep),
                            e4->HitTest(mousep));


//                static float vec4a[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
//                static glm::vec3 v = { 0, 0, 0 };
//                float* vptr = glm::value_ptr(v);
//                ImGui::DragFloat4("drag float", vptr, 0.0005f);
//                ImGui::DragFloat4("input float3", vec4a);
            }


            static bool mouseMove = false;
            static glm::vec2 mouseMovePos = { 0, 0 };
            static glm::vec2 objOffset = { 0, 0 };

            ImGui::End();

            {
                ImGui::Begin("Entity Transformation");

                Point p = { xpos, ypos };

                int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
                if (!ImGui::GetIO().WantCaptureMouse) {

                    if (selectedEntity) {
                        selectedEntity->OnMouseMove(p);
                    }

                    if (state == GLFW_PRESS) {
                            selectedEntity = world->HitTest(p);
                            if (selectedEntity) {
                                selectedEntity->OnMouseDown(p);
                            }
                    }
                    else if (state == GLFW_RELEASE) {
                        if (selectedEntity) {
                            selectedEntity->OnMouseUp(p);
                        }
                    }
                }

                if (selectedEntity) {
                    ImGui::Text("Selected %s", selectedEntity->GetName().c_str());

                    ImGui::Text("Position (global)");

                    glm::vec2 lpos = selectedEntity->GetPos().xy;
                    glm::vec2 wpos = selectedEntity->ToWorldCoords(lpos);
                    ImGui::DragFloat2("Global pos:", glm::value_ptr(wpos));
                    lpos = selectedEntity->ToLocalCoords(wpos);
                    selectedEntity->SetPos(lpos);

                    ImGui::Text("Position (local)");
                    glm::vec3 pos = selectedEntity->GetPos();
                    ImGui::DragFloat3("Local pos:", glm::value_ptr(pos));
                    selectedEntity->SetPos(pos);

                    ImGui::Text("Rotation");
                    float rotz = selectedEntity->GetRot();
                    ImGui::DragFloat("Z rotation", &rotz);
                    selectedEntity->RotateZ(rotz);

                    ImGui::Text("Scale:");
                    glm::vec3 scale = selectedEntity->GetScale();
                    ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.01f);
                    selectedEntity->SetScale(scale);
                }

                ImGui::End();
            }
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }





        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());



        // Draw nanogui
//        screen->drawContents();
//        screen->drawWidgets();


        float dt = 1000/60.0f;






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

        nvgBeginFrame(vg, winWidth, winHeight, pxRatio);


//        nvgStrokeColor(vg, nvgRGBA(155, 0, 0, 255));
//        nvgRect(vg, 0, 0, 100, 100);
//        nvgStroke(vg);

//        nvgStrokeColor(vg, nvgRGBA(255, 0, 0, 255));

////        nvgBeginPath(vg);
////        nvgMoveTo(vg, 0, 0);
////        nvgLineTo(vg, 100, 100);
////        nvgStroke(vg);

//        nvgBeginPath(vg);
//        nvgMoveTo(vg, 0, 0);
//        nvgLineTo(vg, 100, 100);
//        nvgLineTo(vg, 200, 100);
//        nvgLineTo(vg, 200, 200);
////        nvgClosePath(vg);
//        nvgStroke(vg);

        world->Render(vg);

        if (selectedEntity) {
            selectedEntity->RenderModifiers(vg);
        }

        nvgEndFrame(vg);


        glfwSwapBuffers(window);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();

    return 0;
}
