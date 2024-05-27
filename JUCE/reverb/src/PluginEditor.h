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
class Riverbero2AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Riverbero2AudioProcessorEditor (Riverbero2AudioProcessor&);
    ~Riverbero2AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Riverbero2AudioProcessor& audioProcessor;

    juce::Slider preDelaySlider;
    juce::Slider decayTimeSlider;
    juce::Slider sizeSlider;
    juce::Slider dampingSlider;
    juce::Slider mixSlider;
    juce::Slider widthSlider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> preDelayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayTimeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sizeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dampingAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthAttachment;


    juce::Label preDelayLabel;
    juce::Label decayTimeLabel;
    juce::Label sizeLabel;
    juce::Label dampingLabel;
    juce::Label mixLabel;
    juce::Label widthLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Riverbero2AudioProcessorEditor)
};
