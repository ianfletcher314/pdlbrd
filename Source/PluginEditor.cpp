#include "PluginProcessor.h"
#include "PluginEditor.h"

PDLBRDAudioProcessorEditor::PDLBRDAudioProcessorEditor(PDLBRDAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    juce::Colour comp1Colour(0xff4ecdc4);   // Cyan
    juce::Colour distColour(0xffff6b6b);    // Red
    juce::Colour ampColour(0xffffd93d);     // Yellow/Gold
    juce::Colour modColour(0xffbb86fc);     // Purple
    juce::Colour revColour(0xff6bcf7f);     // Green
    juce::Colour comp2Colour(0xff45b7d1);   // Light blue

    // === SIGNAL CHAIN ===
    const std::array<juce::Colour, 6> chainColours = { comp1Colour, distColour, ampColour, modColour, revColour, comp2Colour };
    for (int i = 0; i < PDLBRDAudioProcessor::NUM_EFFECTS; ++i)
    {
        chainButtons[i].setColour(juce::TextButton::buttonColourId, juce::Colour(0xff2a2a3e));
        chainButtons[i].setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        addAndMakeVisible(chainButtons[i]);

        upButtons[i].setButtonText(juce::CharPointer_UTF8("\xe2\x96\xb2")); // Up arrow
        upButtons[i].setColour(juce::TextButton::buttonColourId, juce::Colour(0xff3a3a4e));
        upButtons[i].onClick = [this, i]() {
            auto order = audioProcessor.getEffectOrder();
            for (int j = 1; j < PDLBRDAudioProcessor::NUM_EFFECTS; ++j) {
                if (order[j] == i) {
                    std::swap(order[j], order[j-1]);
                    audioProcessor.setEffectOrder(order);
                    updateChainDisplay();
                    return;
                }
            }
        };
        addAndMakeVisible(upButtons[i]);

        downButtons[i].setButtonText(juce::CharPointer_UTF8("\xe2\x96\xbc")); // Down arrow
        downButtons[i].setColour(juce::TextButton::buttonColourId, juce::Colour(0xff3a3a4e));
        downButtons[i].onClick = [this, i]() {
            auto order = audioProcessor.getEffectOrder();
            for (int j = 0; j < PDLBRDAudioProcessor::NUM_EFFECTS - 1; ++j) {
                if (order[j] == i) {
                    std::swap(order[j], order[j+1]);
                    audioProcessor.setEffectOrder(order);
                    updateChainDisplay();
                    return;
                }
            }
        };
        addAndMakeVisible(downButtons[i]);
    }
    updateChainDisplay();

    // === COMPRESSOR 1 ===
    setupSlider(comp1ThresholdSlider, comp1ThresholdLabel, "Thresh", comp1Colour);
    setupSlider(comp1RatioSlider, comp1RatioLabel, "Ratio", comp1Colour);
    setupSlider(comp1AttackSlider, comp1AttackLabel, "Attack", comp1Colour);
    setupSlider(comp1ReleaseSlider, comp1ReleaseLabel, "Release", comp1Colour);
    setupSlider(comp1MakeupSlider, comp1MakeupLabel, "Makeup", comp1Colour);
    setupSlider(comp1BlendSlider, comp1BlendLabel, "Blend", comp1Colour);
    comp1BypassButton.setButtonText("Bypass");
    comp1BypassButton.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    addAndMakeVisible(comp1BypassButton);

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

    // === COMPRESSOR 2 ===
    setupSlider(comp2ThresholdSlider, comp2ThresholdLabel, "Thresh", comp2Colour);
    setupSlider(comp2RatioSlider, comp2RatioLabel, "Ratio", comp2Colour);
    setupSlider(comp2AttackSlider, comp2AttackLabel, "Attack", comp2Colour);
    setupSlider(comp2ReleaseSlider, comp2ReleaseLabel, "Release", comp2Colour);
    setupSlider(comp2MakeupSlider, comp2MakeupLabel, "Makeup", comp2Colour);
    setupSlider(comp2BlendSlider, comp2BlendLabel, "Blend", comp2Colour);
    comp2BypassButton.setButtonText("Bypass");
    comp2BypassButton.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    addAndMakeVisible(comp2BypassButton);

    // Create attachments
    auto& apvts = audioProcessor.getAPVTS();

    comp1ThresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "comp1Threshold", comp1ThresholdSlider);
    comp1RatioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "comp1Ratio", comp1RatioSlider);
    comp1AttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "comp1Attack", comp1AttackSlider);
    comp1ReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "comp1Release", comp1ReleaseSlider);
    comp1MakeupAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "comp1Makeup", comp1MakeupSlider);
    comp1BlendAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "comp1Blend", comp1BlendSlider);
    comp1BypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "comp1Bypass", comp1BypassButton);

    comp2ThresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "comp2Threshold", comp2ThresholdSlider);
    comp2RatioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "comp2Ratio", comp2RatioSlider);
    comp2AttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "comp2Attack", comp2AttackSlider);
    comp2ReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "comp2Release", comp2ReleaseSlider);
    comp2MakeupAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "comp2Makeup", comp2MakeupSlider);
    comp2BlendAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "comp2Blend", comp2BlendSlider);
    comp2BypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "comp2Bypass", comp2BypassButton);

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

    setSize(800, 1120);
    startTimerHz(10);
}

