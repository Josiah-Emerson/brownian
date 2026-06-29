set(dir "Graphics")
set(target "Core_${dir}")
message(STATUS "${target} module")
add_library(${target})
add_library("Core::${dir}" ALIAS ${target})

# TODO: Separate out gl files based on compile flags (unless we want it to be able to be changed at runtime)
# SOURCES
target_sources(${target} PRIVATE
   ${target}/AssetManager.cpp
   ${target}/Camera.cpp
   ${target}/Mesh.cpp
   ${target}/RenderDevice.cpp
   ${target}/Renderer.cpp
   ${target}/gl/GLCommandList.cpp
   ${target}/gl/GLRenderDevice.cpp
   ${target}/gl/OpenGL.cpp
)

# INCLUDE DIRECTORY
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# LINK TO LIBRARIES
target_link_libraries(${target} PRIVATE
   Core::ECS
   Core::Utils
   Core::Window
)
