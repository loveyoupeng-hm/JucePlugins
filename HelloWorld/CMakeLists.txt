cmake_minimum_required(VERSION 3.28)

project(HelloWorld VERSION 0.1.0)

juce_add_plugin(
    ${PROJECT_NAME}
    COMPANY_NAME loveyoupeng
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    FORMATS VST3
)
target_compile_definitions(
    ${PROJECT_NAME} 
    PUBLIC 
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(
    ${PROJECT_NAME} 
    PRIVATE 
        juce::juce_audio_utils
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
        impls/PluginEditor.cpp
        impls/PluginProcessor.cpp
)