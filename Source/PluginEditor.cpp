#include "PluginProcessor.h"
#include "PluginEditor.h"

PDLBRDAudioProcessorEditor::PDLBRDAudioProcessorEditor (PDLBRDAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    juce::Colour compColour(0xff4ecdc4);  // Cyan
    juce::Colour distColour(0xffff6b6b);  // Red
    juce::Colour ampColour(0xffffd93d);   // Yellow/Gold

    // === COMPRESSOR ===
    setupSlider(compThresholdSlider, compThresholdLabel, "Thresh", compColour);
    setupSlider(compRatioSlider, compRatioLabel, "Ratio", compColour);
    setupSlider(compAttackSlider, compAttackLabel, "Attack", compColour);
    setupSlider(compReleaseSlider, compReleaseLabel, "Release", compColour);
    setupSlider(compMakeupSlider, compMakeupLabel, "Makeup", compColour);
    setupSlider(compBlendSlider, compBlendLabel, "Blend", compColour);
    compBypassButton.setButtonText("Bypass");
    compBypassButton.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    addAndMakeVisible(compBypassButton);

    // === DISTORTION ===
    setupSlider(distDriveSlider, distDriveLabel, "Drive", distColour);
    setupSlider(distToneSlider, distToneLabel, "Tone", distColour);
    setupSlider(distLevelSlider, distLevelLabel, "Level", distColour);
    setupComboBox(distTypeBox, distTypeLabel, "Type", distColour);
    distTypeBox.addItem("TS9", 1);
    distTypeBox.addItem("RAT", 2);
    distTypeBox.addItem("Blues", 3);
    distTypeBox.addItem("Fuzz", 4);
    distTypeBox.addItem("Muff", 5);
    distBypassButton.setButtonText("Bypass");
    distBypassButton.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    addAndMakeVisible(distBypassButton);

    // === AMP SIM ===
    setupSlider(ampGainSlider, ampGainLabel, "Gain", ampColour);
    setupSlider(ampBassSlider, ampBassLabel, "Bass", ampColour);
    setupSlider(ampMidSlider, ampMidLabel, "Mid", ampColour);
    setupSlider(ampMidFreqSlider, ampMidFreqLabel, "MidFreq", ampColour);
    setupSlider(ampTrebleSlider, ampTrebleLabel, "Treble", ampColour);
    setupSlider(ampMasterSlider, ampMasterLabel, "Master", ampColour);
    setupComboBox(ampTypeBox, ampTypeLabel, "Type", ampColour);
    ampTypeBox.addItem("Clean", 1);
    ampTypeBox.addItem("Crunch", 2);
    ampTypeBox.addItem("Lead", 3);
    ampTypeBox.addItem("Modern", 4);
    ampTypeBox.addItem("Vintage", 5);
    ampBypassButton.setButtonText("Bypass");
    ampBypassButton.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    addAndMakeVisible(ampBypassButton);

    // Create attachments
    auto& apvts = audioProcessor.getAPVTS();

    // Compressor
    compThresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "compThreshold", compThresholdSlider);
    compRatioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "compRatio", compRatioSlider);
    compAttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "compAttack", compAttackSlider);
    compReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "compRelease", compReleaseSlider);
    compMakeupAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "compMakeup", compMakeupSlider);
    compBlendAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "compBlend", compBlendSlider);
    compBypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "compBypass", compBypassButton);

    // Distortion
    distDriveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "distDrive", distDriveSlider);
    distToneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "distTone", distToneSlider);
    distLevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "distLevel", distLevelSlider);
    distTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "distType", distTypeBox);
    distBypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "distBypass", distBypassButton);

    // Amp Sim
    ampGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "ampGain", ampGainSlider);
    ampBassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "ampBass", ampBassSlider);
    ampMidAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "ampMid", ampMidSlider);
    ampMidFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "ampMidFreq", ampMidFreqSlider);
    ampTrebleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "ampTreble", ampTrebleSlider);
    ampMasterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "ampMaster", ampMasterSlider);
    ampTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "ampType", ampTypeBox);
    ampBypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "ampBypass", ampBypassButton);

    setSize(800, 680);
}

PDLBRDAudioProcessorEditor::~PDLBRDAudioProcessorEditor() {}

void PDLBRDAudioProcessorEditor::setupSlider(juce::Slider& slider, juce::Label& label,
                                              const juce::String& labelText, juce::Colour colour)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 55, 16);
    slider.setColour(juce::Slider::rotarySliderFillColourId, colour);
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff2a2a3e));
    slider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(slider);

    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colours::white);
    label.setFont(juce::FontOptions(11.0f));
    addAndMakeVisible(label);
}

void PDLBRDAudioProcessorEditor::setupComboBox(juce::ComboBox& box, juce::Label& label,
                                                const juce::String& labelText, juce::Colour colour)
{
    box.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff2a2a3e));
    box.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    box.setColour(juce::ComboBox::outlineColourId, colour);
    addAndMakeVisible(box);

    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colours::white);
    label.setFont(juce::FontOptions(11.0f));
    addAndMakeVisible(label);
}

void PDLBRDAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a2e));

    // Header
    g.setColour(juce::Colour(0xff16213e));
    g.fillRect(0, 0, getWidth(), 45);
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(22.0f).withStyle("Bold"));
    g.drawText("PDLBRD", 15, 10, 200, 25, juce::Justification::left);

    // Section backgrounds and titles
    int sectionHeight = 155;

    // Compressor
    g.setColour(juce::Colour(0xff16213e));
    g.fillRoundedRectangle(10.0f, 55.0f, getWidth() - 20.0f, (float)sectionHeight, 6.0f);
    g.setColour(juce::Colour(0xff4ecdc4));
    g.setFont(juce::FontOptions(14.0f).withStyle("Bold"));
    g.drawText("COMPRESSOR", 20, 60, 120, 18, juce::Justification::left);

    // Distortion
    g.setColour(juce::Colour(0xff16213e));
    g.fillRoundedRectangle(10.0f, 55.0f + sectionHeight + 10.0f, getWidth() - 20.0f, (float)sectionHeight, 6.0f);
    g.setColour(juce::Colour(0xffff6b6b));
    g.drawText("DISTORTION", 20, 60 + sectionHeight + 10, 120, 18, juce::Justification::left);

    // Amp Sim
    g.setColour(juce::Colour(0xff16213e));
    g.fillRoundedRectangle(10.0f, 55.0f + (sectionHeight + 10.0f) * 2, getWidth() - 20.0f, (float)sectionHeight, 6.0f);
    g.setColour(juce::Colour(0xffffd93d));
    g.drawText("AMP SIM", 20, 60 + (sectionHeight + 10) * 2, 120, 18, juce::Justification::left);
}

void PDLBRDAudioProcessorEditor::resized()
{
    const int knobSize = 60;
    const int labelHeight = 16;
    const int spacing = 75;
    const int sectionHeight = 155;

    // Compressor row
    int y = 85;
    int x = 20;
    compThresholdLabel.setBounds(x, y, knobSize, labelHeight);
    compThresholdSlider.setBounds(x, y + labelHeight, knobSize, knobSize);
    x += spacing;
    compRatioLabel.setBounds(x, y, knobSize, labelHeight);
    compRatioSlider.setBounds(x, y + labelHeight, knobSize, knobSize);
    x += spacing;
    compAttackLabel.setBounds(x, y, knobSize, labelHeight);
    compAttackSlider.setBounds(x, y + labelHeight, knobSize, knobSize);
    x += spacing;
    compReleaseLabel.setBounds(x, y, knobSize, labelHeight);
    compReleaseSlider.setBounds(x, y + labelHeight, knobSize, knobSize);
    x += spacing;
    compMakeupLabel.setBounds(x, y, knobSize, labelHeight);
    compMakeupSlider.setBounds(x, y + labelHeight, knobSize, knobSize);
    x += spacing;
    compBlendLabel.setBounds(x, y, knobSize, labelHeight);
    compBlendSlider.setBounds(x, y + labelHeight, knobSize, knobSize);
    x += spacing;
    compBypassButton.setBounds(x + 5, y + 35, 70, 22);

    // Distortion row
    y = 85 + sectionHeight + 10;
    x = 20;
    distDriveLabel.setBounds(x, y, knobSize, labelHeight);
    distDriveSlider.setBounds(x, y + labelHeight, knobSize, knobSize);
    x += spacing;
    distToneLabel.setBounds(x, y, knobSize, labelHeight);
    distToneSlider.setBounds(x, y + labelHeight, knobSize, knobSize);
    x += spacing;
    distLevelLabel.setBounds(x, y, knobSize, labelHeight);
    distLevelSlider.setBounds(x, y + labelHeight, knobSize, knobSize);
    x += spacing;
    distTypeLabel.setBounds(x, y, 90, labelHeight);
    distTypeBox.setBounds(x, y + labelHeight + 15, 90, 24);
    x += 110;
    distBypassButton.setBounds(x, y + 35, 70, 22);

    // Amp Sim row
    y = 85 + (sectionHeight + 10) * 2;
    x = 20;
    ampGainLabel.setBounds(x, y, knobSize, labelHeight);
    ampGainSlider.setBounds(x, y + labelHeight, knobSize, knobSize);
    x += spacing;
    ampBassLabel.setBounds(x, y, knobSize, labelHeight);
    ampBassSlider.setBounds(x, y + labelHeight, knobSize, knobSize);
    x += spacing;
    ampMidLabel.setBounds(x, y, knobSize, labelHeight);
    ampMidSlider.setBounds(x, y + labelHeight, knobSize, knobSize);
    x += spacing;
    ampMidFreqLabel.setBounds(x, y, knobSize, labelHeight);
    ampMidFreqSlider.setBounds(x, y + labelHeight, knobSize, knobSize);
    x += spacing;
    ampTrebleLabel.setBounds(x, y, knobSize, labelHeight);
    ampTrebleSlider.setBounds(x, y + labelHeight, knobSize, knobSize);
    x += spacing;
    ampMasterLabel.setBounds(x, y, knobSize, labelHeight);
    ampMasterSlider.setBounds(x, y + labelHeight, knobSize, knobSize);
    x += spacing;
    ampTypeLabel.setBounds(x, y, 80, labelHeight);
    ampTypeBox.setBounds(x, y + labelHeight + 15, 80, 24);
    x += 95;
    ampBypassButton.setBounds(x, y + 35, 70, 22);
}
