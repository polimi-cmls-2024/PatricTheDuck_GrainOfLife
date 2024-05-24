/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ComplexDelayAudioProcessor::ComplexDelayAudioProcessor(): delayBuffer(2,1)
{delayLength = 0.5;
    dryMix = 1.0;
        wetMix = 0.0;
        feedback = 0.75;
    delayBufferLength = 1;
     modulationDepth = 0.1;
    modulationFrequency = 1 ;
      
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

float ComplexDelayAudioProcessor::getParameter (int index)
                   {
                       switch (index) {
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
                   
                   void ComplexDelayAudioProcessor::setParameter (int index, float newValue)
                   {
                       switch (index) {
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
                               delayReadPosition = (int) (delayWritePosition - (delayLength * getSampleRate()) + delayBufferLength) % delayBufferLength;
                               break;
                           case modulationDepthParam:
                                modulationDepth = newValue;
                               break;
                           case modulationFrequencyParam:
                              modulationFrequency = newValue;
                               break;
                           default:
                               break;;
                       }
                   }
                   
const juce::String ComplexDelayAudioProcessor::getParameterName (int index)
                   {
                       switch (index) {
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
                               return juce::String();;
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
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ComplexDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ComplexDelayAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ComplexDelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void ComplexDelayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ComplexDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
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
bool ComplexDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void ComplexDelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();
    int dpr, dpw;
    float modIndex = 0;
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        float* delayData = delayBuffer.getWritePointer(fmin(channel, delayBuffer.getNumChannels() - 1));
               
        dpr = delayReadPosition;
        dpw = delayWritePosition;
        
        for (int i = 0; i < numSamples; ++i) {
            const float in = channelData[i];
            float out = 0.0;
            
            double delayTime = delayLength / 2.0 * (1 + modulationDepth * sin(2 * M_PI * modulationFrequency * modIndex / getSampleRate()));
        
            // Get fractional and integer parts of delay time
            size_t intDelay = static_cast<size_t>(delayTime);
            double fracDelay = delayTime - intDelay;
        
            // Interpolation
            double delayedSample = (1 - fracDelay) * delayData[(dpw - intDelay - 1 + delayBufferLength) % delayBufferLength]
                                 + fracDelay * delayData[(dpw - intDelay + delayBufferLength) % delayBufferLength];
        
            out = in + feedback * delayedSample; // Add feedback
            delayData[dpw] = out; // Store output in buffer
            dpw = (dpw + 1) % delayBufferLength; // Update index for circular buffer
            modIndex = fmod(modIndex + 1, getSampleRate());
            
            channelData[i] = out;
        }
    }
            
    delayReadPosition = dpr;
    delayWritePosition = dpw;
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
}



//==============================================================================
bool ComplexDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ComplexDelayAudioProcessor::createEditor()
{
    return new ComplexDelayAudioProcessorEditor (*this);
}

//==============================================================================
void ComplexDelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}


void ComplexDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ComplexDelayAudioProcessor();
}

