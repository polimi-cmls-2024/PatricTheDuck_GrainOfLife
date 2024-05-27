

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BitcrushAudioProcessor::BitcrushAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      )
#endif
{
}

BitcrushAudioProcessor::~BitcrushAudioProcessor()
{
    bitDepth = 1;
    sampleRateReduction = 0;
}

//==============================================================================
const juce::String BitcrushAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BitcrushAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool BitcrushAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool BitcrushAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double BitcrushAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BitcrushAudioProcessor::getNumPrograms()
{
    return 1;
}

int BitcrushAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BitcrushAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String BitcrushAudioProcessor::getProgramName(int index)
{
    return {};
}

void BitcrushAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
}

float BitcrushAudioProcessor::getParameter(int index)
{
    switch (index)
    {
    case bitDepthParam:
        return bitDepth;
    case sampleRateReductionParam:
        return sampleRateReduction;
    default:
        return 0.0f;
    }
}

void BitcrushAudioProcessor::setParameter(int index, float newValue)
{
    switch (index)
    {
    case bitDepthParam:
        bitDepth = newValue;
        break;
    case sampleRateReductionParam:
        sampleRateReduction = newValue;
        break;
    default:
        break;
        ;
    }
}

//==============================================================================
void BitcrushAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
}

void BitcrushAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BitcrushAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
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

void BitcrushAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto *channelData = buffer.getWritePointer(channel);

        // Bit depth refers to the number of bits used to represent each audio sample.
        float bitDepthFactor = pow(2, 16 - bitDepth);

        // Sample rate refers to the number of audio samples per second.
        float sampleRateFactor = 1.0f / sampleRateReduction;

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {

            float inputSample = channelData[sample];

            // Apply bit depth reduction
            float crushedSample = std::floor(inputSample * bitDepthFactor) / bitDepthFactor;

            // Apply sample rate reduction
            if (sample % static_cast<int>(sampleRateFactor) == 0)
            {
                channelData[sample] = crushedSample;
            }
            else
            {
                channelData[sample] = 0;
            }
        }
    }
}

//==============================================================================
bool BitcrushAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor *BitcrushAudioProcessor::createEditor()
{
    return new BitcrushAudioProcessorEditor(*this);
}

//==============================================================================
void BitcrushAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
}

void BitcrushAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new BitcrushAudioProcessor();
}
