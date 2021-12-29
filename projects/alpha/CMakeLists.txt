set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_ROOT}/examples)

project(alpha)

add_executable(alpha1 alpha1.cpp)
link_dependencies(alpha1 LIBS glad glfw nanovg eigen imgui nanogui)

add_executable(alpha2 alpha2.cpp alpha2/game.h alpha2/game.cpp)
link_dependencies(alpha2 LIBS opengl glad glfw eigen imgui nanovg nanogui glm cute)
link_src(alpha2)

add_executable(alpha3 alpha3.cpp alpha3/game.h alpha3/game.cpp)
link_dependencies(alpha3 LIBS opengl glad glfw eigen imgui nanovg nanogui glm cute)
link_src(alpha3)

add_executable(alpha4 alpha4.cpp alpha4/game.h alpha4/game.cpp)
link_dependencies(alpha4 LIBS opengl glad glfw eigen imgui nanovg nanogui glm cute)
link_src(alpha4)

add_executable(alpha5 alpha5.cpp alpha5/game.h alpha5/game.cpp)
link_dependencies(alpha5 LIBS src opengl glad glfw eigen imgui2 nanovg nanogui glm cute)

# setup output dir for all exampels
list(APPEND TARGETS alpha1 alpha2 alpha3 alpha4 alpha5)
setup_output_destination("${TARGETS}")

message("Adding alpha")

