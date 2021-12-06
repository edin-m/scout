
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


# todo: use "${TARGET_NAME}"
# todo: use "${GIT_ROOT}/..."
# todo: use target_include_directories

macro(link_opengl TARGET_NAME)
    target_link_libraries(${TARGET_NAME} opengl32.lib)
endmacro()

macro(link_glfw TARGET_NAME)
    target_include_directories(${TARGET_NAME} PUBLIC ${GIT_ROOT}/external/lib_bin/glfw-3.3.5-build/include)
    target_link_directories(${TARGET_NAME} PUBLIC ${GIT_ROOT}/external/lib_bin/glfw-3.3.5-build/lib/x86/Release)
    target_link_libraries(${TARGET_NAME} glfw3.lib)
endmacro()

macro(link_glad TARGET_NAME)
    include_directories(${GIT_ROOT}/external/lib_bin/glad/include)
    target_sources(${TARGET_NAME} PRIVATE ${GIT_ROOT}/external/lib_bin/glad/src/glad.c)
endmacro()

macro(link_nanovg TARGET_NAME)
    include_directories(${GIT_ROOT}/external/lib_bin/nanovg-build/include)
    target_link_libraries(${TARGET_NAME} ${GIT_ROOT}/external/lib_bin/nanovg-build/lib/x86/Release/nanovg.lib)
endmacro()

macro(link_imgui TARGET_NAME)
    set(IMGUI_HOME ${GIT_ROOT}/external/lib_bin/imgui)
    include_directories(${IMGUI_HOME} ${IMGUI_HOME}/backends)
    file(GLOB IMGUI_CPP "${IMGUI_HOME}/imgui*.cpp")
    target_sources(${TARGET_NAME} PRIVATE ${IMGUI_CPP})
    target_sources(${TARGET_NAME} PRIVATE
        "${IMGUI_HOME}/backends/imgui_impl_glfw.cpp"
        "${IMGUI_HOME}/backends/imgui_impl_opengl3.cpp" )
endmacro()

macro(add_src_files TARGET_NAME)
    target_sources(${TARGET_NAME} PRIVATE ${SRC_SOURCES})
endmacro()

macro(link_dependencies TARGET_NAME)
    set(oneValueArgs TARGET_NAME)
    set(multiValueArgs LIBS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}"
        "${multiValueArgs}" ${ARGN} )

    foreach(lib ${ARG_LIBS})
        # opengl
        if (${lib} STREQUAL opengl)
            link_opengl(${TARGET_NAME})
        endif()

        # glfw
        if(${lib} STREQUAL glfw)
            link_glfw(${TARGET_NAME})
        endif()

        # glad
        if(${lib} STREQUAL glad)
            link_glad(${TARGET_NAME})
        endif()

        # nanovg
        if(${lib} STREQUAL nanovg)
            link_nanovg(${TARGET_NAME})
        endif()

        # imgui
        if(${lib} STREQUAL imgui)
            link_imgui(${TARGET_NAME})
        endif()

        # nanogui
#        elseif(${lib} STREQUAL imgui)
#            link_imgui(${TARGET_NAME})
    endforeach()
endmacro()
