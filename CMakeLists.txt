cmake_minimum_required(VERSION 3.28)

project(JucePlugins)

set(CMAKE_CXX_STANDARD 23)

set(PROJECT_VERSION 0.1.0)

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CPM_SOURCE_CACHE ${LIB_DIR}/cpm)

include(cmake/cpm.cmake)

CPMAddPackage(
    NAME JUCE
    GITHUB_REPOSITORY juce-framework/JUCE
    GIT_TAG 8.0.3
    VERSION 8.0.3
    SOURCE_DIR ${LIB_DIR}/juce
)

CPMAddPackage(
    NAME GOOGLE_TEST
    GITHUB_REPOSITORY google/googletest
    VERSION 1.15.2
    SOURCE_DIR ${LIB_DIR}/googletest
    OPTIONS
        "INSTALL_GTEST OFF"
        "gtest_force_shared_crt ON"

)

add_compile_options(-Wall -Wextra -Wpedantic)

include_directories(includes)

add_subdirectory(ArpeggiatorPlugin)
add_subdirectory(HelloWorld)
add_subdirectory(Synth)
add_subdirectory(WaveTable)
add_subdirectory(DSP)
add_subdirectory(Host)
add_subdirectory(MidiDrivers)
add_subdirectory(Tests)