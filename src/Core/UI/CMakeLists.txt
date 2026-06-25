set(dir "UI")
set(target "Core_${dir}")
message(STATUS "${target} module")
add_library(${target})
add_library("Core::${dir}" ALIAS ${target})

# SOURCES
target_sources(${target} PRIVATE
   ${imgui_files}
   ${target}/Gui.cpp
)

# INCLUDE DIRECTORY
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# LINK TO LIBRARIES
target_link_libraries(${target} PRIVATE
   Core
   Core::Utils
   Core::Window
)
