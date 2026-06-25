message(STATUS "Core")
set(target "Core")
add_Library(${target})

target_sources(${target} PRIVATE
   Application.cpp
   Layer.cpp
)

add_subdirectory(ECS)
add_subdirectory(Graphics)
# add_subdirectory(Physics)
add_subdirectory(UI)
add_subdirectory(Window)
add_subdirectory(Utils)

target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${target} PUBLIC 
   Core::Window
   Core::Graphics
   Core::Utils
)

