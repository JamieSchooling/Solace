# CMakeList.txt : CMake project for Demo Application

set(APPLICATION Editor)

# Add source to this project's executable.
add_executable (${APPLICATION})

target_sources(${APPLICATION}
    PRIVATE
        "src/Editor.cpp"

    PRIVATE
    FILE_SET HEADERS
    BASE_DIRS
        include
    FILES
        "include/Editor.h")



set_target_properties(${APPLICATION} PROPERTIES CXX_STANDARD 20)

target_link_libraries(${APPLICATION} PUBLIC Engine)

if(WIN32 AND MSVC)
    # Hide console in Release builds
    target_link_options(${APPLICATION} PRIVATE
        "$<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>"
    )
endif()

add_custom_command(TARGET "${APPLICATION}" POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${APPLICATION}>/resources)