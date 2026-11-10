cmake_minimum_required(VERSION 3.25) # add_subdirectory(... SYSTEM)
project(citycore LANGUAGES C CXX)

# compile_commands.json in every build folder: clangd takes each file's real flags from it (native/.clangd points at build-win).
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Godot 4.7 API, single precision (see architecture doc: 30 km fits in float32 with the origin at the map center).
set(GODOTCPP_API_VERSION "4.7" CACHE STRING "" FORCE)
set(GODOTCPP_TARGET "template_release" CACHE STRING "template_release for the export, template_debug for the editor")
set(GODOTCPP_PRECISION "single" CACHE STRING "" FORCE)
set(GODOTCPP_DISABLE_EXCEPTIONS ON CACHE BOOL "" FORCE)

# The PC build codes for what the renderer already asks of the machine: AVX2, FMA and BMI (x86-64-v3, Haswell in 2013 and Excavator
# on; Vulkan on Windows starts there). floor and round are one instruction and not a call into the C library, and the vectorizer has
# eight lanes. The Android build (arm64) keeps the ISA's baseline, armv8-a, where floor and round are one instruction as well.
# Contraction stays off: a product and a sum rounded apart give the same bits with or without FMA, so a city is the same number on
# any machine and the goldens hold (arm64 compilers fuse by default). No errno: sqrt is one instruction too. Everything compiled here
# takes these: godot-cpp and meshoptimizer are called from the same loops.
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64)$")
        add_compile_options(-march=x86-64-v3)
    endif()
    add_compile_options(-ffp-contract=off -fno-math-errno -fno-trapping-math -fvisibility-inlines-hidden)
endif()

# What the build leaves behind names files from the repository root, never by this checkout's folder: __FILE__ in godot-cpp's
# error macros reads `native/...` (a generated file `build/...`: Clang takes the longest matching prefix, GCC the last one), so a
# clone in another folder, a worktree or another machine builds the same bytes, and the user's folders are not in the DLL. The
# profile's names follow the same rule (city_profile_names, below).
cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH CITY_REPO_ROOT)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options("-ffile-prefix-map=${CITY_REPO_ROOT}/=" "-ffile-prefix-map=${CMAKE_BINARY_DIR}/=build/")
endif()

# The profile keys a static function by its file's path as compiled (LLVM InstrProf.cpp: getPGOFuncName), absolute here; the
# separators of `base` are stripped from it (static-func-strip-dirname-prefix), so a static function of the core is
# `native/src/x.cpp;f` and one of a fetched library `mimalloc-src/src/static.c;f`, whatever folder or build folder compiled it,
# and native/pgo matches a build anywhere. One value per target: LLVM takes the option once.
function(city_profile_names target base)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        string(REGEX MATCHALL "/" separators "${base}/")
        list(LENGTH separators depth)
        target_compile_options(${target} PRIVATE "SHELL:-mllvm -static-func-strip-dirname-prefix=${depth}")
    endif()
endfunction()

