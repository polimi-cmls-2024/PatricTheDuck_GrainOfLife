/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ComplexDelayAudioProcessor::ComplexDelayAudioProcessor() : delayBuffer(2, 1)
{
    delayLength = 0.5;
    dryMix = 1.0;
    wetMix = 0.0;
    feedback = 0.75; // The delayed sample is fed back into the delay line
    delayBufferLength = 1;
    modulationDepth = 0.1;   // Defines how much the delay time is modulated
    modulationFrequency = 1; // Determines the speed of the modulation

    delayReadPosition = 0;
    delayWritePosition = 0;

    lastUIWidth = 370;
    lastUIHeight = 140;
}

ComplexDelayAudioProcessor::~ComplexDelayAudioProcessor()
{
}

//==============================================================================
const juce::String ComplexDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

float ComplexDelayAudioProcessor::getParameter(int index)
{
    // Switch case to return the value of the parameter based on the index
    switch (index)
    {
    case dryMixParam:
        return dryMix;
    case wetMixParam:
        return wetMix;
    case feedbackParam:
        return feedback;
    case delayLengthParam:
        return delayLength;
    case modulationDepthParam:
        return modulationDepth;
    case modulationFrequencyParam:
        return modulationFrequency;
    default:
        return 0.0f;
    }
}

void ComplexDelayAudioProcessor::setParameter(int index, float newValue)
{
    // Switch case to set the value of the parameter based on the index
    switch (index)
    {
    case dryMixParam:
        dryMix = newValue;
        break;
    case wetMixParam:
        wetMix = newValue;
        break;
    case feedbackParam:
        feedback = newValue;
        break;
    case delayLengthParam:
        delayLength = newValue;
        delayReadPosition = (int)(delayWritePosition - (delayLength * getSampleRate()) + delayBufferLength) % delayBufferLength;
        break;
    case modulationDepthParam:
        modulationDepth = newValue;
        break;
    case modulationFrequencyParam:
        modulationFrequency = newValue;
        break;
    default:
        break;
        ;
    }
}

const juce::String ComplexDelayAudioProcessor::getParameterName(int index)
{
    // Switch case to return the name of the parameter based on the index
    switch (index)
    {
    case dryMixParam:
        return "dry mix";
    case wetMixParam:
        return "wet mix";
    case feedbackParam:
        return "feedback";
    case delayLengthParam:
        return "delay";
    case modulationDepthParam:
        return "modulationDepth";
    case modulationFrequencyParam:
        return "modulationFrequency";
    default:
        return juce::String();
        ;
    }
}

bool ComplexDelayAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool ComplexDelayAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool ComplexDelayAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double ComplexDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ComplexDelayAudioProcessor::getNumPrograms()
{
    return 1;
}

int ComplexDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ComplexDelayAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String ComplexDelayAudioProcessor::getProgramName(int index)
{
    return {};
}

void ComplexDelayAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
}

//==============================================================================
void ComplexDelayAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    delayBufferLength = static_cast<int>(2.0 * sampleRate);
    if (delayBufferLength < 1)
        delayBufferLength = 1;

    delayBuffer.setSize(2, delayBufferLength);
    delayBuffer.clear();

    delayReadPosition = 0;
    delayWritePosition = 0;
}

void ComplexDelayAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ComplexDelayAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else

    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void ComplexDelayAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();
    int dpr, dpw; // These indices keep track of where to read from and write to in the delay buffer.
    float modIndex = 0;

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float *channelData = buffer.getWritePointer(channel);
        float *delayData = delayBuffer.getWritePointer(fmin(channel, delayBuffer.getNumChannels() - 1));

        dpr = delayReadPosition;
        dpw = delayWritePosition;

        for (int i = 0; i < numSamples; ++i)
        {
            const float in = channelData[i];
            float out = 0.0;

            double delayTime = delayLength / 2.0 * (1 + modulationDepth * sin(2 * M_PI * modulationFrequency * modIndex / getSampleRate()));

            size_t intDelay = static_cast<size_t>(delayTime); // The integer part of the delay time determines how many samples to delay.

            double fracDelay = delayTime - intDelay; // The fractional part determines the interpolation weight between adjacent delay buffer samples.

            // Interpolation
            double delayedSample = (1 - fracDelay) * delayData[(dpw - intDelay - 1 + delayBufferLength) % delayBufferLength] + fracDelay * delayData[(dpw - intDelay + delayBufferLength) % delayBufferLength];

            out = in + feedback * delayedSample;            // Add feedback
            delayData[dpw] = out;                           // Store output in buffer
            dpw = (dpw + 1) % delayBufferLength;            // Update index for circular buffer
            modIndex = fmod(modIndex + 1, getSampleRate()); // Keeps track of the current position in the modulation waveform

            channelData[i] = out;
        }
    }
    // Update read and write positions
    delayReadPosition = dpr;
    delayWritePosition = dpw;

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());
}

//==============================================================================
bool ComplexDelayAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor *ComplexDelayAudioProcessor::createEditor()
{
    return new ComplexDelayAudioProcessorEditor(*this);
}

//==============================================================================
void ComplexDelayAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
}

void ComplexDelayAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
}

//==============================================================================

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new ComplexDelayAudioProcessor();
}
