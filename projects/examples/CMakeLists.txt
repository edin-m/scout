set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_ROOT}/examples)

project(examples)

add_executable(glad_example glad_example.cpp)
link_dependencies(glad_example LIBS opengl glad glfw)

add_executable(nanovg_example nanovg_example.cpp)
link_dependencies(nanovg_example LIBS opengl glad glfw nanovg)

add_executable(imgui_example imgui_example.cpp)
link_dependencies(imgui_example LIBS opengl glad glfw imgui)

set(LIB_BASE C:/QtProjects/scout2/external/lib_bin)
set(TARGET nanogui_example)
add_executable(nanogui_example nanogui_example.cpp)

#target_include_directories(${TARGET} PRIVATE ${LIB_BASE}/glad-build/include)
#target_link_libraries(${TARGET} ${LIB_BASE}/glad-build/lib/${ARCH}/${BUILD_TYPE}/glad.lib)

#target_include_directories(${TARGET} PRIVATE ${LIB_BASE}/eigen-build)
link_dependencies(${TARGET} LIBS glad glfw eigen nanovg nanogui)

#message(${EXT_LIB_ROOT}/glad-build/lib/${ARCH}/${BUILD_TYPE})
#target_include_directories(${TARGET} PRIVATE ${EXT_LIB_ROOT}/glad-build/lib/${ARCH}/${BUILD_TYPE})
#target_link_directories(${TARGET} PUBLIC ${EXT_LIB_ROOT}/glad-build/lib/${ARCH}/${BUILD_TYPE})
#target_link_libraries(${TARGET} glad.lib)

#target_include_directories(${TARGET} PRIVATE ${LIB_BASE}/glfw-3.3.5-build/include)
#target_link_libraries(${TARGET} ${LIB_BASE}/glfw-3.3.5-build/lib/${ARCH}/${BUILD_TYPE}/glfw3.lib)

#target_include_directories(${TARGET} PRIVATE ${LIB_BASE}/nanovg-build/include/nanovg)
#target_link_libraries(${TARGET} ${LIB_BASE}/nanovg-build/lib/${ARCH}/${BUILD_TYPE}/nanovg.lib)




#target_include_directories(${TARGET} PRIVATE ${EXT_LIB_ROOT}/nanogui-build/include/nanogui)
#target_include_directories(${TARGET} PRIVATE ${EXT_LIB_ROOT}/nanogui-build/include)
#target_link_libraries(${TARGET} ${EXT_LIB_ROOT}/nanogui-build/lib/${ARCH}/${BUILD_TYPE}/nanogui.lib)

#target_compile_definitions(${TARGET} PRIVATE NANOGUI_SHARED NVG_SHARED GLAD_GLAPI_EXPORT NANOGUI_GLAD)


list(APPEND TARGETS nanogui_example glad_example nanovg_example imgui_example)
setup_output_destination("${TARGETS}")










