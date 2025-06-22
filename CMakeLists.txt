# fuck cmake. I'd be using makefiles if it werent for the visual studio integration.
# cmake is a fragile piece of shit that never works.
# fuck cmake.

# TO COMPILE ON MAC:
# brew install glew glfw sfml

# TO COMPILE ON DEBIAN:
# sudo apt update
# sudo apt install libglew-dev libglfw3-dev libsfml-dev libglu1-mesa-dev

cmake_minimum_required(VERSION 3.10)
project(Crayfish)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Include directories for all the libraries
include_directories(
    src
)

add_executable(Crayfish ${SOURCES} "src/main.cpp" "src/RenderFrame.cpp" "src/Triangle.cpp" "src/Camera.cpp" "src/CameraOrtho.cpp" "src/Model.cpp" "src/Scene.cpp" "src/Vertex.cpp" "src/Random.cpp")

set_target_properties(Crayfish PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})
target_link_libraries(Crayfish ${PNG_LIBRARY})

find_program(CPPCHECK cppcheck)
if(CPPCHECK)
    add_custom_target(run-cppcheck
        COMMAND ${CPPCHECK} --enable=all --inconclusive --quiet ${CMAKE_SOURCE_DIR}
        COMMENT "Running static analysis with Cppcheck"
        VERBATIM)
endif()

if (APPLE)
    if (DEBUG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    endif()

    include_directories(/opt/homebrew/include/eigen3)
elseif(UNIX)
    include_directories(/usr/include/eigen3)
endif()
