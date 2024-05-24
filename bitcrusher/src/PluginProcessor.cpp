/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BitcrushAudioProcessor::BitcrushAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

BitcrushAudioProcessor::~BitcrushAudioProcessor()
{bitDepth = 1;
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
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BitcrushAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BitcrushAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BitcrushAudioProcessor::getProgramName (int index)
{
    return {};
}

void BitcrushAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

float BitcrushAudioProcessor::getParameter (int index)
                   {
                       switch (index) {
                           case bitDepthParam:
                               return bitDepth;
                           case sampleRateReductionParam:
                               return sampleRateReduction;
                           default:
                               return 0.0f;
                       }
                   }
                   
void BitcrushAudioProcessor::setParameter (int index, float newValue)
                   {
                       switch (index) {
                           case bitDepthParam:
                               bitDepth = newValue;
                               break;
                           case sampleRateReductionParam:
                               sampleRateReduction = newValue;
                               break;
                           default:
                               break;;
                       }
                   }
                   

//==============================================================================
void BitcrushAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void BitcrushAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BitcrushAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void BitcrushAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

         float bitDepthFactor = pow(2, 16 - bitDepth);
        
        // Calculate sample rate reduction factor
        float sampleRateFactor = 1.0f / sampleRateReduction;

            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                // Apply bit depth reduction
                float inputSample = channelData[sample];
                float crushedSample = std::floor(inputSample * bitDepthFactor) / bitDepthFactor;

                // Apply sample rate reduction
                if (sample % static_cast<int>(sampleRateFactor) == 0)
                {
                    channelData[sample] = crushedSample;
                }else{
                    channelData[sample] = 0;}
        
            }
        }
    }


//==============================================================================
bool BitcrushAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BitcrushAudioProcessor::createEditor()
{
    return new BitcrushAudioProcessorEditor (*this);
}

//==============================================================================
void BitcrushAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void BitcrushAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BitcrushAudioProcessor();
}
