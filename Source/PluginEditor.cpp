#include "PluginProcessor.h"
#include "PluginEditor.h"

PDLBRDAudioProcessorEditor::PDLBRDAudioProcessorEditor(PDLBRDAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    juce::Colour compColour(0xff4ecdc4);   // Cyan
    juce::Colour distColour(0xffff6b6b);   // Red
    juce::Colour ampColour(0xffffd93d);    // Yellow/Gold
    juce::Colour modColour(0xffbb86fc);    // Purple
    juce::Colour revColour(0xff6bcf7f);    // Green

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
    setupSlider(ampMidFreqSlider, ampMidFreqLabel, "MidFrq", ampColour);
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

    // === MODULATION ===
    setupSlider(modRateSlider, modRateLabel, "Rate", modColour);
    setupSlider(modDepthSlider, modDepthLabel, "Depth", modColour);
    setupSlider(modBlendSlider, modBlendLabel, "Blend", modColour);
    setupComboBox(modTypeBox, modTypeLabel, "Type", modColour);
    modTypeBox.addItem("Phaser", 1);
    modTypeBox.addItem("Flanger", 2);
    modTypeBox.addItem("Chorus", 3);
    modTypeBox.addItem("Tremolo", 4);
    modTypeBox.addItem("Vibrato", 5);
    modBypassButton.setButtonText("Bypass");
    modBypassButton.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    addAndMakeVisible(modBypassButton);

    // === REVERB ===
    setupSlider(revDecaySlider, revDecayLabel, "Decay", revColour);
    setupSlider(revToneSlider, revToneLabel, "Tone", revColour);
    setupSlider(revBlendSlider, revBlendLabel, "Blend", revColour);
    setupComboBox(revTypeBox, revTypeLabel, "Type", revColour);
    revTypeBox.addItem("Spring", 1);
    revTypeBox.addItem("Plate", 2);
    revTypeBox.addItem("Hall", 3);
    revBypassButton.setButtonText("Bypass");
    revBypassButton.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    addAndMakeVisible(revBypassButton);

    // Create attachments
    auto& apvts = audioProcessor.getAPVTS();

    compThresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "compThreshold", compThresholdSlider);
    compRatioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "compRatio", compRatioSlider);
    compAttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "compAttack", compAttackSlider);
    compReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "compRelease", compReleaseSlider);
    compMakeupAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "compMakeup", compMakeupSlider);
    compBlendAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "compBlend", compBlendSlider);
    compBypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "compBypass", compBypassButton);

    distDriveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "distDrive", distDriveSlider);
    distToneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "distTone", distToneSlider);
    distLevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "distLevel", distLevelSlider);
    distTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "distType", distTypeBox);
    distBypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "distBypass", distBypassButton);

    ampGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "ampGain", ampGainSlider);
    ampBassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "ampBass", ampBassSlider);
    ampMidAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "ampMid", ampMidSlider);
    ampMidFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "ampMidFreq", ampMidFreqSlider);
    ampTrebleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "ampTreble", ampTrebleSlider);
    ampMasterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "ampMaster", ampMasterSlider);
    ampTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "ampType", ampTypeBox);
    ampBypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "ampBypass", ampBypassButton);

    modRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "modRate", modRateSlider);
    modDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "modDepth", modDepthSlider);
    modBlendAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "modBlend", modBlendSlider);
    modTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "modType", modTypeBox);
    modBypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "modBypass", modBypassButton);

    revDecayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "revDecay", revDecaySlider);
    revToneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "revTone", revToneSlider);
    revBlendAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "revBlend", revBlendSlider);
    revTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "revType", revTypeBox);
    revBypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "revBypass", revBypassButton);

    setSize(800, 968);
}

PDLBRDAudioProcessorEditor::~PDLBRDAudioProcessorEditor() {}

