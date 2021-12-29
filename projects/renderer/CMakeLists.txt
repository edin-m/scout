set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_ROOT}/renderer)

project(renderer)

add_executable(renderer1 renderer1/main.cpp renderer1/renderer1.h
    renderer1/scene.h renderer1/renderer_math.h)
link_dependencies(renderer1 LIBS src opengl glad glfw eigen imgui2 nanovg glm cute)

# setup output dir for all exampels
list(APPEND TARGETS renderer1)
setup_output_destination("${TARGETS}")

message("Adding renderer")

