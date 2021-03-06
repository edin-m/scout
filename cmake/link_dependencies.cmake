# link multiple dependencies at once
# usage: link_dependencies(${TARGET} LIBS eigen)
macro(link_dependencies TARGET_NAME)
    set(oneValueArgs TARGET_NAME)
    set(multiValueArgs LIBS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}"
        "${multiValueArgs}" ${ARGN} )
    message("Linking " ${ARG_LIBS} " for " ${TARGET_NAME})
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

        # src
        if (${lib} STREQUAL src)
            link_src(${TARGET_NAME})
        endif()

        # nanovg
        if(${lib} STREQUAL nanovg)
            link_nanovg(${TARGET_NAME})
        endif()

        # imgui
        if(${lib} STREQUAL imgui)
            link_imgui(${TARGET_NAME})
        endif()

        # imgui2
        if(${lib} STREQUAL imgui2)
            link_imgui2(${TARGET_NAME})
        endif()

        # eigen
        if(${lib} STREQUAL eigen)
            link_eigen(${TARGET_NAME})
        endif()

        # nanogui
        if(${lib} STREQUAL nanogui)
            link_nanogui(${TARGET_NAME})
        endif()

        # glm
        if(${lib} STREQUAL glm)
            link_glm(${TARGET_NAME})
        endif()

        # cute
        if(${lib} STREQUAL cute)
            link_cute(${TARGET_NAME})
        endif()

        # uv
        if(${lib} STREQUAL libuv)
            link_libuv(${TARGET_NAME})
        endif()

        # protobuf
        if (${lib} STREQUAL protobuf)
            link_protobuf(${TARGET_NAME})
        endif()
    endforeach()
endmacro()
