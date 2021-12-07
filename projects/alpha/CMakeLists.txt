set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_ROOT}/examples)

project(alpha)

add_executable(alpha alpha1.cpp)
link_dependencies(alpha LIBS glad glfw nanovg eigen imgui nanogui)
##link_src(alpha)

#set(EXT_LIB_DIR C:/del/nanogui_example4/libs)
#target_include_directories(alpha PUBLIC ${EXT_LIB_DIR}/nanogui/include)
#target_include_directories(alpha PUBLIC ${EXT_LIB_DIR}/nanogui/include/nanogui)
#target_include_directories(alpha PUBLIC ${EXT_LIB_DIR}/nanogui/src)


#target_link_libraries(alpha PUBLIC
#    debug C:/del/nanogui_example4/libs/nanogui-build/${ARCH}-Debug-static/nanogui.lib
#    optimized C:/del/nanogui_example4/libs/nanogui-build/${ARCH}-Release-static/nanogui.lib)
#target_link_libraries(alpha PUBLIC
#    debug C:/del/nanogui_example4/libs/glfw-3.3.5-build/${ARCH}-Debug-static/glfw3.lib
#    optimized C:/del/nanogui_example4/libs/glfw-3.3.5-build/${ARCH}-Release-static/glfw3.lib)
#target_compile_definitions(alpha PUBLIC NANOGUI_BUILD)





# setup output dir for all exampels
list(APPEND TARGETS alpha)
setup_output_destination("${TARGETS}")

message("Adding alpha")




#set(EXT_LIB_DIR C:/del/nanogui_example4/libs)
#target_include_directories(alpha PUBLIC ${EXT_LIB_DIR}/nanogui/ext/coro)
#target_include_directories(alpha PUBLIC ${EXT_LIB_DIR}/nanogui/ext/eigen)
#target_include_directories(alpha PUBLIC ${EXT_LIB_DIR}/nanogui/ext/glad/include)
#target_include_directories(alpha PUBLIC ${EXT_LIB_DIR}/nanogui/ext/glfw/include)
#target_include_directories(alpha PUBLIC ${EXT_LIB_DIR}/nanogui/ext/nanovg/src)
#target_include_directories(alpha PUBLIC ${EXT_LIB_DIR}/nanogui/include)
#target_include_directories(alpha PUBLIC ${EXT_LIB_DIR}/nanogui/include/nanogui)
#target_include_directories(alpha PUBLIC ${EXT_LIB_DIR}/nanogui/src)

#target_link_libraries(alpha C:/del/nanogui_example4/libs/nanogui-build/x64-debug-static/nanogui.lib)
#target_link_libraries(alpha C:/del/nanogui_example4/libs/glfw-3.3.5-build/x64-Debug-static/glfw3.lib)
#target_compile_definitions(alpha PUBLIC NANOGUI_BUILD)
