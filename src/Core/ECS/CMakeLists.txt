set(dir "ECS")
set(target "Core_${dir}")
message(STATUS "${target} module")
add_library(${target} STATIC)
add_library("Core::${dir}" ALIAS ${target})

# SOURCES
target_sources(${target} PRIVATE
   ${target}/Dummy.cpp
   ${target}/Test.cpp
)

# INCLUDE DIRECTORY
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# LINK TO LIBRARIES
target_link_libraries(${target} PUBLIC
   Core
   Core::Utils
   Core::AssetManager
)
