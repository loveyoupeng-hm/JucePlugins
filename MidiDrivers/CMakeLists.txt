cmake_minimum_required(VERSION 3.28)

project(MidiDrivers VERSION ${PROJECT_VERSION})

juce_add_gui_app(
    ${PROJECT_NAME}
    COMPANY_NAME loveyoupeng
    IS_SYNTH FALSE 
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
)

target_compile_definitions(
    ${PROJECT_NAME} 
    PUBLIC 
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:Synth,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:Synth,JUCE_VERSION>"
)

target_link_libraries(
    ${PROJECT_NAME} 
    PRIVATE 
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        includes
)

target_sources(
    ${PROJECT_NAME}
    PRIVATE
        impls/Main.cpp
)