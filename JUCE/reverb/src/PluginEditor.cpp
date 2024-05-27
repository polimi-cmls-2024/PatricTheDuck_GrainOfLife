/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Riverbero2AudioProcessorEditor::Riverbero2AudioProcessorEditor(Riverbero2AudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Configura gli slider
    auto configureSlider = [this](juce::Slider& slider, const juce::String& name) {
        slider.setSliderStyle(juce::Slider::Rotary);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
        slider.setName(name);
        addAndMakeVisible(slider);
        };

    configureSlider(preDelaySlider, "Pre-Delay");
    preDelayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "preDelay", preDelaySlider);

    configureSlider(decayTimeSlider, "Decay Time");
    decayTimeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "decayTime", decayTimeSlider);

    configureSlider(sizeSlider, "Size");
    sizeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "size", sizeSlider);

    configureSlider(dampingSlider, "Damping");
    dampingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "damping", dampingSlider);

    configureSlider(mixSlider, "Mix");
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "mix", mixSlider);

    configureSlider(widthSlider, "Width");
    widthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "width", widthSlider);

    // Configura le etichette
    auto configureLabel = [this](juce::Label& label, const juce::String& text) {
        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
        };

    configureLabel(preDelayLabel, "Pre-Delay");
    configureLabel(decayTimeLabel, "Decay Time");
    configureLabel(sizeLabel, "Size");
    configureLabel(dampingLabel, "Damping");
    configureLabel(mixLabel, "Mix");
    configureLabel(widthLabel, "Width");

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(600, 400);
}


Riverbero2AudioProcessorEditor::~Riverbero2AudioProcessorEditor()
{
}

//==============================================================================
void Riverbero2AudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (25.0f);
    g.drawFittedText ("Reverb", getLocalBounds(), juce::Justification::centredTop, 1);
}

void Riverbero2AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    // Disposizione dei controlli

    /*
    auto area = getLocalBounds().reduced(40);
    auto sliderArea = area.removeFromTop(area.getHeight() / 2);

    preDelaySlider.setBounds(sliderArea.removeFromLeft(sliderArea.getWidth() / 3).reduced(10));
    decayTimeSlider.setBounds(sliderArea.removeFromLeft(sliderArea.getWidth() / 2).reduced(10));
    sizeSlider.setBounds(sliderArea.reduced(10));

    sliderArea = area;

    dampingSlider.setBounds(sliderArea.removeFromLeft(sliderArea.getWidth() / 3).reduced(10));
    mixSlider.setBounds(sliderArea.removeFromLeft(sliderArea.getWidth() / 2).reduced(10));
    widthSlider.setBounds(sliderArea.reduced(10));
    */

    // Disposizione dei controlli
    auto area = getLocalBounds().reduced(40);
    auto sliderArea = area.removeFromTop(area.getHeight() / 2);

    preDelaySlider.setBounds(sliderArea.removeFromLeft(sliderArea.getWidth() / 3).reduced(10));
    preDelayLabel.setBounds(preDelaySlider.getX(), preDelaySlider.getBottom(), preDelaySlider.getWidth(), 20);

    decayTimeSlider.setBounds(sliderArea.removeFromLeft(sliderArea.getWidth() / 2).reduced(10));
    decayTimeLabel.setBounds(decayTimeSlider.getX(), decayTimeSlider.getBottom(), decayTimeSlider.getWidth(), 20);

    sizeSlider.setBounds(sliderArea.reduced(10));
    sizeLabel.setBounds(sizeSlider.getX(), sizeSlider.getBottom(), sizeSlider.getWidth(), 20);

    sliderArea = area;

    dampingSlider.setBounds(sliderArea.removeFromLeft(sliderArea.getWidth() / 3).reduced(10));
    dampingLabel.setBounds(dampingSlider.getX(), dampingSlider.getBottom(), dampingSlider.getWidth(), 20);

    mixSlider.setBounds(sliderArea.removeFromLeft(sliderArea.getWidth() / 2).reduced(10));
    mixLabel.setBounds(mixSlider.getX(), mixSlider.getBottom(), mixSlider.getWidth(), 20);

    widthSlider.setBounds(sliderArea.reduced(10));
    widthLabel.setBounds(widthSlider.getX(), widthSlider.getBottom(), widthSlider.getWidth(), 20);

}
