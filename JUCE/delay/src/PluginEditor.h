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
class ComplexDelayAudioProcessorEditor  : public juce::AudioProcessorEditor,
public juce::Slider::Listener, public juce::Timer

{
public:
    ComplexDelayAudioProcessorEditor (ComplexDelayAudioProcessor&);
    ~ComplexDelayAudioProcessorEditor() override;

    //==============================================================================
    void timerCallback() override;
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged (juce::Slider*) override;
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    juce::Label delayLengthLabel, feedbackLabel, dryMixLabel, wetMixLabel, modulationDepthLabel, modulationFrequencyLabel;
    juce::Slider delayLengthSlider, feedbackSlider, dryMixSlider, wetMixSlider, modulationDepthSlider, modulationFrequencySlider;
    ComplexDelayAudioProcessor& audioProcessor;

    ComplexDelayAudioProcessor* getProcessor() const
        {
            return static_cast <ComplexDelayAudioProcessor*> (getAudioProcessor());
        }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ComplexDelayAudioProcessorEditor)
};

