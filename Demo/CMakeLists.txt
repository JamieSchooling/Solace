# CMakeList.txt : CMake project for Demo Application

set(APPLICATION Demo)

# Add source to this project's executable.
add_executable (${APPLICATION})

target_sources(${APPLICATION}
    PRIVATE
        "src/Demo.cpp"

    PRIVATE
    FILE_SET HEADERS
    BASE_DIRS
        include
    FILES
        "include/Demo.h")



set_target_properties(${APPLICATION} PROPERTIES CXX_STANDARD 20)

target_link_libraries(${APPLICATION} PUBLIC Engine)

if(WIN32 AND MSVC)
    # Hide console in Release builds
    target_link_options(${APPLICATION} PRIVATE
        "$<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>"
    )
endif()
