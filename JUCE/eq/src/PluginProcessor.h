#pragma once

#include <JuceHeader.h>

class SimpleFilter
{
public:
    SimpleFilter()
    {
        lowPassFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
        highPassFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
        setCutoffFrequency(1000.0f);
        setQFactor(0.707f);
    }

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        lowPassFilter.prepare(spec);
        highPassFilter.prepare(spec);
    }

    void processBlock(juce::AudioBuffer<float>& buffer, int filterType)
    {
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        
        switch (filterType)
        {
        case 0: // Low-pass filter
            lowPassFilter.process(context);
            break;
        case 1: // High-pass filter
            highPassFilter.process(context);
            break;
        case 2: // Both filters
            lowPassFilter.process(context);
            highPassFilter.process(context);
            break;
        default:
            break;
        }
    }

    void setCutoffFrequency(float newCutoff)
    {
        cutoffFrequency = newCutoff;
        updateFilter();
    }

    void setQFactor(float newQ)
    {
        qFactor = newQ;
        updateFilter();
    }

private:
    void updateFilter()
    {
        lowPassFilter.setCutoffFrequency(cutoffFrequency);
        lowPassFilter.setResonance(qFactor);
        highPassFilter.setCutoffFrequency(cutoffFrequency);
        highPassFilter.setResonance(qFactor);
    }

    float cutoffFrequency = 1000.0f;
    float qFactor = 0.707f;

    juce::dsp::StateVariableTPTFilter<float> lowPassFilter;
    juce::dsp::StateVariableTPTFilter<float> highPassFilter;
};



//==============================================================================
//==============================================================================
//==============================================================================
class EqualizzatoreAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    EqualizzatoreAudioProcessor();
    ~EqualizzatoreAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return parameters; } // Aggiungi questo metodo


private:
    SimpleFilter filter;
    int filterType;

    juce::AudioProcessorValueTreeState parameters;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EqualizzatoreAudioProcessor)
};
