cmake_minimum_required(VERSION 3.17)
project(citycore LANGUAGES C CXX)

# Godot 4.7 API, single precision (see architecture doc: 30 km fits in float32 with the origin at the map center).
set(GODOTCPP_API_VERSION "4.7" CACHE STRING "" FORCE)
set(GODOTCPP_TARGET "template_release" CACHE STRING "template_release for the export, template_debug for the editor")
set(GODOTCPP_PRECISION "single" CACHE STRING "" FORCE)
set(GODOTCPP_DISABLE_EXCEPTIONS ON CACHE BOOL "" FORCE)

add_subdirectory(godot-cpp)

file(GLOB_RECURSE CITYCORE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE CITYCORE_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

add_library(citycore SHARED ${CITYCORE_SOURCES} ${CITYCORE_HEADERS})
target_link_libraries(citycore PRIVATE godot-cpp)
# godot-cpp adds --gc-sections after our options; passed last as a library item it wins, keeping the static
# engine-class registration callbacks (Image, Camera3D, Terrain3D...) that the release DLL needs at runtime.
if(MINGW)
    target_link_libraries(citycore PRIVATE "-Wl,--no-gc-sections")
endif()
target_include_directories(citycore PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

get_target_property(GODOTCPP_SUFFIX godot-cpp GODOTCPP_SUFFIX)
set(OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../project/bin/")

set_target_properties(citycore PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
    CXX_VISIBILITY_PRESET hidden
    POSITION_INDEPENDENT_CODE ON
    LINK_SEARCH_START_STATIC ON
    LINK_SEARCH_END_STATIC ON
    LIBRARY_OUTPUT_DIRECTORY "$<1:${OUTPUT_DIR}>"
    RUNTIME_OUTPUT_DIRECTORY "$<1:${OUTPUT_DIR}>"
    PREFIX "lib"
    OUTPUT_NAME "citycore${GODOTCPP_SUFFIX}"
)
