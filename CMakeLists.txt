cmake_minimum_required(VERSION 3.23.1)

project(SimpleGainPlugin VERSION 0.1.0 LANGUAGES C CXX)

# Modern C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Project metadata
set(PROJECT_NAME "SimpleGainPlugin")
set(PRODUCT_NAME "Simple Gain Plugin")
set(COMPANY_NAME "Archie DSP")
set(BUNDLE_ID "com.archiedsp.SimpleGainPlugin")

# Formats: Standalone & VST3 by default, plus AudioUnit (AU) on macOS
set(FORMATS Standalone VST3)
if(APPLE)
    list(APPEND FORMATS AU)
endif()

# Build options
# WARNING: Setting COPY_PLUGIN_TO_SYSTEM to ON requires Administrator privileges on Windows
# or may fail if your DAW is currently open and locking the plugin binary.
option(COPY_PLUGIN_TO_SYSTEM "Copy plugin to system plugin folder after build" OFF)

# JUCE dependency management
# Uses local submodule if present; otherwise fetches JUCE 8 automatically via FetchContent
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/modules/JUCE/CMakeLists.txt")
    message(STATUS "Using local JUCE from modules/JUCE")
    add_subdirectory(modules/JUCE)
else()
    message(STATUS "Local JUCE not found — fetching JUCE 8.0.12 via FetchContent...")
    include(FetchContent)
    FetchContent_Declare(
        JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG        8.0.12
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(JUCE)
endif()

# Windows runtime library configuration (static runtime for easier distribution)
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Define plugin target
juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME "${COMPANY_NAME}"
    BUNDLE_ID "${BUNDLE_ID}"
    COPY_PLUGIN_AFTER_BUILD ${COPY_PLUGIN_TO_SYSTEM}
    PLUGIN_MANUFACTURER_CODE ADSP
    PLUGIN_CODE SmGP
    FORMATS ${FORMATS}
    PRODUCT_NAME "${PRODUCT_NAME}"
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    IS_SYNTH FALSE
)

# Source files
target_sources(${PROJECT_NAME} PRIVATE
    Source/PluginProcessor.h
    Source/PluginProcessor.cpp
    Source/PluginEditor.h
    Source/PluginEditor.cpp
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Source"
)

# Optional asset bundling
file(GLOB_RECURSE Assets CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/*.*"
)

if(Assets)
    juce_add_binary_data(AudioPluginData
        SOURCES ${Assets}
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE AudioPluginData)
endif()

# Preprocessor definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

# Link necessary JUCE modules
# juce_audio_utils automatically provides:
# juce_audio_processors, juce_audio_basics, juce_gui_basics, juce_graphics, juce_events, juce_core, juce_data_structures
target_link_libraries(${PROJECT_NAME} PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)