# sorted_link.cmake: the release DLL's link, laid out by the profile's call graph and the same at every link. native/CMakeLists.txt
# runs it as the target's linker launcher: cmake -DORDER=<file> -P sorted_link.cmake -- <the link command>.
#
# lld COFF sorts by the call graph the profile wrote into the objects (.llvm.call-graph-profile, lld/COFF/CallGraphSort.cpp), and its
# result is a map keyed by the sections' addresses, copied into a map keyed by symbol name (lld/COFF/Writer.cpp, sortSections): the
# sections that share a name (a __cxx_global_var_init per file) take whichever priority the walk of the addresses gives last, so
# the layout changed with every link. The order it prints (/print-symbol-order) walks the sorted clusters instead, the same at
# every link. So the command runs twice: the first link sorts and prints the order to ORDER, the second lays the DLL out by it
# (/order, which turns the sort off). The second link's ThinLTO backends come from the first one's cache.
cmake_minimum_required(VERSION 3.25)

set(command "")
set(after_separator OFF)
math(EXPR last_arg "${CMAKE_ARGC} - 1")
foreach(i RANGE ${last_arg})
    if(after_separator)
        list(APPEND command "${CMAKE_ARGV${i}}")
    elseif("${CMAKE_ARGV${i}}" STREQUAL "--")
        set(after_separator ON)
    endif()
endforeach()
if(NOT ORDER OR NOT command)
    message(FATAL_ERROR "usage: cmake -DORDER=<file> -P sorted_link.cmake -- <link command>")
endif()

file(REMOVE "${ORDER}")
execute_process(COMMAND ${command} "-Wl,--Xlink=-print-symbol-order:${ORDER}" COMMAND_ERROR_IS_FATAL ANY)
# lld prints the order only when the objects carry a call graph: none means the profile reached no function.
set(order_bytes 0)
if(EXISTS "${ORDER}")
    file(SIZE "${ORDER}" order_bytes)
endif()
if(order_bytes EQUAL 0)
    message(FATAL_ERROR "the link printed no order (${ORDER}): the objects carry no call graph profile")
endif()
execute_process(COMMAND ${command} "-Wl,--Xlink=-call-graph-profile-sort:no" "-Wl,--Xlink=-order:@${ORDER}" COMMAND_ERROR_IS_FATAL ANY)
