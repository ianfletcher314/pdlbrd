#include "PluginProcessor.h"
#include "PluginEditor.h"

PDLBRDAudioProcessorEditor::PDLBRDAudioProcessorEditor (PDLBRDAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Compressor sliders (cyan color)
    juce::Colour compColour(0xff4ecdc4);
    setupSlider(compThresholdSlider, compThresholdLabel, "Threshold", compColour);
    setupSlider(compRatioSlider, compRatioLabel, "Ratio", compColour);
    setupSlider(compAttackSlider, compAttackLabel, "Attack", compColour);
    setupSlider(compReleaseSlider, compReleaseLabel, "Release", compColour);
    setupSlider(compMakeupSlider, compMakeupLabel, "Makeup", compColour);
    setupSlider(compBlendSlider, compBlendLabel, "Blend", compColour);

    compBypassButton.setButtonText("Bypass");
    compBypassButton.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    addAndMakeVisible(compBypassButton);

    // Distortion sliders (orange color)
    juce::Colour distColour(0xffff6b6b);
    setupSlider(distDriveSlider, distDriveLabel, "Drive", distColour);
    setupSlider(distToneSlider, distToneLabel, "Tone", distColour);
    setupSlider(distLevelSlider, distLevelLabel, "Level", distColour);
    setupComboBox(distTypeBox, distTypeLabel, "Type");

    distBypassButton.setButtonText("Bypass");
    distBypassButton.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    addAndMakeVisible(distBypassButton);

    // Create attachments
    auto& apvts = audioProcessor.getAPVTS();

    // Compressor attachments
    compThresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "compThreshold", compThresholdSlider);
    compRatioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "compRatio", compRatioSlider);
    compAttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "compAttack", compAttackSlider);
    compReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "compRelease", compReleaseSlider);
    compMakeupAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "compMakeup", compMakeupSlider);
    compBlendAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "compBlend", compBlendSlider);
    compBypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, "compBypass", compBypassButton);

    // Distortion attachments
    distDriveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "distDrive", distDriveSlider);
    distToneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "distTone", distToneSlider);
    distLevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "distLevel", distLevelSlider);
    distTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts, "distType", distTypeBox);
    distBypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, "distBypass", distBypassButton);

    setSize (750, 500);
}

PDLBRDAudioProcessorEditor::~PDLBRDAudioProcessorEditor()
{
}

void PDLBRDAudioProcessorEditor::setupSlider(juce::Slider& slider, juce::Label& label,
                                              const juce::String& labelText, juce::Colour colour)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    slider.setColour(juce::Slider::rotarySliderFillColourId, colour);
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff2a2a3e));
    slider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(slider);

    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colours::white);
    label.setFont(juce::FontOptions(12.0f));
    addAndMakeVisible(label);
}

void PDLBRDAudioProcessorEditor::setupComboBox(juce::ComboBox& box, juce::Label& label, const juce::String& labelText)
{
    box.addItem("TS9", 1);
    box.addItem("RAT", 2);
    box.addItem("Blues", 3);
    box.addItem("Fuzz", 4);
    box.addItem("Muff", 5);
    box.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff2a2a3e));
    box.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    box.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xffff6b6b));
    addAndMakeVisible(box);

    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colours::white);
    label.setFont(juce::FontOptions(12.0f));
    addAndMakeVisible(label);
}

void PDLBRDAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Dark background
    g.fillAll(juce::Colour(0xff1a1a2e));

    // Header
    g.setColour(juce::Colour(0xff16213e));
    g.fillRect(0, 0, getWidth(), 50);

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(24.0f).withStyle("Bold"));
    g.drawText("PDLBRD", 20, 12, 200, 26, juce::Justification::left);

    // Compressor section
    g.setColour(juce::Colour(0xff16213e));
    g.fillRoundedRectangle(15.0f, 60.0f, getWidth() - 30.0f, 180.0f, 8.0f);

    g.setColour(juce::Colour(0xff4ecdc4));
    g.setFont(juce::FontOptions(16.0f).withStyle("Bold"));
    g.drawText("COMPRESSOR", 25, 65, 150, 20, juce::Justification::left);

    // Distortion section
    g.setColour(juce::Colour(0xff16213e));
    g.fillRoundedRectangle(15.0f, 255.0f, getWidth() - 30.0f, 180.0f, 8.0f);

    g.setColour(juce::Colour(0xffff6b6b));
    g.setFont(juce::FontOptions(16.0f).withStyle("Bold"));
    g.drawText("DISTORTION", 25, 260, 150, 20, juce::Justification::left);
}

void PDLBRDAudioProcessorEditor::resized()
{
    const int knobSize = 70;
    const int labelHeight = 18;
    const int spacing = 85;

    // Compressor row
    int compY = 95;
    int x = 30;

    compThresholdLabel.setBounds(x, compY, knobSize, labelHeight);
    compThresholdSlider.setBounds(x, compY + labelHeight, knobSize, knobSize);
    x += spacing;

    compRatioLabel.setBounds(x, compY, knobSize, labelHeight);
    compRatioSlider.setBounds(x, compY + labelHeight, knobSize, knobSize);
    x += spacing;

    compAttackLabel.setBounds(x, compY, knobSize, labelHeight);
    compAttackSlider.setBounds(x, compY + labelHeight, knobSize, knobSize);
    x += spacing;

    compReleaseLabel.setBounds(x, compY, knobSize, labelHeight);
    compReleaseSlider.setBounds(x, compY + labelHeight, knobSize, knobSize);
    x += spacing;

    compMakeupLabel.setBounds(x, compY, knobSize, labelHeight);
    compMakeupSlider.setBounds(x, compY + labelHeight, knobSize, knobSize);
    x += spacing;

    compBlendLabel.setBounds(x, compY, knobSize, labelHeight);
    compBlendSlider.setBounds(x, compY + labelHeight, knobSize, knobSize);
    x += spacing;

    compBypassButton.setBounds(x + 10, compY + 40, 80, 25);

    // Distortion row
    int distY = 290;
    x = 30;

    distDriveLabel.setBounds(x, distY, knobSize, labelHeight);
    distDriveSlider.setBounds(x, distY + labelHeight, knobSize, knobSize);
    x += spacing;

    distToneLabel.setBounds(x, distY, knobSize, labelHeight);
    distToneSlider.setBounds(x, distY + labelHeight, knobSize, knobSize);
    x += spacing;

    distLevelLabel.setBounds(x, distY, knobSize, labelHeight);
    distLevelSlider.setBounds(x, distY + labelHeight, knobSize, knobSize);
    x += spacing;

    distTypeLabel.setBounds(x, distY, 100, labelHeight);
    distTypeBox.setBounds(x, distY + labelHeight + 20, 100, 28);
    x += 120;

    distBypassButton.setBounds(x + 10, distY + 40, 80, 25);
}
