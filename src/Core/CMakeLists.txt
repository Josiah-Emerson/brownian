message(STATUS "Core")
set(target "Core")
add_Library(${target})

target_sources(${target} PRIVATE
   Application.cpp
   Layer.cpp
)

add_subdirectory(ECS)
add_subdirectory(Graphics)
add_subdirectory(Physics)
add_subdirectory(UI)
add_subdirectory(Utils)
add_subdirectory(Window)

target_link_libraries(${target} PUBLIC 
   Core::ECS
   Core::Graphics
   Core::Utils
   Core::Window
)

target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
