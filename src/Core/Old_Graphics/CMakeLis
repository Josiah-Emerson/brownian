set(dir "Graphics")
set(target "Core_${dir}")
message(STATUS "${target} module")
add_library(${target})
add_library("Core::${dir}" ALIAS ${target})

# SOURCES
target_sources(${target} PRIVATE
   ${target}/Camera.cpp
   ${target}/Model.cpp
   ${target}/RenderDevice.cpp
   ${target}/ShaderProgram.cpp
   ${target}/gl/GLRenderDevice.cpp
   ${target}/gl/GLShader.cpp
   ${target}/gl/GLShaderProgram.cpp
   ${target}/gl/OpenGL.cpp
)

# INCLUDE DIRECTORY
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# LINK TO LIBRARIES
target_link_libraries(${target} PRIVATE
   Core
   Core::Utils
)
