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

    // Compressor
    juce::Slider compThresholdSlider, compRatioSlider, compAttackSlider;
    juce::Slider compReleaseSlider, compMakeupSlider, compBlendSlider;
    juce::ToggleButton compBypassButton;
    juce::Label compThresholdLabel, compRatioLabel, compAttackLabel;
    juce::Label compReleaseLabel, compMakeupLabel, compBlendLabel;

    // Distortion
    juce::Slider distDriveSlider, distToneSlider, distLevelSlider;
    juce::ComboBox distTypeBox;
    juce::ToggleButton distBypassButton;
    juce::Label distDriveLabel, distToneLabel, distLevelLabel, distTypeLabel;

    // Amp Sim
    juce::Slider ampGainSlider, ampBassSlider, ampMidSlider;
    juce::Slider ampMidFreqSlider, ampTrebleSlider, ampMasterSlider;
    juce::ComboBox ampTypeBox;
    juce::ToggleButton ampBypassButton;
    juce::Label ampGainLabel, ampBassLabel, ampMidLabel;
    juce::Label ampMidFreqLabel, ampTrebleLabel, ampMasterLabel, ampTypeLabel;

    // Modulation
    juce::Slider modRateSlider, modDepthSlider, modBlendSlider;
    juce::ComboBox modTypeBox;
    juce::ToggleButton modBypassButton;
    juce::Label modRateLabel, modDepthLabel, modBlendLabel, modTypeLabel;

    // Compressor attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compThresholdAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compRatioAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compAttackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compReleaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compMakeupAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compBlendAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> compBypassAttachment;

    // Distortion attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distDriveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distToneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distLevelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> distTypeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> distBypassAttachment;

    // Amp Sim attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ampGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ampBassAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ampMidAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ampMidFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ampTrebleAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ampMasterAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> ampTypeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> ampBypassAttachment;

    // Modulation attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modRateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modDepthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modBlendAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modTypeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> modBypassAttachment;

    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText, juce::Colour colour);
    void setupComboBox(juce::ComboBox& box, juce::Label& label, const juce::String& labelText, juce::Colour colour);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PDLBRDAudioProcessorEditor)
};
