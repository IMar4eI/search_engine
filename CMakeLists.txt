cmake_minimum_required(VERSION 3.21)

# Name of the project
project(search_engine)

# Set standard C++ version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Directory for headers files
include_directories(${PROJECT_SOURCE_DIR}/include)

# Sources project
file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/include/*.h)

# Connecting resources
#qt_add_resources(RESOURCES resources/resources.qrc)

# FetchContent to load nlohmann/json
include(FetchContent)
FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.10.5 # Use the stable version
)
FetchContent_MakeAvailable(json)

# Executable file
add_executable(search_engine
        ${SOURCES}
        ${HEADERS}
        #${RESOURCES}
)

# Library connection
target_link_libraries(search_engine PRIVATE nlohmann_json::nlohmann_json)
