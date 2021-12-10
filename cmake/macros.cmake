
#macro(link_sdl2 TARGET_NAME)
#    target_include_directories(${TARGET_NAME} PUBLIC ${GIT_ROOT}/external/lib_bin/SDL2-2.0.14/include)
#	#target_include_directories(${TARGET_NAME} PUBLIC ${GIT_ROOT}/external/lib_bin/SDL2-2.0.14/include/SDL2)
#    target_link_directories(${TARGET_NAME} PUBLIC ${GIT_ROOT}/external/lib_bin/SDL2-2.0.14/lib/x64)
#    target_link_libraries(${TARGET_NAME} SDL2 SDL2main)
#endmacro()

#macro(link_sdl2_image TARGET_NAME)
#    target_include_directories(${TARGET_NAME} PUBLIC ${GIT_ROOT}/external/lib_bin/SDL2_image-2.0.5/include)
#    target_link_directories(${TARGET_NAME} PUBLIC ${GIT_ROOT}/external/lib_bin/SDL2_image-2.0.5/lib/x64)
#    target_link_libraries(${TARGET_NAME} SDL2_image)
#endmacro()

#macro(link_sdl2_ttf TARGET_NAME)
#    target_include_directories(${TARGET_NAME} PUBLIC ${GIT_ROOT}/external/lib_bin/SDL2_ttf-2.0.15/include)
#    target_link_directories(${TARGET_NAME} PUBLIC ${GIT_ROOT}/external/lib_bin/SDL2_ttf-2.0.15/lib/x64)
#    target_link_libraries(${TARGET_NAME} SDL2_ttf)
#endmacro()

macro(set_arch)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ARCH x64)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(ARCH x86)
    endif()
    message("Arch:" ${ARCH})
endmacro()

macro(set_build_type)
    if (CMAKE_BUILD_TYPE STREQUAL Debug)
        set(BUILD_TYPE Debug)
    else()
        set(BUILD_TYPE Release)
    endif()
    message("Build type: " ${BUILD_TYPE})
endmacro()

# todo: use "${TARGET_NAME}"
# todo: use "${GIT_ROOT}/..."
# todo: use target_include_directories
# todo: use ${ARCH}

macro(link_opengl TARGET_NAME)
    target_link_libraries(${TARGET_NAME} PRIVATE opengl32.lib)
endmacro()

macro(link_glfw TARGET_NAME)
    target_include_directories(${TARGET_NAME} PUBLIC ${EXT_LIB_ROOT}/glfw-3.3.5-build/include)
    target_link_directories(${TARGET_NAME} PRIVATE
        debug ${EXT_LIB_ROOT}/glfw-3.3.5-build/lib/${ARCH}/Debug
        optimized ${EXT_LIB_ROOT}/glfw-3.3.5-build/lib/${ARCH}/Release)
    target_link_libraries(${TARGET_NAME} PRIVATE glfw3.lib)
endmacro()

macro(link_glad TARGET_NAME)
    target_include_directories(${TARGET_NAME} PRIVATE ${EXT_LIB_ROOT}/glad-build/include)
    target_link_directories(${TARGET_NAME} PRIVATE
        debug ${EXT_LIB_ROOT}/glad-build/lib/${ARCH}/Debug
        optimized ${EXT_LIB_ROOT}/glad-build/lib/${ARCH}/Release)
    target_link_libraries(${TARGET_NAME} PRIVATE glad.lib)
endmacro()

macro(link_nanovg TARGET_NAME)
    message("setting up" ${TARGET_NAME})
    target_include_directories(${TARGET_NAME} PUBLIC ${EXT_LIB_ROOT}/nanovg-build/include)
    target_include_directories(${TARGET_NAME} PUBLIC ${EXT_LIB_ROOT}/nanovg-build/include/nanovg)
    target_link_libraries(${TARGET_NAME} PRIVATE
        debug ${EXT_LIB_ROOT}/nanovg-build/lib/${ARCH}/Debug/nanovg.lib
        optimized ${EXT_LIB_ROOT}/nanovg-build/lib/${ARCH}/Release/nanovg.lib)
endmacro()

macro(link_eigen TARGET_NAME)
    target_include_directories(${TARGET_NAME} PRIVATE "${GIT_ROOT}/external/lib_bin/eigen-build")
endmacro()

