#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class PDLBRDAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    PDLBRDAudioProcessorEditor (PDLBRDAudioProcessor&);
    ~PDLBRDAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    PDLBRDAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PDLBRDAudioProcessorEditor)
};
