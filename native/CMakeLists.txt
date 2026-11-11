cmake_minimum_required(VERSION 3.25) # add_subdirectory(... SYSTEM)
project(citycore LANGUAGES C CXX)

# compile_commands.json in every build folder: clangd takes each file's real flags from it (native/.clangd points at build-win).
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Godot 4.7 API, single precision (see architecture doc: 30 km fits in float32 with the origin at the map center).
set(GODOTCPP_API_VERSION "4.7" CACHE STRING "" FORCE)
set(GODOTCPP_TARGET "template_release" CACHE STRING "template_release for the export, template_debug for the editor")
set(GODOTCPP_PRECISION "single" CACHE STRING "" FORCE)
set(GODOTCPP_DISABLE_EXCEPTIONS ON CACHE BOOL "" FORCE)

# Code for what the renderer already asks of the machine: AVX2, FMA and BMI (x86-64-v3, Haswell in 2013 and Excavator on; Vulkan on
# Windows starts there). floor and round are one instruction and not a call into the C library, and the vectorizer has eight lanes.
# Contraction stays off: a product and a sum rounded apart give the same bits with or without FMA, so a city is the same number on
# any machine and the goldens hold. No errno: sqrt is one instruction too. Everything compiled here takes these: godot-cpp and
# meshoptimizer are called from the same loops.
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-march=x86-64-v3 -ffp-contract=off -fno-math-errno -fno-trapping-math -fvisibility-inlines-hidden)
endif()

