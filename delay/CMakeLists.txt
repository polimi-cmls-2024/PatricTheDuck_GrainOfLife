cmake_minimum_required(VERSION 3.15) 
project(delay VERSION 0.0.1)
add_subdirectory(/Library/JUCE/ ./JUCE) 
juce_add_plugin(delay
    COMPANY_NAME Garofalo # Change to your name
    # set to false if you want audio input
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    IS_MIDI_EFFECT FALSE 
    NEEDS_MIDI_OUTPUT FALSE 
    COPY_PLUGIN_AFTER_BUILD TRUE 
    PLUGIN_MANUFACTURER_CODE Garofalo

    # should change for each plugin 
    PLUGIN_CODE shr4
    FORMATS AU VST3 Standalone
        
    # should change for each plugin
    PRODUCT_NAME "delay"
)

juce_generate_juce_header(delay)
target_sources(delay PRIVATE
    src/PluginEditor.cpp
    src/PluginProcessor.cpp)
target_compile_definitions(delay
    PUBLIC #
        JUCE_ALSA=1
        JUCE_DIRECTSOUND=1
        JUCE_DISABLE_CAUTIOUS_PARAMETER_ID_CHECKING=1
        JUCE_USE_OGGVORBIS=1
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(delay
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_basics
        juce::juce_audio_devices    
        juce::juce_audio_utils
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_formats
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events   
        juce::juce_graphics
        juce::juce_gui_basics   
        juce::juce_gui_extra    


    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
