/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BitcrushAudioProcessorEditor::BitcrushAudioProcessorEditor(BitcrushAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(690, 471);

    bitDepthSlider.setSliderStyle(juce::Slider::Rotary);
    bitDepthSlider.addListener(this);
    bitDepthSlider.setRange(1.0, 16.0, 1);
    bitDepthSlider.setColour(juce::Slider::thumbColourId, juce::Colours::cyan);
    bitDepthSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkcyan);
    addAndMakeVisible(&bitDepthSlider);

    bitDepthLabel.setText("Bit Depth", juce::dontSendNotification);
    bitDepthLabel.attachToComponent(&bitDepthSlider, false);
    bitDepthLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    bitDepthLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(&bitDepthLabel);

    sampleRateReductionSlider.setSliderStyle(juce::Slider::Rotary);
    sampleRateReductionSlider.addListener(this);
    sampleRateReductionSlider.setRange(0.0, 1.0, 0.01);
    sampleRateReductionSlider.setColour(juce::Slider::thumbColourId, juce::Colours::magenta);
    sampleRateReductionSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkmagenta);
    addAndMakeVisible(&sampleRateReductionSlider);

    sampleRateReductionLabel.setText("Sample Rate Reduction", juce::dontSendNotification);
    sampleRateReductionLabel.attachToComponent(&sampleRateReductionSlider, false);
    sampleRateReductionLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    sampleRateReductionLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(&sampleRateReductionLabel);
}

BitcrushAudioProcessorEditor::~BitcrushAudioProcessorEditor()
{
}

void BitcrushAudioProcessorEditor::timerCallback()
{
    // upadtes the sliders to reflect the current state of the processor
    BitcrushAudioProcessor *ourProcessor = getProcessor();

    bitDepthSlider.setValue(ourProcessor->bitDepth, juce::dontSendNotification);
    sampleRateReductionSlider.setValue(ourProcessor->sampleRateReduction, juce::dontSendNotification);

    bitDepthLabel.setText("Bit Depth: " + juce::String(bitDepthSlider.getValue()), juce::dontSendNotification);
    sampleRateReductionLabel.setText("Sample Rate Reduction: " + juce::String(sampleRateReductionSlider.getValue()), juce::dontSendNotification);
}

void BitcrushAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    // modifies the parameters of the processor when the sliders are changed
    if (slider == &bitDepthSlider)
    {
        getProcessor()->setParameter(BitcrushAudioProcessor::bitDepthParam, (float)bitDepthSlider.getValue());
    }
    else if (slider == &sampleRateReductionSlider)
    {
        getProcessor()->setParameter(BitcrushAudioProcessor::sampleRateReductionParam, (float)sampleRateReductionSlider.getValue());
    }
}

//==============================================================================
void BitcrushAudioProcessorEditor::paint(juce::Graphics &g)
{

    g.fillAll(juce::Colour(0xff222222));

    g.setColour(juce::Colours::white);
    g.setFont(18.0f);
    g.drawFittedText("Bitcrusher", getLocalBounds(), juce::Justification::centredTop, 1);
}

void BitcrushAudioProcessorEditor::resized()
{
    int sliderWidth = 200;
    int sliderHeight = 200;
    int padding = 20;

    bitDepthSlider.setBounds(padding, getHeight() / 2 - sliderHeight / 2, sliderWidth, sliderHeight);
    sampleRateReductionSlider.setBounds(getWidth() - sliderWidth - padding, getHeight() / 2 - sliderHeight / 2, sliderWidth, sliderHeight);
}
