cmake_minimum_required(VERSION 3.28)

project(PluginTests)

enable_testing()

add_executable(${PROJECT_NAME} test.cpp)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        ${GOOGLE_TEST_SOURCE_DIR}/googletest/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../HelloWorld/includes
        ${JUCE_SOURCE_DIR}/modules
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        HelloWorld
        juce::juce_dsp
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})