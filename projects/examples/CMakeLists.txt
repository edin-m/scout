set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_ROOT}/examples)

add_executable(glad_example glad_example.cpp)
link_glfw(glad_example)
link_glad(glad_example)

add_executable(nanovg_example nanovg_example.cpp)
link_glfw(nanovg_example)
link_glad(nanovg_example)
link_nanovg(nanovg_example)
