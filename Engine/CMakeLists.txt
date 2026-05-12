# CMakeList.txt : CMake project for Core Library

set(CORE_LIBRARY Engine)

# Add source to this project's executable.
add_library (${CORE_LIBRARY})

target_sources(${CORE_LIBRARY}
    PRIVATE
        "src/Core/Application.cpp"
        "src/Rendering/Window.cpp"
        "src/Rendering/RenderSystem.cpp"
        "src/Events/EventSystem.cpp"
        "src/Rendering/VAO.cpp"
        "src/Rendering/Shader.cpp"
        "src/Rendering/Material.cpp"
        "src/Scenes/Scene.cpp"
        "src/Scenes/SceneSystem.cpp"
        "src/Rendering/Camera.cpp"  
        "src/Rendering/FBO.cpp"
        "src/Rendering/Texture.cpp"
        "src/Scenes/SceneSerialiser.cpp"


    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
        include
    FILES
        "include/Core/EntryPoint.h"
        "include/Core/Application.h"
        "include/Core/Subsystem.h" 
        "include/Rendering/Window.h"        
        "include/Rendering/RenderSystem.h" 
        "include/Events/Event.h"
        "include/Events/EventSystem.h"  
        "include/Rendering/VAO.h"
        "include/Rendering/Shader.h"
        "include/Rendering/Material.h"  
        "include/Rendering/ShaderDataTypes.h" 
        "include/Rendering/Camera.h" 
        "include/Scenes/Scene.h"
        "include/Scenes/SceneSystem.h"  
        "include/Rendering/FrameRenderData.h"  
        "include/Scenes/DefaultScene.h" 
        "include/Rendering/MeshRender.h" 
        "include/Transform/Transform.h" 
        "include/Rendering/FBO.h" 
        "include/Rendering/Texture.h" 
        "include/Scenes/NameComponent.h"
        "include/Scenes/SceneSerialiser.h" 
        "include/Reflection/Property.h" 
        "include/Reflection/ComponentReflection.h" 
        "include/Reflection/ReflectionRegistry.h" 
        "include/Reflection/TypeID.h" 
        "include/Reflection/Reflection.h")

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

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	1.0.3
)

FetchContent_Declare(entt
	GIT_REPOSITORY	https://github.com/skypjack/entt.git
	GIT_TAG			v3.16.0
)

FetchContent_Declare(json 
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
)

FetchContent_Declare(assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v6.0.5
)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw glm entt json assimp)

target_link_libraries(${CORE_LIBRARY} PUBLIC glfw)
target_link_libraries(${CORE_LIBRARY} PUBLIC glm)
target_link_libraries(${CORE_LIBRARY} PUBLIC EnTT)
target_link_libraries(${CORE_LIBRARY} PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(${CORE_LIBRARY} PUBLIC assimp)
