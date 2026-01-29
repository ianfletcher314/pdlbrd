#include "PluginProcessor.h"
#include "PluginEditor.h"

PDLBRDAudioProcessorEditor::PDLBRDAudioProcessorEditor (PDLBRDAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Setup all sliders
    setupSlider(thresholdSlider, thresholdLabel, "Threshold");
    setupSlider(ratioSlider, ratioLabel, "Ratio");
    setupSlider(attackSlider, attackLabel, "Attack");
    setupSlider(releaseSlider, releaseLabel, "Release");
    setupSlider(makeupSlider, makeupLabel, "Makeup");
    setupSlider(blendSlider, blendLabel, "Blend");

    // Bypass button
    bypassButton.setButtonText("Bypass");
    bypassButton.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    bypassButton.setColour(juce::ToggleButton::tickColourId, juce::Colours::red);
    addAndMakeVisible(bypassButton);

    // Create attachments
    auto& apvts = audioProcessor.getAPVTS();
    thresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "compThreshold", thresholdSlider);
    ratioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "compRatio", ratioSlider);
    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "compAttack", attackSlider);
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "compRelease", releaseSlider);
    makeupAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "compMakeup", makeupSlider);
    blendAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "compBlend", blendSlider);
    bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, "compBypass", bypassButton);

    setSize (700, 400);
}

PDLBRDAudioProcessorEditor::~PDLBRDAudioProcessorEditor()
{
}

void PDLBRDAudioProcessorEditor::setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff4ecdc4));
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff2a2a3e));
    slider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(slider);

    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(label);
}

void PDLBRDAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Dark background
    g.fillAll(juce::Colour(0xff1a1a2e));

    // Header
    g.setColour(juce::Colour(0xff16213e));
    g.fillRect(0, 0, getWidth(), 60);

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(28.0f).withStyle("Bold"));
    g.drawText("PDLBRD", 20, 15, 200, 30, juce::Justification::left);

    g.setFont(juce::FontOptions(14.0f));
    g.setColour(juce::Colours::grey);
    g.drawText("Compressor", 20, 40, 200, 20, juce::Justification::left);

    // Compressor section background
    g.setColour(juce::Colour(0xff16213e));
    g.fillRoundedRectangle(15.0f, 70.0f, getWidth() - 30.0f, getHeight() - 85.0f, 10.0f);
}

void PDLBRDAudioProcessorEditor::resized()
{
    const int knobWidth = 90;
    const int knobHeight = 90;
    const int labelHeight = 20;
    const int startY = 100;
    const int spacing = 100;

    // Layout knobs in a row
    int x = 30;

    thresholdLabel.setBounds(x, startY, knobWidth, labelHeight);
    thresholdSlider.setBounds(x, startY + labelHeight, knobWidth, knobHeight);
    x += spacing;

    ratioLabel.setBounds(x, startY, knobWidth, labelHeight);
    ratioSlider.setBounds(x, startY + labelHeight, knobWidth, knobHeight);
    x += spacing;

    attackLabel.setBounds(x, startY, knobWidth, labelHeight);
    attackSlider.setBounds(x, startY + labelHeight, knobWidth, knobHeight);
    x += spacing;

    releaseLabel.setBounds(x, startY, knobWidth, labelHeight);
    releaseSlider.setBounds(x, startY + labelHeight, knobWidth, knobHeight);
    x += spacing;

    makeupLabel.setBounds(x, startY, knobWidth, labelHeight);
    makeupSlider.setBounds(x, startY + labelHeight, knobWidth, knobHeight);
    x += spacing;

    blendLabel.setBounds(x, startY, knobWidth, labelHeight);
    blendSlider.setBounds(x, startY + labelHeight, knobWidth, knobHeight);

    // Bypass button at the bottom
    bypassButton.setBounds(30, startY + labelHeight + knobHeight + 40, 100, 30);
}
