set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_ROOT}/examples)

project(examples)

add_executable(glad_example glad_example.cpp)
link_dependencies(glad_example LIBS opengl glad glfw)

add_executable(nanovg_example nanovg_example.cpp)
link_dependencies(nanovg_example LIBS opengl glad glfw nanovg)

add_executable(imgui_example imgui_example.cpp)
link_dependencies(imgui_example LIBS opengl glad glfw imgui)

add_executable(nanogui_example nanogui_example.cpp)
link_dependencies(nanogui_example LIBS glad glfw eigen nanovg nanogui)
target_compile_definitions(nanogui_example PUBLIC NANOGUI_GLAD)

add_executable(libuv_example libuv_example.cpp)
link_dependencies(libuv_example LIBS opengl glad glfw imgui libuv)

add_executable(libuv_example2 libuv_example2.cpp person/person.pb.h person/person.pb.cc)
link_dependencies(libuv_example2 LIBS opengl glad glfw imgui libuv protobuf)

# setup output dir for all exampels
list(APPEND TARGETS nanogui_example glad_example imgui_example)
list(APPEND TARGETS nanovg_example libuv_example libuv_example2)
setup_output_destination("${TARGETS}")
