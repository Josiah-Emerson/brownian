set(dir "Window")
set(target "Core_${dir}")
message(STATUS "${target} module")
add_library(${target}) # TODO: remove interface when cpp files are added
add_library("Core::${dir}" ALIAS ${target})

# SOURCES 
target_sources(${target} PRIVATE 
   ${target}/LinuxWindow.cpp
   ${target}/Window.cpp
   ${imgui_files}
)

# INCLUDE DIRECTORY
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# LINK TO OTHER MODULES
# target_link_libraries(${target} PUBLIC)

if(FIG_TARGET_OS_LINUX) # TODO: Extend for Win32 and OSX
   find_package(X11 REQUIRED)
   target_link_libraries(${target} PRIVATE X11::X11)
endif()

# TODO: Will need to move this to somewhere higher that also used opengl
find_package(OpenGL REQUIRED)
target_link_libraries(${target} PRIVATE 
   OpenGL::GL
   Core::Utils
) # OpenGL::GLU may be needed in future

