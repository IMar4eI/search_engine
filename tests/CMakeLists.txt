# Name test project
project(search_engine_tests LANGUAGES CXX)

# Executable file
add_executable(unit_tests)

file(GLOB TEST_SOURCES ${CMAKE_SOURCE_DIR}/tests/*.cpp)
target_sources(unit_tests PRIVATE
    ${TEST_SOURCES}
    ${CMAKE_SOURCE_DIR}/src/ConverterJSON.cpp
    ${CMAKE_SOURCE_DIR}/src/InvertedIndex.cpp
    ${CMAKE_SOURCE_DIR}/src/SearchServer.cpp
)

# Header files
target_include_directories(unit_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(unit_tests PRIVATE gtest_main nlohmann_json)

include(GoogleTest)
gtest_discover_tests(unit_tests)