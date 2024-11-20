cmake_minimum_required(VERSION 3.21)

# Name of the project
project(search_engine LANGUAGES CXX)

# Set standard C++ version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set directories
set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(TEST_DIR ${PROJECT_SOURCE_DIR}/tests)

# Sources project
file(GLOB SOURCES ${SOURCE_DIR}/*.cpp)
file(GLOB HEADERS ${INCLUDE_DIR}/*.h)

include_directories(${INCLUDE_DIR})

# Connecting resources
#qt_add_resources(RESOURCES resources/resources.qrc)


# Connect to FetchContent
include(FetchContent)

# Google Test
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/release-1.12.1.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# nlohmann/json
FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.10.5 # Use the stable version
)

FetchContent_MakeAvailable(googletest json)

# Executable file
add_executable(search_engine
        ${SOURCES}
        ${HEADERS}
        #${RESOURCES}
)

# Library connection
target_link_libraries(search_engine PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(search_engine PRIVATE ${INCLUDE_DIR})

# Tests
enable_testing()
add_subdirectory(${TEST_DIR})
