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

# Searching all of UI files
#file(GLOB UI_FILES ${SOURCE_DIR}/*.ui)
#set(SOURCES ${SOURCES} ${UI_FILES})

# Enabling automatic processing of Qt files
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Finding Qt
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Include directories
include_directories(${INCLUDE_DIR})

# Connecting resources.qrc
#qt_add_resources(QRC_RESOURCES resources/resources.qrc)

# Connect to FetchContent
include(FetchContent)

# Google Test
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/release-1.12.1.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(googletest)

# Executable file
add_executable(search_engine
        ${SOURCES}
        ${HEADERS}
        ${QRC_RESOURCES}
)

# Library connection
target_link_libraries(search_engine PRIVATE Qt6::Core Qt6::Widgets)

# Add test executable
enable_testing()

#Create test executable
add_executable(unit_tests)

# Add test sources
file(GLOB TEST_SOURCES ${TEST_DIR}/*.cpp)

target_sources(unit_tests PRIVATE
        ${TEST_SOURCES}
        ${SOURCE_DIR}/ConverterJSON.cpp
        ${SOURCE_DIR}/InvertedIndex.cpp
        ${SOURCE_DIR}/SearchServer.cpp
)

# Link test libraries
target_include_directories(unit_tests PRIVATE ${INCLUDE_DIR})
target_link_libraries(unit_tests PRIVATE gtest_main Qt6::Core Qt6::Widgets)

# GoogleTest integration
include(GoogleTest)
gtest_discover_tests(unit_tests)
