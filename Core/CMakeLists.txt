# CMakeList.txt : CMake project for Core Library

set(CORE_LIBRARY Core)

# Add source to this project's executable.
add_library (${CORE_LIBRARY})

target_sources(${CORE_LIBRARY}
    PRIVATE
        "src/Application.cpp"

    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
        include
    FILES
        "include/EntryPoint.h"
        "include/Application.h"
)

target_include_directories(${CORE_LIBRARY}
    PUBLIC "${CMAKE_SOURCE_DIR}/Core/include"
)

set_target_properties(${CORE_LIBRARY} PROPERTIES CXX_STANDARD 20)
