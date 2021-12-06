set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_ROOT}/examples)

add_executable(glad_example glad_example.cpp)
link_dependencies(glad_example LIBS opengl glad glfw)

add_executable(nanovg_example nanovg_example.cpp)
link_dependencies(nanovg_example LIBS opengl glad glfw nanovg)

add_executable(imgui_example imgui_example.cpp)
link_dependencies(imgui_example LIBS opengl glad glfw imgui)