# No folder of the machine that builds goes into what it builds: a source path the compiler writes (__FILE__ in the asserts of godot-cpp
# and libc++, the debug info) is written from the repository's root, the toolchain's from its own name (-ffile-prefix-map: clang's macro
# and debug maps together, ClangCommandLineReference), and the profile's name of a static function drops the root's directories
# (-static-func-strip-dirname-prefix, the compiler's own option for the counter names): a DLL, a profile and an export hold no person's
# folder, and two machines build the same names.
get_filename_component(CITY_REPO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    string(REGEX MATCHALL "/" CITY_ROOT_SLASHES "${CITY_REPO_ROOT}/")
    list(LENGTH CITY_ROOT_SLASHES CITY_ROOT_LEVELS)
    add_compile_options("-ffile-prefix-map=${CITY_REPO_ROOT}/=" "SHELL:-mllvm -static-func-strip-dirname-prefix=${CITY_ROOT_LEVELS}")
    if(LLVM_MINGW)
        add_compile_options("-ffile-prefix-map=${LLVM_MINGW}/=llvm-mingw/")
    endif()
endif()

# The export's DLL: one program for the optimizer across every file (ThinLTO, https://clang.llvm.org/docs/ThinLTO.html), and with the
# profile of a run of the game (native/pgo, tools/build/pgo.sh) it knows which branch is taken and what is worth inlining. The tests'
# DLL leaves both out: it is linked many times an hour. CITY_PROFILE=generate is the instrumented build pgo.sh trains.
set(CITY_PROFILE "use" CACHE STRING "use: the release DLL reads native/pgo/citycore.profdata; generate: instrumented, for tools/build/pgo.sh")
set(CITY_PROFILE_DATA "${CMAKE_CURRENT_SOURCE_DIR}/pgo/citycore.profdata")
if(GODOTCPP_TARGET STREQUAL "template_release" AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-flto=thin)
    add_link_options(-flto=thin -Wl,--lto-O3 "-Wl,--thinlto-cache-dir=${CMAKE_BINARY_DIR}/thinlto")
    if(CITY_PROFILE STREQUAL "generate")
        add_compile_options(-fprofile-generate)
        add_link_options(-fprofile-generate)
    elseif(EXISTS "${CITY_PROFILE_DATA}")
        add_compile_options("-fprofile-use=${CITY_PROFILE_DATA}" -Wno-profile-instr-out-of-date -Wno-profile-instr-unprofiled -Wno-backend-plugin)
        add_link_options("-fprofile-use=${CITY_PROFILE_DATA}")
    endif()
endif()

add_subdirectory(godot-cpp SYSTEM) # its headers are third-party: their warnings are not ours

# The simplifier of the vehicles' levels (render/agents/agent_lod.h), fetched into the build folder at a fixed version.
include(FetchContent)
FetchContent_Declare(meshoptimizer GIT_REPOSITORY https://github.com/zeux/meshoptimizer.git GIT_TAG v1.3 GIT_SHALLOW TRUE SYSTEM)
FetchContent_MakeAvailable(meshoptimizer)
set_target_properties(meshoptimizer PROPERTIES POSITION_INDEPENDENT_CODE ON)

# new and delete of the whole core (core/memory.cpp): a thread-local heap per worker, so the jobs that assemble meshes and the
# simulation never meet on the C runtime's one lock, and a block is freed by whoever holds it last.
set(MI_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(MI_BUILD_OBJECT OFF CACHE BOOL "" FORCE)
set(MI_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(MI_OVERRIDE OFF CACHE BOOL "" FORCE)
set(MI_WIN_REDIRECT OFF CACHE BOOL "" FORCE)
# How the allocator learns that the library was loaded and that a thread ended. As a static library it registers both through
# MSVC pragmas (src/prim/windows/prim.c: const_seg ".CRT$XLB", linker /INCLUDE), which clang for MinGW ignores: its callbacks
# landed in .rdata and never ran. It then stayed in its loading state for good, where freed memory is only marked reusable
# (MEM_RESET, src/os.c _mi_os_purge_ex) and never given back, and the heap of a thread that ended was never freed: a thread that
# freed 240 of 256 MiB still held all of them, and every save and restart of the simulation left its thread's heap behind.
# MI_WIN_USE_FLS is the allocator's own path outside MSVC: a constructor runs its start (src/prim/prim.c), a fiber-local callback
# each thread's end. tests/core/memory_test holds it.
if(WIN32)
    set(MI_WIN_USE_FLS ON CACHE BOOL "" FORCE)
endif()
FetchContent_Declare(mimalloc GIT_REPOSITORY https://github.com/microsoft/mimalloc.git GIT_TAG v2.2.4 GIT_SHALLOW TRUE SYSTEM)
FetchContent_MakeAvailable(mimalloc)
set_target_properties(mimalloc-static PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Polygon booleans in integers (the lots cut against the paving), the same on every machine.
set(CLIPPER2_UTILS OFF CACHE BOOL "" FORCE)
set(CLIPPER2_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CLIPPER2_TESTS OFF CACHE BOOL "" FORCE)
set(CLIPPER2_USINGZ "OFF" CACHE STRING "" FORCE)
FetchContent_Declare(clipper2 GIT_REPOSITORY https://github.com/AngusJohnson/Clipper2.git GIT_TAG Clipper2_2.0.1 GIT_SHALLOW TRUE SOURCE_SUBDIR CPP)
FetchContent_MakeAvailable(clipper2)
set_target_properties(Clipper2 PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_options(Clipper2 PRIVATE -include iterator) # its header leans on what libstdc++ brings along and libc++ does not

file(GLOB_RECURSE CITYCORE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE CITYCORE_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

add_library(citycore SHARED ${CITYCORE_SOURCES} ${CITYCORE_HEADERS})
target_link_libraries(citycore PRIVATE godot-cpp meshoptimizer mimalloc-static Clipper2)
target_include_directories(citycore PRIVATE ${clipper2_SOURCE_DIR}/CPP/Clipper2Lib/include/clipper2) # <clipper.h>, as <meshoptimizer.h>: a library by its own name

# Every object of the link is compiled against the one profile: ThinLTO refuses two summaries, and Ninja knows nothing of a file read through
# a flag, so a new profile would leave the libraries on the old one.
if(GODOTCPP_TARGET STREQUAL "template_release" AND CITY_PROFILE STREQUAL "use" AND EXISTS "${CITY_PROFILE_DATA}")
    foreach(lib citycore godot-cpp meshoptimizer mimalloc-static Clipper2)
        get_target_property(lib_dir ${lib} SOURCE_DIR)
        get_target_property(lib_sources ${lib} SOURCES)
        set(lib_files "")
        foreach(file ${lib_sources})
            if(NOT file MATCHES "\\$<")
                cmake_path(ABSOLUTE_PATH file BASE_DIRECTORY "${lib_dir}")
                list(APPEND lib_files "${file}")
            endif()
        endforeach()
        set_source_files_properties(${lib_files} TARGET_DIRECTORY ${lib} PROPERTIES OBJECT_DEPENDS "${CITY_PROFILE_DATA}")
    endforeach()
endif()
# godot-cpp adds --gc-sections after our options; passed last as a library item it wins, keeping the static
# engine-class registration callbacks (Image, Camera3D, Terrain3D...) that the release DLL needs at runtime.
if(MINGW)
    target_link_libraries(citycore PRIVATE "-Wl,--no-gc-sections")
endif()
target_include_directories(citycore PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
# Every warning on (the build must stay free of them: CLAUDE.md "Antes de concluir"); godot-cpp keeps its own flags.
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(citycore PRIVATE -Wall -Wextra -fno-exceptions)
elseif(MSVC)
    target_compile_options(citycore PRIVATE /W4)
endif()

# The tests' DLL: an index out of a container stops there, the first time, with its place (libc++ Hardening.rst, fast mode): the
# bug shows in any ordinary run, not when the heap happens to make it visible.
# CITY_CHECKS: the core's own rules checked there too (src/core/checks.h), where the export's DLL carries no branch for them.
if(GODOTCPP_TARGET STREQUAL "template_debug")
    target_compile_definitions(citycore PRIVATE _LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_FAST CITY_CHECKS=1)
endif()

get_target_property(GODOTCPP_SUFFIX godot-cpp GODOTCPP_SUFFIX)
set(CITY_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../project/bin/" CACHE PATH "where the DLL goes (the instrumented one stays out of the project)")
set(OUTPUT_DIR "${CITY_OUTPUT_DIR}")

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
