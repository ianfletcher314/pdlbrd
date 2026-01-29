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

    // Compressor controls
    juce::Slider thresholdSlider;
    juce::Slider ratioSlider;
    juce::Slider attackSlider;
    juce::Slider releaseSlider;
    juce::Slider makeupSlider;
    juce::Slider blendSlider;
    juce::ToggleButton bypassButton;

    // Labels
    juce::Label thresholdLabel;
    juce::Label ratioLabel;
    juce::Label attackLabel;
    juce::Label releaseLabel;
    juce::Label makeupLabel;
    juce::Label blendLabel;

    // Attachments (connect sliders to parameters)
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ratioAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> makeupAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> blendAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;

    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PDLBRDAudioProcessorEditor)
};