void PDLBRDAudioProcessorEditor::setupSlider(juce::Slider& slider, juce::Label& label,
                                              const juce::String& labelText, juce::Colour colour)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 14);
    slider.setColour(juce::Slider::rotarySliderFillColourId, colour);
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff2a2a3e));
    slider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(slider);

    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colours::white);
    label.setFont(juce::FontOptions(10.0f));
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
    label.setFont(juce::FontOptions(10.0f));
    addAndMakeVisible(label);
}

void PDLBRDAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a2e));

    // Header
    g.setColour(juce::Colour(0xff16213e));
    g.fillRect(0, 0, getWidth(), 40);
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(20.0f).withStyle("Bold"));
    g.drawText("PDLBRD", 15, 8, 200, 24, juce::Justification::left);

    int sectionHeight = 140;
    int sectionY = 50;

    // Compressor
    g.setColour(juce::Colour(0xff16213e));
    g.fillRoundedRectangle(10.0f, (float)sectionY, getWidth() - 20.0f, (float)sectionHeight, 5.0f);
    g.setColour(juce::Colour(0xff4ecdc4));
    g.setFont(juce::FontOptions(13.0f).withStyle("Bold"));
    g.drawText("COMPRESSOR", 18, sectionY + 5, 120, 16, juce::Justification::left);

    // Distortion
    sectionY += sectionHeight + 8;
    g.setColour(juce::Colour(0xff16213e));
    g.fillRoundedRectangle(10.0f, (float)sectionY, getWidth() - 20.0f, (float)sectionHeight, 5.0f);
    g.setColour(juce::Colour(0xffff6b6b));
    g.drawText("DISTORTION", 18, sectionY + 5, 120, 16, juce::Justification::left);

    // Amp Sim
    sectionY += sectionHeight + 8;
    g.setColour(juce::Colour(0xff16213e));
    g.fillRoundedRectangle(10.0f, (float)sectionY, getWidth() - 20.0f, (float)sectionHeight, 5.0f);
    g.setColour(juce::Colour(0xffffd93d));
    g.drawText("AMP SIM", 18, sectionY + 5, 120, 16, juce::Justification::left);

    // Modulation
    sectionY += sectionHeight + 8;
    g.setColour(juce::Colour(0xff16213e));
    g.fillRoundedRectangle(10.0f, (float)sectionY, getWidth() - 20.0f, (float)sectionHeight, 5.0f);
    g.setColour(juce::Colour(0xffbb86fc));
    g.drawText("MODULATION", 18, sectionY + 5, 120, 16, juce::Justification::left);

    // Reverb
    sectionY += sectionHeight + 8;
    g.setColour(juce::Colour(0xff16213e));
    g.fillRoundedRectangle(10.0f, (float)sectionY, getWidth() - 20.0f, (float)sectionHeight, 5.0f);
    g.setColour(juce::Colour(0xff6bcf7f));
    g.drawText("REVERB", 18, sectionY + 5, 120, 16, juce::Justification::left);
}

