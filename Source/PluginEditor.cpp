#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    // Title Header
    titleLabel.setText("ARCHIE DSP  |  GAIN", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(15.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xfff1f5f9));
    addAndMakeVisible(titleLabel);

    // Gain Rotary Slider
    gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    gainSlider.setTextValueSuffix(" dB");
    gainSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff3b82f6));
    gainSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff334155));
    gainSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff60a5fa));
    gainSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    gainSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffe2e8f0));
    addAndMakeVisible(gainSlider);

    // Connect slider to APVTS "gain" parameter safely
    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "gain", gainSlider);

    // Subtitle / Info Label
    gainLabel.setText("OUTPUT METERS", juce::dontSendNotification);
    gainLabel.setFont(juce::Font(11.0f, juce::Font::bold));
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.setColour(juce::Label::textColourId, juce::Colour(0xff94a3b8));
    addAndMakeVisible(gainLabel);

    // Window dimensions and resizability
    setResizable(true, true);
    setResizeLimits(260, 320, 520, 640);
    setSize(300, 360);

    // Start 30 FPS UI timer for smooth meter rendering
    startTimerHz(30);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void AudioPluginAudioProcessorEditor::timerCallback()
{
    // Smooth meter decay animation
    constexpr float decayRate = 0.82f;
    leftMeterValue  = std::max(processorRef.getRmsLevel(0), leftMeterValue * decayRate);
    rightMeterValue = std::max(processorRef.getRmsLevel(1), rightMeterValue * decayRate);

    repaint();
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics &g)
{
    // Background gradient (Dark Slate)
    juce::ColourGradient bgGradient(
        juce::Colour(0xff0f172a), 0.0f, 0.0f,
        juce::Colour(0xff1e293b), 0.0f, static_cast<float>(getHeight()),
        false
    );
    g.setGradientFill(bgGradient);
    g.fillAll();

    // Inner Card container
    auto cardArea = getLocalBounds().reduced(16).toFloat();
    g.setColour(juce::Colour(0x661e293b));
    g.fillRoundedRectangle(cardArea, 12.0f);
    g.setColour(juce::Colour(0x33475569));
    g.drawRoundedRectangle(cardArea, 12.0f, 1.0f);

    // Dual Stereo Meter Bars
    auto bounds = getLocalBounds().reduced(28);
    auto meterArea = bounds.removeFromBottom(24);

    g.setColour(juce::Colour(0xff090d16));
    g.fillRoundedRectangle(meterArea.toFloat(), 4.0f);

    auto leftBar = meterArea.removeFromTop(meterArea.getHeight() / 2).reduced(1, 2);
    auto rightBar = meterArea.reduced(1, 2);

    // Left channel meter bar (scaled RMS)
    const float leftNorm = juce::jlimit(0.0f, 1.0f, leftMeterValue * 2.2f);
    juce::Colour leftColour = leftNorm > 0.85f ? juce::Colour(0xffef4444) : juce::Colour(0xff10b981);
    g.setColour(leftColour);
    g.fillRect(leftBar.removeFromLeft(static_cast<int>(leftBar.getWidth() * leftNorm)));

    // Right channel meter bar (scaled RMS)
    const float rightNorm = juce::jlimit(0.0f, 1.0f, rightMeterValue * 2.2f);
    juce::Colour rightColour = rightNorm > 0.85f ? juce::Colour(0xffef4444) : juce::Colour(0xff10b981);
    g.setColour(rightColour);
    g.fillRect(rightBar.removeFromLeft(static_cast<int>(rightBar.getWidth() * rightNorm)));
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(24);

    // Header title
    titleLabel.setBounds(area.removeFromTop(36));

    // Meter area reserved at the bottom
    area.removeFromBottom(30);
    gainLabel.setBounds(area.removeFromBottom(20));

    // Rotary slider gets the central space
    gainSlider.setBounds(area.reduced(10));
}
