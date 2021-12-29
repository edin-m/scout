#include "glad/glad.h"
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"


#include "nanovg/nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg/nanovg_gl.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <cstdio>
#include <cstdint>
#include <iostream>
#include <thread>
#include <chrono>

#include "renderer1.h"

//using namespace nanogui;

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

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }

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
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSwapInterval(0);
    glfwSwapBuffers(window);

    // Create nanogui gui
    bool enabled = true;







    float pxRatio;
    double mx, my;
    int winWidth, winHeight, fbWidth, fbHeight;
    bool premult = false;







    glfwSetCursorPosCallback(window,
            [](GLFWwindow *win, double x, double y) {

            double xpos, ypos;
            glfwGetCursorPos(win, &xpos, &ypos);

//            isys->OnMouseMove(xpos, ypos);
        }
    );

    glfwSetMouseButtonCallback(window,
        [](GLFWwindow *win, int button, int action, int modifiers) {

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
        }
    );

    glfwSetDropCallback(window,
        [](GLFWwindow *, int count, const char **filenames) {
        }
    );

    glfwSetScrollCallback(window,
        [](GLFWwindow* win, double x, double y) {
            ImGui_ImplGlfw_ScrollCallback(win, x, y);
       }
    );

    glfwSetFramebufferSizeCallback(window,
        [](GLFWwindow *, int width, int height) {
        }
    );

    bool show_demo_window = true;
    bool show_another_window = false;



    NVGcontext* nvg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);

    struct NvgImage {
        int width;
        int height;
        char* data;

        NvgImage(int w, int h) {
            width = w; height = h;
            data = new char[w*h];
        }

        ~NvgImage() {
            delete data;
        }
    };


    NvgImage nvgimage(500, 500);


    // Create a OpenGL texture identifier
    GLuint image_width = 512;
    GLuint image_height = 512;
    GLchar* image_data = new GLchar[image_width*image_height*4];
    GLuint image_texture;

    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 512; j++) {
            int pos = image_width*100;
        }
    }

    int col = 40;
    for (int i = 0; i < 512; i++) {
        int row = i*image_width*4;
        int pos = row+col;

        image_data[pos] = 0;
        image_data[pos+1] = 255;
        image_data[pos+2] = 0;
        image_data[pos+3] = 255;
    }


    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);


    render(image_data, image_width, image_height);

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


        bool is_rendering = false;


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







            static bool mouseMove = false;
            static glm::vec2 mouseMovePos = { 0, 0 };
            static glm::vec2 objOffset = { 0, 0 };

            ImGui::End();

            {
                ImGui::Begin("Entity Transformation");

                int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
                if (!ImGui::GetIO().WantCaptureMouse) {

                    if (state == GLFW_PRESS) {
                    }
                    else if (state == GLFW_RELEASE) {
                    }
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




        glBindTexture(GL_TEXTURE_2D, image_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
        ImGui::Begin("OpenGL Texture Text");
        ImGui::Text("pointer = %p", image_texture);
        ImGui::Text("size = %d x %d", image_width, image_height);
        if (ImGui::Button("Render")) {
            render(image_data, image_width, image_height);
        }
        ImGui::Image((void*)(intptr_t)image_texture, ImVec2(image_width, image_height));
        ImGui::End();




        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


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

        glfwSwapBuffers(window);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();

    return 0;
}
