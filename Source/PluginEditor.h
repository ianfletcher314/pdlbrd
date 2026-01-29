#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class PDLBRDAudioProcessorEditor : public juce::AudioProcessorEditor,
                                   public juce::Timer
{
public:
    PDLBRDAudioProcessorEditor (PDLBRDAudioProcessor&);
    ~PDLBRDAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    PDLBRDAudioProcessor& audioProcessor;

    // Signal chain display
    std::array<juce::TextButton, PDLBRDAudioProcessor::NUM_EFFECTS> chainButtons;
    std::array<juce::TextButton, PDLBRDAudioProcessor::NUM_EFFECTS> upButtons;
    std::array<juce::TextButton, PDLBRDAudioProcessor::NUM_EFFECTS> downButtons;
    void updateChainDisplay();

    // Compressor 1
    juce::Slider comp1ThresholdSlider, comp1RatioSlider, comp1AttackSlider;
    juce::Slider comp1ReleaseSlider, comp1MakeupSlider, comp1BlendSlider;
    juce::ToggleButton comp1BypassButton;
    juce::Label comp1ThresholdLabel, comp1RatioLabel, comp1AttackLabel;
    juce::Label comp1ReleaseLabel, comp1MakeupLabel, comp1BlendLabel;

    // Compressor 2
    juce::Slider comp2ThresholdSlider, comp2RatioSlider, comp2AttackSlider;
    juce::Slider comp2ReleaseSlider, comp2MakeupSlider, comp2BlendSlider;
    juce::ToggleButton comp2BypassButton;
    juce::Label comp2ThresholdLabel, comp2RatioLabel, comp2AttackLabel;
    juce::Label comp2ReleaseLabel, comp2MakeupLabel, comp2BlendLabel;

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

    // Reverb
    juce::Slider revDecaySlider, revToneSlider, revBlendSlider;
    juce::ComboBox revTypeBox;
    juce::ToggleButton revBypassButton;
    juce::Label revDecayLabel, revToneLabel, revBlendLabel, revTypeLabel;

    // Compressor 1 attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp1ThresholdAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp1RatioAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp1AttackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp1ReleaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp1MakeupAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp1BlendAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> comp1BypassAttachment;

    // Compressor 2 attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp2ThresholdAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp2RatioAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp2AttackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp2ReleaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp2MakeupAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp2BlendAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> comp2BypassAttachment;

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

    // Reverb attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> revDecayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> revToneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> revBlendAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> revTypeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> revBypassAttachment;

    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText, juce::Colour colour);
    void setupComboBox(juce::ComboBox& box, juce::Label& label, const juce::String& labelText, juce::Colour colour);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PDLBRDAudioProcessorEditor)
};
