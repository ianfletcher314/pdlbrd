#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class PDLBRDAudioProcessorEditor;

//==============================================================================
// Custom LookAndFeel for Compstortion-style knobs
//==============================================================================
class CompstortionLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CompstortionLookAndFeel() {}

    void setAccentColour(juce::Colour c) { accentColour = c; }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float, float,
                          juce::Slider&) override
    {
        auto bounds = juce::Rectangle<float>((float)x, (float)y, (float)width, (float)height).reduced(2.0f);
        float cx = bounds.getCentreX();
        float cy = bounds.getCentreY();
        float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f - 2.0f;

        // Outer ring (knurled edge)
        g.setColour(juce::Colour(0xff303030));
        g.fillEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

        // Knurl pattern
        g.setColour(juce::Colour(0xff404040));
        int numKnurls = 24;
        for (int i = 0; i < numKnurls; ++i)
        {
            float angle = i * juce::MathConstants<float>::twoPi / numKnurls;
            float x1 = cx + (radius - 1.0f) * std::cos(angle);
            float y1 = cy + (radius - 1.0f) * std::sin(angle);
            float x2 = cx + (radius - 4.0f) * std::cos(angle);
            float y2 = cy + (radius - 4.0f) * std::sin(angle);
            g.drawLine(x1, y1, x2, y2, 1.5f);
        }

        // Main knob body
        float innerRadius = radius * 0.78f;
        juce::ColourGradient knobGradient(juce::Colour(0xff555555), cx - innerRadius * 0.5f, cy - innerRadius * 0.5f,
                                           juce::Colour(0xff252525), cx + innerRadius * 0.5f, cy + innerRadius * 0.5f, true);
        g.setGradientFill(knobGradient);
        g.fillEllipse(cx - innerRadius, cy - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f);

        // Ring
        g.setColour(juce::Colour(0xff606060));
        g.drawEllipse(cx - innerRadius, cy - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f, 1.0f);

        // Indicator line (7 o'clock to 5 o'clock, clockwise)
        float indicatorAngle = juce::jmap(sliderPosProportional, 0.0f, 1.0f, -1.047f, 4.189f) + juce::MathConstants<float>::pi;
        float indicatorLength = innerRadius * 0.65f;
        float ix1 = cx + (innerRadius * 0.2f) * std::cos(indicatorAngle);
        float iy1 = cy + (innerRadius * 0.2f) * std::sin(indicatorAngle);
        float ix2 = cx + indicatorLength * std::cos(indicatorAngle);
        float iy2 = cy + indicatorLength * std::sin(indicatorAngle);
        g.setColour(accentColour);
        g.drawLine(ix1, iy1, ix2, iy2, 3.0f);

        // Center cap
        float capRadius = innerRadius * 0.25f;
        g.setColour(juce::Colour(0xff404040));
        g.fillEllipse(cx - capRadius, cy - capRadius, capRadius * 2.0f, capRadius * 2.0f);
    }

private:
    juce::Colour accentColour = juce::Colours::white;
};

//==============================================================================
class LevelMeter : public juce::Component
{
public:
    void setLevel(float newLevel) { level = newLevel; repaint(); }
    void paint(juce::Graphics& g) override;

private:
    float level = 0.0f;
};

//==============================================================================
class EffectSection : public juce::Component,
                      public juce::DragAndDropTarget
{
public:
    EffectSection(PDLBRDAudioProcessorEditor& editor, int effectId, const juce::String& name, juce::Colour colour);
    ~EffectSection() override { for (auto& s : sliders) s.setLookAndFeel(nullptr); }

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

    juce::Slider& getSlider(int index) { return sliders[index]; }
    juce::Label& getLabel(int index) { return labels[index]; }
    juce::ComboBox& getTypeBox() { return typeBox; }
    juce::Label& getTypeLabel() { return typeLabel; }
    juce::ToggleButton& getBypassButton() { return bypassButton; }

    void setNumSliders(int count) { numSliders = count; }
    void setHasTypeBox(bool has) { hasTypeBox = has; }
    void setIsCompressor(bool isComp) { isCompressor = isComp; }
    void setGainReduction(float gr) { gainReduction = gr; }
    void setupSlider(int index, const juce::String& labelText);
    void setupTypeBox(const juce::String& labelText, const juce::StringArray& items);

private:
    PDLBRDAudioProcessorEditor& parentEditor;
    int effectId;
    juce::String name;
    juce::Colour colour;
    bool isDragOver = false;

    std::unique_ptr<CompstortionLookAndFeel> lookAndFeel;

    static constexpr int MAX_SLIDERS = 6;
    std::array<juce::Slider, MAX_SLIDERS> sliders;
    std::array<juce::Label, MAX_SLIDERS> labels;
    int numSliders = 0;

    juce::ComboBox typeBox;
    juce::Label typeLabel;
    bool hasTypeBox = false;

    bool isCompressor = false;
    float gainReduction = 0.0f;

    juce::ToggleButton bypassButton;
};

//==============================================================================
// Scrollable content component that holds all the effect sections
//==============================================================================
class PedalboardContent : public juce::Component
{
public:
    PedalboardContent() {}
    void paint(juce::Graphics& g) override;
};

//==============================================================================
class PDLBRDAudioProcessorEditor : public juce::AudioProcessorEditor,
                                   public juce::DragAndDropContainer,
                                   public juce::Timer
{
public:
    PDLBRDAudioProcessorEditor(PDLBRDAudioProcessor&);
    ~PDLBRDAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

    void handleReorder(int fromEffectId, int toEffectId);
    PDLBRDAudioProcessor& getProcessor() { return audioProcessor; }

private:
    PDLBRDAudioProcessor& audioProcessor;

    // Scrollable viewport
    juce::Viewport viewport;
    PedalboardContent contentComponent;

    // Level meters
    LevelMeter inputMeter, outputMeter;
    float smoothedInputLevel = 0.0f;
    float smoothedOutputLevel = 0.0f;

    // Preset controls
    juce::ComboBox presetSelector;
    juce::TextButton saveButton { "Save" };
    juce::TextButton loadButton { "Load" };
    void refreshPresetList();
    void savePreset();
    void loadPreset();

    // Effect sections
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
