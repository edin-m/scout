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

#include "alpha3/game.h"

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
InputSystem* isys;

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



    GraphicsScene* world = new GraphicsScene();
    world->SetShape(Shape::Null);


    Entity* e1 = new Entity(world);
    e1->SetShape(Shape::Box);
    e1->GetTransformation().SetPos(100, 100);

//    Entity* e2 = new Entity(nullptr);
    Entity* e2 = new Entity(world);
    e2->SetShape(Shape::Box);
    e2->GetTransformation().SetPos(200, 200);
    e2->GetTransformation().eulerRot.z = 0.0f;

    Entity* e3 = new Entity(e2);
    e3->SetShape(Shape::Box);
    e3->GetTransformation().SetPos(100, 100);
    e3->GetTransformation().eulerRot.z = 25.0f;

    world->GetBoundingBox();

    Vertices data = {
        { 0, 0 },
        { 0, 100 },
        { 100, 100 },
        { 50, 50 },
        { 100, 0 }
    };
    e2->GetShape().SetPoints(data);

    world->UpdateSelfAndChildTransform();

    std::cout << world->GetName() << world->GetBoundingBox() << std::endl;
    std::cout << e1->GetName() << e1->GetBoundingBox() << std::endl;
    std::cout << e2->GetName() << e2->GetBoundingBox() << std::endl;
    std::cout << e3->GetName() << e3->GetBoundingBox() << std::endl;



    auto& bb = e3->GetBoundingBox();
    glm::vec2 min = bb.GetMin();
    glm::vec2 max = bb.GetMax();
    std::cout << "min" << min
              << "max" << max << std::endl;








    isys = new InputSystem(world);
    RenderSystem* rsys = new RenderSystem(window, vg, world);



    std::cout << e1->GetName() << std::endl;
    std::cout << e2->GetName() << std::endl;


//    EntityComponentSystem* ecs = new EntityComponentSystem();
//    Entity* world = ecs->CreateEntity();

//    SceneRoot* root = new SceneRoot(world);


//    Entity* e1 = ecs->CreateEntity();
//    auto r1 = ecs->Assign(ComponentType::Render, e1);
//    auto s1 = ecs->Assign(ComponentType::Shape, e1);

//    Entity* e2 = ecs->CreateEntity();
//    auto r2 = ecs->Assign(ComponentType::Render, e2);
//    auto s2 = ecs->Assign(ComponentType::Shape, e2);

//    auto* rc1 = e1->GetComponent<RenderComponent>(ComponentType::Render);
//    rc1->Select(true);


//    RenderSystem* rsys = new RenderSystem(window, ecs, vg);
//    isys = new InputSystem(ecs);







    glfwSetCursorPosCallback(window,
            [](GLFWwindow *win, double x, double y) {
            screen->cursorPosCallbackEvent(x, y);

            double xpos, ypos;
            glfwGetCursorPos(win, &xpos, &ypos);

            isys->OnMouseMove(xpos, ypos);
        }
    );

    glfwSetMouseButtonCallback(window,
        [](GLFWwindow *win, int button, int action, int modifiers) {
            screen->mouseButtonCallbackEvent(button, action, modifiers);

            double xpos, ypos;
            glfwGetCursorPos(win, &xpos, &ypos);

            if (action == GLFW_PRESS) {
                isys->OnMouseDown(xpos, ypos);
            }
            else if (action == GLFW_RELEASE) {
                isys->OnMouseUp(xpos, ypos);
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
                isys->OnKeyDown(key, scancode, action, mods);
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
            ImGui::Text("xpos %d, ypos %d", (int)xpos, (int)ypos);


            {
                glm::vec2 out = world->GetTransformation().TranslateGlobalToLocal(
                            glm::vec2(xpos, ypos));
                ImGui::Text("screen to world: %.2f %.2f", out.x, out.y);
            }



            {
                bool e1hit = e1->IsHit(xpos, ypos);
                bool e2hit = e2->IsHit(xpos, ypos);
                ImGui::Text("hit e1: %d, hit e2: %d", e1hit, e2hit);

                if (e1hit) {
                    glm::vec2 local = e1->GetTransformation().TranslateGlobalToLocal(
                                glm::vec2 {xpos, ypos});
                    ImGui::Text("e1 local coords %f %f", local.x, local.y);
                }
//                if (e2hit) {
                    glm::vec2 local = e2->GetTransformation().TranslateGlobalToLocal(
                                glm::vec2 {xpos, ypos});
                    ImGui::Text("e2 local coords %f %f", local.x, local.y);
//                }



                Entity* selected = world->Select(xpos, ypos);
                if (selected) {
                    std::string& name = selected->GetName();
                    ImGui::InputText("Selected entity:", &name);
                }

            }

            ImGui::End();
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
        screen->drawContents();
        screen->drawWidgets();


        float dt = 1000/60.0f;


        rsys->Render();



        {
            // draw bounding boxes
            std::vector<Entity*> draw = {
                world, e1, e2, e3
            };

            for (auto& e : draw) {
                nvgSave(vg);
                NVGcolor c { 0, 155, 0, 255 };
                nvgStrokeColor(vg, c);
                nvgBeginPath(vg);

                auto& bb = e->GetBoundingBox();
                glm::vec4 min = e->GetTransformation().GetWorldMatrix()
                        * glm::vec4(bb.GetMin().xy, 0.0f, 1.0f);
                glm::vec4 max = e->GetTransformation().GetWorldMatrix()
                        * glm::vec4(bb.GetMax().xy, 0.0f, 1.0f);
                int w = max.x - min.x;
                int h = max.y - min.y;
                nvgRect(vg, min.x, min.y, w, h);

                nvgClosePath(vg);
                nvgStroke(vg);
            }


            auto& bb = e3->GetBoundingBox();
            glm::vec2 min = bb.GetMin();
            glm::vec2 max = bb.GetMax();
            std::cout << "min" << min
                      << "max" << max << std::endl;
        }




        glfwSwapBuffers(window);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();

    return 0;
}
