# CMakeList.txt : CMake project for Core Library

set(CORE_LIBRARY Engine)

# Add source to this project's executable.
add_library (${CORE_LIBRARY})

target_sources(${CORE_LIBRARY}
    PRIVATE
        "src/Core/Application.cpp"
        "src/Rendering/Window.cpp"
        "src/Events/EventSystem.cpp"

    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
        include
    FILES
        "include/Core/EntryPoint.h"
        "include/Core/Application.h"
        "include/Core/Subsystem.h" 
        "include/Rendering/Window.h"        
        "include/Events/Event.h"
        "include/Events/EventSystem.h" 
        )

target_include_directories(${CORE_LIBRARY}
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/vendor/include"
)

set_target_properties(${CORE_LIBRARY} PROPERTIES CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
	glfw 
	GIT_REPOSITORY	https://github.com/glfw/glfw.git 
	GIT_TAG			3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

target_link_libraries(${CORE_LIBRARY} PRIVATE glfw)
