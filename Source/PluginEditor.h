#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class PDLBRDAudioProcessorEditor;

//==============================================================================
class EffectSection : public juce::Component,
                      public juce::DragAndDropTarget
{
public:
    EffectSection(PDLBRDAudioProcessorEditor& editor, int effectId, const juce::String& name, juce::Colour colour);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;

    bool isInterestedInDragSource(const SourceDetails& details) override;
    void itemDragEnter(const SourceDetails& details) override;
    void itemDragExit(const SourceDetails& details) override;
    void itemDropped(const SourceDetails& details) override;

    int getEffectId() const { return effectId; }
    juce::Colour getColour() const { return colour; }

    // Access to controls for attachment
    juce::Slider& getSlider(int index) { return sliders[index]; }
    juce::Label& getLabel(int index) { return labels[index]; }
    juce::ComboBox& getTypeBox() { return typeBox; }
    juce::Label& getTypeLabel() { return typeLabel; }
    juce::ToggleButton& getBypassButton() { return bypassButton; }

    void setNumSliders(int count) { numSliders = count; }
    void setHasTypeBox(bool has) { hasTypeBox = has; }
    void setupSlider(int index, const juce::String& labelText);
    void setupTypeBox(const juce::String& labelText, const juce::StringArray& items);

private:
    PDLBRDAudioProcessorEditor& parentEditor;
    int effectId;
    juce::String name;
    juce::Colour colour;
    bool isDragOver = false;

    static constexpr int MAX_SLIDERS = 6;
    std::array<juce::Slider, MAX_SLIDERS> sliders;
    std::array<juce::Label, MAX_SLIDERS> labels;
    int numSliders = 0;

    juce::ComboBox typeBox;
    juce::Label typeLabel;
    bool hasTypeBox = false;

    juce::ToggleButton bypassButton;
};

//==============================================================================
class PDLBRDAudioProcessorEditor : public juce::AudioProcessorEditor,
                                   public juce::DragAndDropContainer
{
public:
    PDLBRDAudioProcessorEditor(PDLBRDAudioProcessor&);
    ~PDLBRDAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void handleReorder(int fromEffectId, int toEffectId);
    PDLBRDAudioProcessor& getProcessor() { return audioProcessor; }

private:
    PDLBRDAudioProcessor& audioProcessor;

    // Effect sections - one for each effect
    std::array<std::unique_ptr<EffectSection>, PDLBRDAudioProcessor::NUM_EFFECTS> sections;

    void createSections();
    void layoutSections();

    // Compressor 1 attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp1ThresholdAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp1RatioAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp1AttackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp1ReleaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp1MakeupAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp1BlendAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> comp1BypassAttachment;

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

    // Compressor 2 attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp2ThresholdAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp2RatioAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp2AttackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp2ReleaseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp2MakeupAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> comp2BlendAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> comp2BypassAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PDLBRDAudioProcessorEditor)
};
