/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class EqualizzatoreAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    EqualizzatoreAudioProcessorEditor (EqualizzatoreAudioProcessor&);
    ~EqualizzatoreAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    EqualizzatoreAudioProcessor& audioProcessor;

    //EqualizzatoreAudioProcessor& processorRef;

    juce::Slider cutoffSlider;
    juce::Slider qSlider;
    juce::ComboBox filterTypeBox;

    juce::Label cutoffLabel;
    juce::Label qLabel;
    juce::Label filterTypeLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> qAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EqualizzatoreAudioProcessorEditor)
};
