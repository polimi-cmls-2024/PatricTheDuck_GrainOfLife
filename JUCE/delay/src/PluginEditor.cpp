#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ComplexDelayAudioProcessorEditor::ComplexDelayAudioProcessorEditor(ComplexDelayAudioProcessor &p)
    : AudioProcessorEditor(&p),
      delayLengthLabel("Delay Length", "Delay (sec):"),
      feedbackLabel("Feedback", "Feedback:"),
      dryMixLabel("DryMix", "Dry:"),
      wetMixLabel("WetMix", "Wet:"),
      modulationDepthLabel("Modulation Depth", "Mod Depth:"),
      modulationFrequencyLabel("Modulation Frequency", "Mod Freq:"),

      audioProcessor(p)
{
    setSize(700, 500);

    // Slider common settings
    auto configureSlider = [this](juce::Slider &slider, double min, double max, double interval)
    {
        slider.setSliderStyle(juce::Slider::Rotary);
        slider.setRange(min, max, interval);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        slider.addListener(this);
        addAndMakeVisible(&slider);
    };

    configureSlider(delayLengthSlider, 0.01, 2.0, 0.01);
    configureSlider(feedbackSlider, 0.0, 0.995, 0.005);
    configureSlider(dryMixSlider, 0.0, 1.0, 0.01);
    configureSlider(wetMixSlider, 0.0, 1.0, 0.01);
    configureSlider(modulationDepthSlider, 0.0, 0.5, 0.01);
    configureSlider(modulationFrequencySlider, 0.01, 10.0, 0.01);

    // Label common settings
    auto configureLabel = [this](juce::Label &label)
    {
        label.setFont(juce::Font(15.0f));
        label.setColour(juce::Label::textColourId, juce::Colours::pink);
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(&label);
    };

    configureLabel(delayLengthLabel);
    delayLengthLabel.attachToComponent(&delayLengthSlider, false);

    configureLabel(feedbackLabel);
    feedbackLabel.attachToComponent(&feedbackSlider, false);

    configureLabel(dryMixLabel);
    dryMixLabel.attachToComponent(&dryMixSlider, false);

    configureLabel(wetMixLabel);
    wetMixLabel.attachToComponent(&wetMixSlider, false);

    configureLabel(modulationDepthLabel);
    modulationDepthLabel.attachToComponent(&modulationDepthSlider, false);

    configureLabel(modulationFrequencyLabel);
    modulationFrequencyLabel.attachToComponent(&modulationFrequencySlider, false);

    startTimer(50);
}

ComplexDelayAudioProcessorEditor::~ComplexDelayAudioProcessorEditor()
{
}

void ComplexDelayAudioProcessorEditor::timerCallback()
{
    auto *ourProcessor = getProcessor();
    // Update the sliders to match the processor's parameters
    delayLengthSlider.setValue(ourProcessor->delayLength, juce::dontSendNotification);
    feedbackSlider.setValue(ourProcessor->feedback, juce::dontSendNotification);
    dryMixSlider.setValue(ourProcessor->dryMix, juce::dontSendNotification);
    wetMixSlider.setValue(ourProcessor->wetMix, juce::dontSendNotification);
    modulationDepthSlider.setValue(ourProcessor->modulationDepth, juce::dontSendNotification);
    modulationFrequencySlider.setValue(ourProcessor->modulationFrequency, juce::dontSendNotification);
}

void ComplexDelayAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    // When a slider is moved, update the processor's parameters
    if (slider == &delayLengthSlider)
    {
        getProcessor()->setParameter(ComplexDelayAudioProcessor::delayLengthParam, static_cast<float>(slider->getValue()));
    }
    else if (slider == &feedbackSlider)
    {
        getProcessor()->setParameter(ComplexDelayAudioProcessor::feedbackParam, static_cast<float>(slider->getValue()));
    }
    else if (slider == &dryMixSlider)
    {
        getProcessor()->setParameter(ComplexDelayAudioProcessor::dryMixParam, static_cast<float>(slider->getValue()));
    }
    else if (slider == &wetMixSlider)
    {
        getProcessor()->setParameter(ComplexDelayAudioProcessor::wetMixParam, static_cast<float>(slider->getValue()));
    }
    else if (slider == &modulationDepthSlider)
    {
        getProcessor()->setParameter(ComplexDelayAudioProcessor::modulationDepthParam, static_cast<float>(slider->getValue()));
    }
    else if (slider == &modulationFrequencySlider)
    {
        getProcessor()->setParameter(ComplexDelayAudioProcessor::modulationFrequencyParam, static_cast<float>(slider->getValue()));
    }
}

void ComplexDelayAudioProcessorEditor::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colour(0xff323e44)); // Background color
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
}

void ComplexDelayAudioProcessorEditor::resized()
{
    int sliderHeight = 150;
    int padding = 20;

    auto area = getLocalBounds();
    auto leftArea = area.removeFromLeft(area.getWidth() / 2);
    auto rightArea = area;

    delayLengthSlider.setBounds(leftArea.removeFromTop(sliderHeight).reduced(padding));
    feedbackSlider.setBounds(leftArea.removeFromTop(sliderHeight).reduced(padding));
    dryMixSlider.setBounds(leftArea.removeFromTop(sliderHeight).reduced(padding));

    wetMixSlider.setBounds(rightArea.removeFromTop(sliderHeight).reduced(padding));
    modulationDepthSlider.setBounds(rightArea.removeFromTop(sliderHeight).reduced(padding));
    modulationFrequencySlider.setBounds(rightArea.removeFromTop(sliderHeight).reduced(padding));
}
