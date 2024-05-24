/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class ComplexDelayAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    ComplexDelayAudioProcessor();
    ~ComplexDelayAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

        float getParameter (int index) override;
        void setParameter (int index, float newValue) override;

    const juce::String getParameterName (int index) override;
    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    int lastUIWidth, lastUIHeight;
    
    enum Parameters
        {
            delayLengthParam = 0,
        dryMixParam,
                wetMixParam,
                feedbackParam,
        modulationDepthParam,
        modulationFrequencyParam,
        };
    
    float delayLength;
        float dryMix;
        float wetMix;
        float feedback;
    float modulationDepth;
    float modulationFrequency;

    
private:
        juce :: AudioSampleBuffer delayBuffer;
        int delayBufferLength;
        int delayReadPosition;
        int delayWritePosition;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ComplexDelayAudioProcessor)
};