void PDLBRDAudioProcessorEditor::resized()
{
    const int knobSize = 55;
    const int labelHeight = 14;
    const int spacing = 68;
    const int sectionHeight = 140;

    auto layoutRow = [&](int y, auto& sliders, auto& labels, auto& typeBox, auto& typeLabel, auto& bypassBtn, int numSliders)
    {
        int x = 18;
        for (int i = 0; i < numSliders; ++i)
        {
            labels[i]->setBounds(x, y, knobSize, labelHeight);
            sliders[i]->setBounds(x, y + labelHeight, knobSize, knobSize);
            x += spacing;
        }
        typeLabel.setBounds(x, y, 75, labelHeight);
        typeBox.setBounds(x, y + labelHeight + 12, 75, 22);
        x += 90;
        bypassBtn.setBounds(x, y + 30, 65, 20);
    };

    // Compressor row
    int y = 75;
    juce::Slider* compSliders[] = {&compThresholdSlider, &compRatioSlider, &compAttackSlider, &compReleaseSlider, &compMakeupSlider, &compBlendSlider};
    juce::Label* compLabels[] = {&compThresholdLabel, &compRatioLabel, &compAttackLabel, &compReleaseLabel, &compMakeupLabel, &compBlendLabel};
    int x = 18;
    for (int i = 0; i < 6; ++i) {
        compLabels[i]->setBounds(x, y, knobSize, labelHeight);
        compSliders[i]->setBounds(x, y + labelHeight, knobSize, knobSize);
        x += spacing;
    }
    compBypassButton.setBounds(x + 5, y + 30, 65, 20);

    // Distortion row
    y += sectionHeight + 8;
    x = 18;
    distDriveLabel.setBounds(x, y, knobSize, labelHeight);
    distDriveSlider.setBounds(x, y + labelHeight, knobSize, knobSize);
    x += spacing;
    distToneLabel.setBounds(x, y, knobSize, labelHeight);
    distToneSlider.setBounds(x, y + labelHeight, knobSize, knobSize);
    x += spacing;
    distLevelLabel.setBounds(x, y, knobSize, labelHeight);
    distLevelSlider.setBounds(x, y + labelHeight, knobSize, knobSize);
    x += spacing;
    distTypeLabel.setBounds(x, y, 80, labelHeight);
    distTypeBox.setBounds(x, y + labelHeight + 12, 80, 22);
    x += 95;
    distBypassButton.setBounds(x, y + 30, 65, 20);

    // Amp Sim row
    y += sectionHeight + 8;
    x = 18;
    juce::Slider* ampSliders[] = {&ampGainSlider, &ampBassSlider, &ampMidSlider, &ampMidFreqSlider, &ampTrebleSlider, &ampMasterSlider};
    juce::Label* ampLabels[] = {&ampGainLabel, &ampBassLabel, &ampMidLabel, &ampMidFreqLabel, &ampTrebleLabel, &ampMasterLabel};
    for (int i = 0; i < 6; ++i) {
        ampLabels[i]->setBounds(x, y, knobSize, labelHeight);
        ampSliders[i]->setBounds(x, y + labelHeight, knobSize, knobSize);
        x += spacing;
    }
    ampTypeLabel.setBounds(x, y, 75, labelHeight);
    ampTypeBox.setBounds(x, y + labelHeight + 12, 75, 22);
    x += 90;
    ampBypassButton.setBounds(x, y + 30, 65, 20);

    // Modulation row
    y += sectionHeight + 8;
    x = 18;
    modRateLabel.setBounds(x, y, knobSize, labelHeight);
    modRateSlider.setBounds(x, y + labelHeight, knobSize, knobSize);
    x += spacing;
    modDepthLabel.setBounds(x, y, knobSize, labelHeight);
    modDepthSlider.setBounds(x, y + labelHeight, knobSize, knobSize);
    x += spacing;
    modBlendLabel.setBounds(x, y, knobSize, labelHeight);
    modBlendSlider.setBounds(x, y + labelHeight, knobSize, knobSize);
    x += spacing;
    modTypeLabel.setBounds(x, y, 80, labelHeight);
    modTypeBox.setBounds(x, y + labelHeight + 12, 80, 22);
    x += 95;
    modBypassButton.setBounds(x, y + 30, 65, 20);

    // Reverb row
    y += sectionHeight + 8;
    x = 18;
    revDecayLabel.setBounds(x, y, knobSize, labelHeight);
    revDecaySlider.setBounds(x, y + labelHeight, knobSize, knobSize);
    x += spacing;
    revToneLabel.setBounds(x, y, knobSize, labelHeight);
    revToneSlider.setBounds(x, y + labelHeight, knobSize, knobSize);
    x += spacing;
    revBlendLabel.setBounds(x, y, knobSize, labelHeight);
    revBlendSlider.setBounds(x, y + labelHeight, knobSize, knobSize);
    x += spacing;
    revTypeLabel.setBounds(x, y, 80, labelHeight);
    revTypeBox.setBounds(x, y + labelHeight + 12, 80, 22);
    x += 95;
    revBypassButton.setBounds(x, y + 30, 65, 20);
}
