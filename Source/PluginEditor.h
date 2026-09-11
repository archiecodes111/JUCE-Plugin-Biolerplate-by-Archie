#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                              private juce::Timer
{
public:
    explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics &) override;
    void resized() override;

private:
    void timerCallback() override;

    // Reference to processor
    AudioPluginAudioProcessor &processorRef;

    // UI Components
    juce::Label titleLabel;
    juce::Slider gainSlider;
    juce::Label gainLabel;

    // APVTS Parameter Attachment
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;

    // Meter animation decay tracking
    float leftMeterValue = 0.0f;
    float rightMeterValue = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};

// Convenience alias matching the CMake project name
using SimpleGainAudioProcessorEditor = AudioPluginAudioProcessorEditor;
