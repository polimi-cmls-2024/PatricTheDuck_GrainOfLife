/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Riverbero2AudioProcessor::Riverbero2AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
        parameters(*this, nullptr, "Parameters", {
              std::make_unique<juce::AudioParameterFloat>("preDelay", "Pre-Delay", 0.0f, 200.0f, 20.0f),
              std::make_unique<juce::AudioParameterFloat>("decayTime", "Decay Time", 0.1f, 10.0f, 2.0f),
              std::make_unique<juce::AudioParameterFloat>("size", "Size", 0.1f, 1.0f, 0.5f),
              std::make_unique<juce::AudioParameterFloat>("damping", "Damping", 0.0f, 1.0f, 0.5f),
              std::make_unique<juce::AudioParameterFloat>("mix", "Mix", 0.0f, 1.0f, 0.5f),
              std::make_unique<juce::AudioParameterFloat>("width", "Width", 0.0f, 1.0f, 0.5f),
              std::make_unique<juce::AudioParameterFloat>("earlyReflectionsLevel", "Early Reflections Level", 0.0f, 1.0f, 0.5f),
              std::make_unique<juce::AudioParameterFloat>("lateReflectionsLevel", "Late Reflections Level", 0.0f, 1.0f, 0.5f)
            })
#endif
{
}

Riverbero2AudioProcessor::~Riverbero2AudioProcessor()
{
}

//==============================================================================
const juce::String Riverbero2AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Riverbero2AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Riverbero2AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Riverbero2AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Riverbero2AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Riverbero2AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Riverbero2AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Riverbero2AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Riverbero2AudioProcessor::getProgramName (int index)
{
    return {};
}

void Riverbero2AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Riverbero2AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    reverb.prepare(spec);
}

void Riverbero2AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Riverbero2AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void Riverbero2AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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

        // ..do something to the data...
    }

    // Aggiorna i parametri del riverbero con i valori attuali
    reverbParameters.roomSize = *parameters.getRawParameterValue("size");
    reverbParameters.damping = *parameters.getRawParameterValue("damping");
    reverbParameters.wetLevel = *parameters.getRawParameterValue("mix");
    reverbParameters.dryLevel = 1.0f - reverbParameters.wetLevel;
    reverbParameters.width = *parameters.getRawParameterValue("width");
    reverbParameters.freezeMode = 0.0f; // Manteniamo freezeMode disabilitato per semplicità

    reverb.setParameters(reverbParameters);

    // Applica il riverbero al buffer
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    reverb.process(context);
}

//==============================================================================
bool Riverbero2AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Riverbero2AudioProcessor::createEditor()
{
    return new Riverbero2AudioProcessorEditor (*this);
}

//==============================================================================
void Riverbero2AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::MemoryOutputStream stream(destData, true);
    parameters.state.writeToStream(stream);
}

void Riverbero2AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ValueTree tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
    {
        parameters.state = tree;
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Riverbero2AudioProcessor();
}