PDLBRDAudioProcessorEditor::~PDLBRDAudioProcessorEditor()
{
    stopTimer();
}

void PDLBRDAudioProcessorEditor::timerCallback()
{
    updateChainDisplay();
}

void PDLBRDAudioProcessorEditor::updateChainDisplay()
{
    auto order = audioProcessor.getEffectOrder();
    const std::array<juce::Colour, 6> chainColours = {
        juce::Colour(0xff4ecdc4),  // Comp1 - Cyan
        juce::Colour(0xffff6b6b),  // Dist - Red
        juce::Colour(0xffffd93d),  // Amp - Yellow
        juce::Colour(0xffbb86fc),  // Mod - Purple
        juce::Colour(0xff6bcf7f),  // Rev - Green
        juce::Colour(0xff45b7d1)   // Comp2 - Light blue
    };

    for (int pos = 0; pos < PDLBRDAudioProcessor::NUM_EFFECTS; ++pos)
    {
        int effectId = order[pos];
        chainButtons[pos].setButtonText(audioProcessor.effectNames[effectId]);
        chainButtons[pos].setColour(juce::TextButton::buttonColourId, chainColours[effectId].darker(0.3f));
    }
}

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

    // Signal chain section
    g.setColour(juce::Colour(0xff16213e));
    g.fillRoundedRectangle(10.0f, 50.0f, getWidth() - 20.0f, 50.0f, 5.0f);
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(11.0f));
    g.drawText("SIGNAL CHAIN (click arrows to reorder)", 18, 52, 300, 16, juce::Justification::left);

    int sectionHeight = 140;
    int sectionY = 110;

    // Compressor 1
    g.setColour(juce::Colour(0xff16213e));
    g.fillRoundedRectangle(10.0f, (float)sectionY, getWidth() - 20.0f, (float)sectionHeight, 5.0f);
    g.setColour(juce::Colour(0xff4ecdc4));
    g.setFont(juce::FontOptions(13.0f).withStyle("Bold"));
    g.drawText("COMPRESSOR 1", 18, sectionY + 5, 140, 16, juce::Justification::left);

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

    // Compressor 2
    sectionY += sectionHeight + 8;
    g.setColour(juce::Colour(0xff16213e));
    g.fillRoundedRectangle(10.0f, (float)sectionY, getWidth() - 20.0f, (float)sectionHeight, 5.0f);
    g.setColour(juce::Colour(0xff45b7d1));
    g.drawText("COMPRESSOR 2", 18, sectionY + 5, 140, 16, juce::Justification::left);
}

void PDLBRDAudioProcessorEditor::resized()
{
    const int knobSize = 55;
    const int labelHeight = 14;
    const int spacing = 68;
    const int sectionHeight = 140;

    // Signal chain buttons
    int chainX = 18;
    int chainY = 70;
    int btnWidth = 70;
    int btnHeight = 22;
    int arrowSize = 18;

    for (int i = 0; i < PDLBRDAudioProcessor::NUM_EFFECTS; ++i)
    {
        upButtons[i].setBounds(chainX, chainY - 2, arrowSize, arrowSize);
        chainButtons[i].setBounds(chainX + arrowSize + 2, chainY, btnWidth, btnHeight);
        downButtons[i].setBounds(chainX + arrowSize + btnWidth + 4, chainY - 2, arrowSize, arrowSize);
        chainX += arrowSize + btnWidth + arrowSize + 18;
    }

    // Compressor 1 row
    int y = 135;
    int x = 18;
    juce::Slider* comp1Sliders[] = {&comp1ThresholdSlider, &comp1RatioSlider, &comp1AttackSlider, &comp1ReleaseSlider, &comp1MakeupSlider, &comp1BlendSlider};
    juce::Label* comp1Labels[] = {&comp1ThresholdLabel, &comp1RatioLabel, &comp1AttackLabel, &comp1ReleaseLabel, &comp1MakeupLabel, &comp1BlendLabel};
    for (int i = 0; i < 6; ++i) {
        comp1Labels[i]->setBounds(x, y, knobSize, labelHeight);
        comp1Sliders[i]->setBounds(x, y + labelHeight, knobSize, knobSize);
        x += spacing;
    }
    comp1BypassButton.setBounds(x + 5, y + 30, 65, 20);

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

    // Compressor 2 row
    y += sectionHeight + 8;
    x = 18;
    juce::Slider* comp2Sliders[] = {&comp2ThresholdSlider, &comp2RatioSlider, &comp2AttackSlider, &comp2ReleaseSlider, &comp2MakeupSlider, &comp2BlendSlider};
    juce::Label* comp2Labels[] = {&comp2ThresholdLabel, &comp2RatioLabel, &comp2AttackLabel, &comp2ReleaseLabel, &comp2MakeupLabel, &comp2BlendLabel};
    for (int i = 0; i < 6; ++i) {
        comp2Labels[i]->setBounds(x, y, knobSize, labelHeight);
        comp2Sliders[i]->setBounds(x, y + labelHeight, knobSize, knobSize);
        x += spacing;
    }
    comp2BypassButton.setBounds(x + 5, y + 30, 65, 20);
}
