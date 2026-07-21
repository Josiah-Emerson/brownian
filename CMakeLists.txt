message(STATUS "Root level CMake")
set(project "Fig")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# set(CMAKE_C_COMPILER "/usr/bin/gcc-11")
# set(CMAKE_CXX_COMPILER "/usr/bin/g++-11")
project(${project} LANGUAGES CXX)
cmake_minimum_required(VERSION 3.22)
cmake_policy(SET CMP0079 NEW) # `target_link_libraries()` allows use with targets in other directories.

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
message("Runtime dir: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")


# TODO: if need to add compile options for future (for example allow user to decide to use X11 or wayland) see erhe cmakelists root file for set_options macro

# TODO: For now we only support openGL but add support for other version. Also, for now this is compile time, but should we allow it to be a runtime feature?
set(FIG_TARGET_GRAPHICS_OPENGL TRUE)
add_definitions(-DOPENGL)
message("OpenGL set as target graphics platform")

# NOTE: If we add another platform ensure that ${CMAKE_SYSTEM_NAME} is wrapped in "" or else the MATCHES or STREQUAL will not run as expected
if(WIN32) 
   message("Detected Windows platform")
   set(FIG_TARGET_OS_WINDOWS TRUE)
   add_definitions(-DWIN32)
elseif(UNIX AND ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux"))
   message("Detected Linux platform")
   set(FIG_TARGET_OS_LINUX TRUE)
   add_definitions(-DLINUX)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin") # TODO: Why strequal not matches ? And why Darwin
   message("Detected Apple OSX")
   set(FIG_TARGET_OS_OSX TRUE)
   add_definitions(-DOSX)
else()
   message(FATAL_ERROR "OS not supported ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSYTEM_PROCESSOR}")
endif()

if("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
   message("Debug build detected")
   # TODO: Actual debug stuff
endif()

# ImGui
# TODO: Should this be defined here? It needs to be targeted in the next cmakelists file anyways?
include_directories(${CMAKE_SOURCE_DIR}/imgui)
file(GLOB_RECURSE imgui_files "imgui/*.cpp")

add_subdirectory(external)
add_subdirectory(src)

if(TESTS)
   message("Test build requested")
   add_subdirectory(${CMAKE_SOURCE_DIR}/Catch2)
   add_subdirectory(test)
endif()

# cmake -DCMAKE_BUILD_TYPE=Debug
