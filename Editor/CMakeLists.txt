# CMakeList.txt : CMake project for Demo Application

set(APPLICATION Editor)

# Add source to this project's executable.
add_executable (${APPLICATION})

target_sources(${APPLICATION}
    PRIVATE
        "src/Core/Editor.cpp"
        "src/Core/EditorSystem.cpp"
        "src/Inspectors/ComponentInspector.cpp"
        "src/Inspectors/TransformInspector.cpp"
        "src/Inspectors/CameraInspector.cpp" 
        "src/Windows/EditorWindow.cpp"
        "src/Windows/SceneHierarchy.cpp"
        "src/Windows/InspectorWindow.cpp"

    PRIVATE
    FILE_SET HEADERS
    BASE_DIRS
        include
    FILES
        "include/Core/Editor.h" 
        "include/Core/EditorSystem.h"
        "include/Inspectors/EditorProperty.h"
        "include/Inspectors/ComponentInspector.h" 
        "include/Inspectors/TransformInspector.h"
        "include/Inspectors/CameraInspector.h"
        "include/Inspectors/InspectorRegistry.h" 
        "include/Windows/EditorWindow.h"
 "include/Windows/SceneHierarchy.h" "include/Windows/InspectorWindow.h")

include(FetchContent)

FetchContent_Declare(imgui
    URL https://github.com/ocornut/imgui/archive/refs/tags/v1.92.7-docking.zip
)

FetchContent_Declare(nfd
    URL https://github.com/mlabbe/nativefiledialog/archive/refs/tags/release_116.zip
)

FetchContent_MakeAvailable(imgui nfd)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_link_libraries(imgui PRIVATE glfw)
target_include_directories(imgui
    PUBLIC
      ${imgui_SOURCE_DIR}
      ${imgui_SOURCE_DIR}/backends
)

add_library(nfd STATIC
  ${nfd_SOURCE_DIR}/src/nfd_common.c
  ${nfd_SOURCE_DIR}/src/nfd_win.cpp
)

target_include_directories(nfd
    PUBLIC
      ${nfd_SOURCE_DIR}/src
      ${nfd_SOURCE_DIR}/src/include
)

set_target_properties(${APPLICATION} PROPERTIES CXX_STANDARD 20)

target_link_libraries(${APPLICATION} PUBLIC Engine)
target_link_libraries(${APPLICATION} PUBLIC imgui)
target_link_libraries(${APPLICATION} PRIVATE nfd)

if(WIN32 AND MSVC)
    # Hide console in Release builds
    target_link_options(${APPLICATION} PRIVATE
        "$<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>"
    )
endif()

add_custom_command(TARGET "${APPLICATION}" POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${APPLICATION}>/resources)