macro(link_nanogui TARGET_NAME)
    target_include_directories(${TARGET_NAME} PUBLIC ${EXT_LIB_ROOT}/glfw-3.3.5-build/include)
    target_include_directories(${TARGET_NAME} PRIVATE ${EXT_LIB_ROOT}/glad-build/include)
    target_include_directories(${TARGET_NAME} PUBLIC ${EXT_LIB_ROOT}/nanovg-build/include)
    target_include_directories(${TARGET_NAME} PUBLIC ${EXT_LIB_ROOT}/nanovg-build/include/nanovg)

#    set(EXT_LIB_DIR C:/del/nanogui_example4/libs)
#    target_include_directories(${TARGET_NAME} PUBLIC ${EXT_LIB_DIR}/nanogui/include)
#    target_include_directories(${TARGET_NAME} PUBLIC ${EXT_LIB_DIR}/nanogui/include/nanogui)
#    target_include_directories(${TARGET_NAME} PUBLIC ${EXT_LIB_DIR}/nanogui/src)

    target_include_directories(${TARGET_NAME} PUBLIC ${EXT_LIB_ROOT}/nanogui-build/include)
    target_include_directories(${TARGET_NAME} PUBLIC ${EXT_LIB_ROOT}/nanogui-build/include/nanogui)
#    target_include_directories(${TARGET_NAME} PUBLIC ${EXT_LIB_ROOT}/nanogui-build/include/nanogui/src)

    target_link_libraries(${TARGET_NAME} PUBLIC
        debug ${EXT_LIB_ROOT}/nanogui-build/${ARCH}-Debug-static/nanogui.lib
        optimized ${EXT_LIB_ROOT}/nanogui-build/${ARCH}-Release-static/nanogui.lib)
    target_link_libraries(${TARGET_NAME} PUBLIC
        debug ${EXT_LIB_ROOT}/glfw-3.3.5-build/${ARCH}-Debug-static/glfw3.lib
        optimized ${EXT_LIB_ROOT}/glfw-3.3.5-build/${ARCH}-Release-static/glfw3.lib)
    target_compile_definitions(${TARGET_NAME} PUBLIC NANOGUI_BUILD)
endmacro()

macro(link_imgui TARGET_NAME)
    # todo: maybe make a static lib
    set(IMGUI_HOME ${GIT_ROOT}/external/lib_bin/imgui)
    target_include_directories(${TARGET_NAME}
        PRIVATE ${IMGUI_HOME} ${IMGUI_HOME}/backends ${IMGUI_HOME}/misc/cpp)
    file(GLOB IMGUI_CPP "${IMGUI_HOME}/imgui*.cpp")
    target_sources(${TARGET_NAME} PRIVATE ${IMGUI_CPP})
    target_sources(${TARGET_NAME} PRIVATE
        "${IMGUI_HOME}/backends/imgui_impl_glfw.cpp"
        "${IMGUI_HOME}/misc/cpp/imgui_stdlib.cpp"
        "${IMGUI_HOME}/backends/imgui_impl_opengl3.cpp" )
endmacro()

macro(link_src TARGET_NAME)
    target_include_directories(${TARGET_NAME} PRIVATE ${GIT_ROOT}/src)
    target_sources(${TARGET_NAME} PRIVATE ${SRC_SOURCES})
endmacro()

macro(link_glm TARGET_NAME)
    target_include_directories(${TARGET_NAME} PRIVATE ${EXT_LIB_ROOT}/glm-build/include)
endmacro()

macro(link_cute TARGET_NAME)
    target_include_directories(${TARGET_NAME} PRIVATE ${EXT_LIB_ROOT}/cute-build/include)
endmacro()

# setup output directory location based on the project name for a target
# usage:
#list(APPEND TARGETS nanogui_example glad_example nanovg_example imgui_example)
#setup_output_destination("${TARGETS}")
macro(setup_output_destination TARGET)
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        set_target_properties(${TARGET}
            PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${BIN_ROOT}/${PROJECT_NAME}/Release"
            LIBRARY_OUTPUT_DIRECTORY_RELEASE "${BIN_ROOT}/${PROJECT_NAME}/Release"
            RUNTIME_OUTPUT_DIRECTORY_RELEASE "${BIN_ROOT}/${PROJECT_NAME}/Release"
            )
    else()
        set_target_properties(${TARGET}
            PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${BIN_ROOT}/${PROJECT_NAME}/Debug"
            LIBRARY_OUTPUT_DIRECTORY_DEBUG "${BIN_ROOT}/${PROJECT_NAME}/Debug"
            RUNTIME_OUTPUT_DIRECTORY_DEBUG "${BIN_ROOT}/${PROJECT_NAME}/Debug"
            )
    endif()
endmacro()