# The export's DLL: one program for the optimizer across every file (ThinLTO, https://clang.llvm.org/docs/ThinLTO.html), and with the
# profile of a run of the game (native/pgo, tools/build/pgo.sh) it knows which branch is taken and what is worth inlining. The tests'
# DLL leaves both out: it is linked many times an hour. CITY_PROFILE=generate is the instrumented build pgo.sh trains; none is a build
# the profile was not trained on (the Android arm64 library: pgo.sh runs the PC game, and the NDK's clang is an older one).
set(CITY_PROFILE "use" CACHE STRING "use: the release DLL reads native/pgo/citycore.profdata and .order; generate: instrumented, for tools/build/pgo.sh; order: the profile's function order written to native/pgo/citycore.order, for tools/build/pgo.sh; none: no profile")
set(CITY_PROFILE_DATA "${CMAKE_CURRENT_SOURCE_DIR}/pgo/citycore.profdata")
set(CITY_PROFILE_ORDER "${CMAKE_CURRENT_SOURCE_DIR}/pgo/citycore.order")
if(GODOTCPP_TARGET STREQUAL "template_release" AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-flto=thin)
    add_link_options(-flto=thin -Wl,--lto-O3 "-Wl,--thinlto-cache-dir=${CMAKE_BINARY_DIR}/thinlto")
    if(CITY_PROFILE STREQUAL "generate")
        add_compile_options(-fprofile-generate)
        add_link_options(-fprofile-generate)
    elseif(CITY_PROFILE MATCHES "^(use|order)$" AND EXISTS "${CITY_PROFILE_DATA}")
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
FetchContent_Declare(mimalloc GIT_REPOSITORY https://github.com/microsoft/mimalloc.git GIT_TAG v2.2.4 GIT_SHALLOW TRUE SYSTEM)
FetchContent_MakeAvailable(mimalloc)
# Its banner prints the hour it was compiled (src/options.c: __DATE__, __TIME__), which put the build's clock in the tests' DLL: its
# sources compile with the clock at the epoch (SOURCE_DATE_EPOCH, read by Clang, https://reviews.llvm.org/D135045, and by GCC), so
# the DLL does not change with the hour it was built.
set_target_properties(mimalloc-static PROPERTIES POSITION_INDEPENDENT_CODE ON C_COMPILER_LAUNCHER "${CMAKE_COMMAND};-E;env;SOURCE_DATE_EPOCH=0")
# A segment (32 MiB, include/mimalloc/types.h) is committed as its pages are used. Its default (src/options.c, eager_commit with
# eager_commit_delay 1) commits whole every segment of a thread but its first: 32 MiB of private bytes a thread whose first segment
# was ever freed, on every worker of the pool, whatever the city holds (log/performance.md P-01: 22 segments, 704 MiB).
target_compile_definitions(mimalloc-static PRIVATE MI_DEFAULT_EAGER_COMMIT=0)

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
city_profile_names(citycore "${CITY_REPO_ROOT}")
city_profile_names(godot-cpp "${CITY_REPO_ROOT}") # its generated bindings keep the build folder: static functions that never ran
cmake_path(GET meshoptimizer_SOURCE_DIR PARENT_PATH fetched) # the fetched libraries from their <name>-src folder on
city_profile_names(meshoptimizer "${fetched}")
cmake_path(GET mimalloc_SOURCE_DIR PARENT_PATH fetched)
city_profile_names(mimalloc-static "${fetched}")
cmake_path(GET clipper2_SOURCE_DIR PARENT_PATH fetched)
city_profile_names(Clipper2 "${fetched}")

# Every object of the link is compiled against the one profile: ThinLTO refuses two summaries, and Ninja knows nothing of a file read through
# a flag, so a new profile would leave the libraries on the old one.
if(GODOTCPP_TARGET STREQUAL "template_release" AND CITY_PROFILE MATCHES "^(use|order)$" AND EXISTS "${CITY_PROFILE_DATA}")
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
    # No link time in the PE header (lld's MinGW driver, --no-insert-timestamp): the same sources give the same DLL bytes, so a
    # rebuild leaves the committed DLLs unchanged and two branches built from the same sources do not conflict.
    target_link_options(citycore PRIVATE "-Wl,--no-insert-timestamp")
endif()
# The order of the export DLL's functions, as the profile's call graph clusters them (lld's /call-graph-profile-sort), read from
# native/pgo/citycore.order and not sorted at the link: lld hands its sort's result on through a map keyed by the sections'
# addresses in memory and stores it by symbol name (lld/COFF/Writer.cpp, sortSections), so the sections that share a name (a
# __cxx_global_var_init per file, 227 of them) took a priority that changed with every link, and every function after them moved:
# two links of the same objects differed in 700 thousand bytes. Read from a file, the order is the same at every link.
# tools/build/pgo.sh writes it with the profile (CITY_PROFILE=order: the sort on, /print-symbol-order into the build folder) and keeps
# only the names every folder gives alike: a local function ThinLTO promotes is named after its module's hash (`.llvm.<n>`), and the
# module holds its source's absolute path. A name the code no longer has is an order out of date, as the profile's counts are
# (-Wno-profile-instr-out-of-date): LNK4037 is left out alike.
if(MINGW AND GODOTCPP_TARGET STREQUAL "template_release" AND EXISTS "${CITY_PROFILE_DATA}")
    if(CITY_PROFILE STREQUAL "order")
        target_link_options(citycore PRIVATE "-Wl,--Xlink=-print-symbol-order:${CMAKE_BINARY_DIR}/citycore.order")
    elseif(CITY_PROFILE STREQUAL "use")
        if(NOT EXISTS "${CITY_PROFILE_ORDER}")
            message(FATAL_ERROR "${CITY_PROFILE_ORDER} is missing: tools/build/pgo.sh writes it with the profile")
        endif()
        target_link_options(citycore PRIVATE "-Wl,--Xlink=-call-graph-profile-sort:no" "-Wl,--Xlink=-order:@${CITY_PROFILE_ORDER}"
            "-Wl,--Xlink=-ignore:4037")
        set_property(TARGET citycore APPEND PROPERTY LINK_DEPENDS "${CITY_PROFILE_ORDER}")
    endif()
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
if(GODOTCPP_TARGET STREQUAL "template_debug")
    target_compile_definitions(citycore PRIVATE _LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_FAST)
endif()

get_target_property(GODOTCPP_SUFFIX godot-cpp GODOTCPP_SUFFIX)
set(CITY_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../project/bin/" CACHE PATH "where the DLL goes (the instrumented one stays out of the project)")
set(OUTPUT_DIR "${CITY_OUTPUT_DIR}")

set_target_properties(citycore PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
    CXX_VISIBILITY_PRESET hidden
    POSITION_INDEPENDENT_CODE ON
    LIBRARY_OUTPUT_DIRECTORY "$<1:${OUTPUT_DIR}>"
    RUNTIME_OUTPUT_DIRECTORY "$<1:${OUTPUT_DIR}>"
    PREFIX "lib"
    OUTPUT_NAME "citycore${GODOTCPP_SUFFIX}"
)
# The library carries its own runtime, nothing to ship beside it. On Android the NDK already links the C++ runtime statically
# (ANDROID_STL=c++_static), and libc, libm and libdl are the system's: a shared object finds them only as shared libraries.
if(NOT ANDROID)
    set_target_properties(citycore PROPERTIES LINK_SEARCH_START_STATIC ON LINK_SEARCH_END_STATIC ON)
endif()
