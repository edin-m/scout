file(GLOB_RECURSE SRC_FILES "*.h" "*.cpp" "**/*.h" "**/*.cpp" "**/**/*.h" "**/**/*.cpp")
set(SRC_SOURCES ${SRC_FILES} PARENT_SCOPE)

message("sdfadsd das dasf adsf das " "${SRC_FILES}")
add_library(libsrc STATIC ${SRC_FILES})
link_dependencies(libsrc LIBS glm)
