#include "PluginProcessor.h"
#include "PluginEditor.h"

PDLBRDAudioProcessorEditor::PDLBRDAudioProcessorEditor (PDLBRDAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (800, 600);
}

PDLBRDAudioProcessorEditor::~PDLBRDAudioProcessorEditor()
{
}

void PDLBRDAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1a1a2e));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (30.0f));
    g.drawFittedText ("PDLBRD", getLocalBounds(), juce::Justification::centred, 1);

    g.setFont (juce::FontOptions (16.0f));
    g.setColour (juce::Colours::grey);
    g.drawFittedText ("Guitar Effects Pedalboard\nPass-through mode - DSP coming soon!",
                      getLocalBounds().reduced(20).withTrimmedTop(50),
                      juce::Justification::centred, 2);
}

void PDLBRDAudioProcessorEditor::resized()
{
}
