/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EqualizzatoreAudioProcessorEditor::EqualizzatoreAudioProcessorEditor (EqualizzatoreAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Add and configure the cutoff frequency slider
    addAndMakeVisible(cutoffSlider);
    cutoffSlider.setRange(20.0, 20000.0); 
    cutoffSlider.setSkewFactorFromMidPoint(1000.0);
    cutoffSlider.setTextValueSuffix(" Hz");
    cutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), "cutoff", cutoffSlider);

    // Add and configure the cutoff label
    addAndMakeVisible(cutoffLabel);
    cutoffLabel.setText("Cutoff Frequency", juce::dontSendNotification);
    cutoffLabel.attachToComponent(&cutoffSlider, false);

    // Add and configure the Q factor slider
    addAndMakeVisible(qSlider);
    qSlider.setRange(0.1, 10.0);
    qSlider.setSkewFactorFromMidPoint(1.0);
    qSlider.setTextValueSuffix(" Q");
    qAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), "q", qSlider);

    // Add and configure the Q label
    addAndMakeVisible(qLabel);
    qLabel.setText("Q Factor", juce::dontSendNotification);
    qLabel.attachToComponent(&qSlider, false);

    // Add and configure the filter type combo box
    addAndMakeVisible(filterTypeBox);
    filterTypeBox.addItem("Low-pass", 1);
    filterTypeBox.addItem("High-pass", 2);
    filterTypeBox.addItem("Both", 3);
    filterTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.getAPVTS(), "filterType", filterTypeBox);

    // Add and configure the filter type label
    addAndMakeVisible(filterTypeLabel);
    filterTypeLabel.setText("Filter Type", juce::dontSendNotification);
    filterTypeLabel.attachToComponent(&filterTypeBox, false);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 250);
}

EqualizzatoreAudioProcessorEditor::~EqualizzatoreAudioProcessorEditor()
{
}

//==============================================================================
void EqualizzatoreAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (25.0f);
    auto titleArea = getLocalBounds().removeFromTop(30);
    g.drawFittedText ("Equalizzatore", getLocalBounds(), juce::Justification::centredTop, 1);
}

void EqualizzatoreAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto area = getLocalBounds();

    // Aggiungi uno spazio vuoto nella parte superiore
    int topMargin = 50;
    area.removeFromTop(topMargin);

    auto sliderHeight = 50;
    cutoffSlider.setBounds(area.removeFromTop(sliderHeight).reduced(10));
    qSlider.setBounds(area.removeFromTop(sliderHeight).reduced(10));
    filterTypeBox.setBounds(area.removeFromTop(sliderHeight).reduced(10));
}
