set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_ROOT}/examples)

project(alpha)

add_executable(alpha1 alpha1.cpp)
link_dependencies(alpha1 LIBS glad glfw nanovg eigen imgui nanogui)

add_executable(alpha2 alpha2.cpp alpha2/game.h alpha2/game.cpp)
link_dependencies(alpha2 LIBS opengl glad glfw eigen imgui nanovg nanogui glm)
link_src(alpha2)

# setup output dir for all exampels
list(APPEND TARGETS alpha1)
setup_output_destination("${TARGETS}")

message("Adding alpha")

