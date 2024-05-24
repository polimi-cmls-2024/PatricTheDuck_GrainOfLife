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
class BitcrushAudioProcessorEditor  : public juce::AudioProcessorEditor,public juce::Slider::Listener, public juce::Timer
{
public:
    BitcrushAudioProcessorEditor (BitcrushAudioProcessor&);
    ~BitcrushAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged (juce::Slider*) override;
    void timerCallback() override;
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    juce::Label bitDepthLabel, sampleRateReductionLabel;
    juce::Slider bitDepthSlider, sampleRateReductionSlider;
    BitcrushAudioProcessor& audioProcessor;
    BitcrushAudioProcessor* getProcessor() const
            {
                return static_cast <BitcrushAudioProcessor*> (getAudioProcessor());
            }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BitcrushAudioProcessorEditor)
};
