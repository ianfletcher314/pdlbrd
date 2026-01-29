#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// LevelMeter implementation
//==============================================================================
void LevelMeter::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Background
    g.setColour(juce::Colour(0xff0a0a15));
    g.fillRoundedRectangle(bounds, 2.0f);

    // Calculate level height (convert to dB, then normalize)
    float db = juce::Decibels::gainToDecibels(level, -60.0f);
    float normalized = juce::jmap(db, -60.0f, 0.0f, 0.0f, 1.0f);
    normalized = juce::jlimit(0.0f, 1.0f, normalized);

    // Level bar
    float barHeight = bounds.getHeight() * normalized;
    auto barBounds = bounds.removeFromBottom(barHeight);

    // Gradient from green to yellow to red
    juce::Colour barColour;
    if (normalized < 0.6f)
        barColour = juce::Colour(0xff4ecdc4);  // Cyan/green
    else if (normalized < 0.85f)
        barColour = juce::Colour(0xffffd93d);  // Yellow
    else
        barColour = juce::Colour(0xffff6b6b);  // Red

    g.setColour(barColour);
    g.fillRoundedRectangle(barBounds.reduced(1), 1.0f);

    // Outline
    g.setColour(juce::Colour(0xff3a3a4e));
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 2.0f, 1.0f);
}

//==============================================================================
// EffectSection implementation
//==============================================================================
EffectSection::EffectSection(PDLBRDAudioProcessorEditor& editor, int id, const juce::String& n, juce::Colour c)
    : parentEditor(editor), effectId(id), name(n), colour(c)
{
    bypassButton.setButtonText("ON");
    bypassButton.setClickingTogglesState(true);
    bypassButton.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    bypassButton.setColour(juce::ToggleButton::tickColourId, colour);
    addAndMakeVisible(bypassButton);
}

void EffectSection::setupSlider(int index, const juce::String& labelText)
{
    if (index >= MAX_SLIDERS) return;

    auto& slider = sliders[index];
    auto& label = labels[index];

    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 14);
    slider.setColour(juce::Slider::rotarySliderFillColourId, colour);
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff1a1a2e));
    slider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white.withAlpha(0.8f));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(slider);

    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.7f));
    label.setFont(juce::FontOptions(10.0f));
    addAndMakeVisible(label);

    if (index >= numSliders)
        numSliders = index + 1;
}

void EffectSection::setupTypeBox(const juce::String& labelText, const juce::StringArray& items)
{
    typeBox.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff1a1a2e));
    typeBox.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    typeBox.setColour(juce::ComboBox::outlineColourId, colour.withAlpha(0.5f));
    typeBox.setColour(juce::ComboBox::arrowColourId, colour);
    for (int i = 0; i < items.size(); ++i)
        typeBox.addItem(items[i], i + 1);
    addAndMakeVisible(typeBox);

    typeLabel.setText(labelText, juce::dontSendNotification);
    typeLabel.setJustificationType(juce::Justification::centred);
    typeLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.7f));
    typeLabel.setFont(juce::FontOptions(10.0f));
    addAndMakeVisible(typeLabel);

    hasTypeBox = true;
}

void EffectSection::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().reduced(2);

    // Main background with subtle gradient
    juce::ColourGradient gradient(juce::Colour(0xff1e1e32), 0.0f, 0.0f,
                                   juce::Colour(0xff141428), 0.0f, (float)bounds.getHeight(),
                                   false);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(bounds.toFloat(), 10.0f);

    // Inner shadow effect
    g.setColour(juce::Colour(0xff0a0a15));
    g.drawRoundedRectangle(bounds.toFloat().reduced(1), 9.0f, 2.0f);

    // Outer border
    g.setColour(colour.withAlpha(isDragOver ? 0.8f : 0.3f));
    g.drawRoundedRectangle(bounds.toFloat(), 10.0f, isDragOver ? 2.0f : 1.0f);

    // LED indicator
    bool isActive = !bypassButton.getToggleState();  // ON when not bypassed
    auto ledBounds = juce::Rectangle<float>(bounds.getX() + 14.0f, bounds.getY() + 12.0f, 10.0f, 10.0f);

    if (isActive)
    {
        // Glow effect
        g.setColour(colour.withAlpha(0.3f));
        g.fillEllipse(ledBounds.expanded(4.0f));
        g.setColour(colour.withAlpha(0.5f));
        g.fillEllipse(ledBounds.expanded(2.0f));
        // Main LED
        g.setColour(colour);
        g.fillEllipse(ledBounds);
        // Highlight
        g.setColour(juce::Colours::white.withAlpha(0.6f));
        g.fillEllipse(ledBounds.reduced(3.0f).translated(-1.0f, -1.0f));
    }
    else
    {
        // Dim LED
        g.setColour(colour.withAlpha(0.15f));
        g.fillEllipse(ledBounds);
        g.setColour(colour.withAlpha(0.3f));
        g.drawEllipse(ledBounds, 1.0f);
    }

    // Title
    g.setColour(colour);
    g.setFont(juce::FontOptions(14.0f).withStyle("Bold"));
    g.drawText(name, 30, 8, 200, 20, juce::Justification::left);

    // Drag indicator dots
    g.setColour(juce::Colours::white.withAlpha(0.25f));
    int dotX = bounds.getRight() - 22;
    int dotY = 12;
    for (int row = 0; row < 3; ++row)
    {
        g.fillEllipse((float)dotX, (float)(dotY + row * 6), 3.0f, 3.0f);
        g.fillEllipse((float)(dotX + 6), (float)(dotY + row * 6), 3.0f, 3.0f);
    }
}

void EffectSection::resized()
{
    const int knobSize = 55;
    const int labelHeight = 14;
    const int spacing = 68;
    const int startY = 32;
    int x = 18;

    for (int i = 0; i < numSliders; ++i)
    {
        labels[i].setBounds(x, startY, knobSize, labelHeight);
        sliders[i].setBounds(x, startY + labelHeight, knobSize, knobSize);
        x += spacing;
    }

    if (hasTypeBox)
    {
        typeLabel.setBounds(x, startY, 80, labelHeight);
        typeBox.setBounds(x, startY + labelHeight + 12, 80, 22);
        x += 95;
    }

    bypassButton.setBounds(x + 5, startY + 25, 55, 22);
}

void EffectSection::mouseDown(const juce::MouseEvent& e)
{
    juce::ignoreUnused(e);
}

void EffectSection::mouseDrag(const juce::MouseEvent& e)
{
    if (e.getDistanceFromDragStart() > 10)
    {
        if (auto* container = juce::DragAndDropContainer::findParentDragContainerFor(this))
        {
            auto snapshot = createComponentSnapshot(getLocalBounds());
            snapshot = snapshot.rescaled(snapshot.getWidth() / 2, snapshot.getHeight() / 2);
            juce::ScaledImage scaledImage(snapshot, 2.0);
            container->startDragging(juce::String(effectId), this, scaledImage, true, nullptr, &e.source);
        }
    }
}

bool EffectSection::isInterestedInDragSource(const SourceDetails& details)
{
    return details.description.toString().containsOnly("0123456789") &&
           details.description.toString().getIntValue() != effectId;
}

void EffectSection::itemDragEnter(const SourceDetails& details)
{
    juce::ignoreUnused(details);
    isDragOver = true;
    repaint();
}

void EffectSection::itemDragExit(const SourceDetails& details)
{
    juce::ignoreUnused(details);
    isDragOver = false;
    repaint();
}

void EffectSection::itemDropped(const SourceDetails& details)
{
    isDragOver = false;
    int fromEffectId = details.description.toString().getIntValue();
    parentEditor.handleReorder(fromEffectId, effectId);
    repaint();
}

//==============================================================================
// PDLBRDAudioProcessorEditor implementation
//==============================================================================
PDLBRDAudioProcessorEditor::PDLBRDAudioProcessorEditor(PDLBRDAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    addAndMakeVisible(inputMeter);
    addAndMakeVisible(outputMeter);

    createSections();

    auto& apvts = audioProcessor.getAPVTS();

    // Comp1 attachments
    auto& comp1 = *sections[PDLBRDAudioProcessor::COMP1];
    comp1ThresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "comp1Threshold", comp1.getSlider(0));
    comp1RatioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "comp1Ratio", comp1.getSlider(1));
    comp1AttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "comp1Attack", comp1.getSlider(2));
    comp1ReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "comp1Release", comp1.getSlider(3));
    comp1MakeupAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "comp1Makeup", comp1.getSlider(4));
    comp1BlendAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "comp1Blend", comp1.getSlider(5));
    comp1BypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "comp1Bypass", comp1.getBypassButton());

    // Distortion attachments
    auto& dist = *sections[PDLBRDAudioProcessor::DIST];
    distDriveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "distDrive", dist.getSlider(0));
    distToneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "distTone", dist.getSlider(1));
    distLevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "distLevel", dist.getSlider(2));
    distTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "distType", dist.getTypeBox());
    distBypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "distBypass", dist.getBypassButton());

    // Amp attachments
    auto& amp = *sections[PDLBRDAudioProcessor::AMP];
    ampGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "ampGain", amp.getSlider(0));
    ampBassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "ampBass", amp.getSlider(1));
    ampMidAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "ampMid", amp.getSlider(2));
    ampMidFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "ampMidFreq", amp.getSlider(3));
    ampTrebleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "ampTreble", amp.getSlider(4));
    ampMasterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "ampMaster", amp.getSlider(5));
    ampTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "ampType", amp.getTypeBox());
    ampBypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "ampBypass", amp.getBypassButton());

    // Modulation attachments
    auto& mod = *sections[PDLBRDAudioProcessor::MOD];
    modRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "modRate", mod.getSlider(0));
    modDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "modDepth", mod.getSlider(1));
    modBlendAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "modBlend", mod.getSlider(2));
    modTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "modType", mod.getTypeBox());
    modBypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "modBypass", mod.getBypassButton());

    // Reverb attachments
    auto& rev = *sections[PDLBRDAudioProcessor::REV];
    revDecayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "revDecay", rev.getSlider(0));
    revToneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "revTone", rev.getSlider(1));
    revBlendAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "revBlend", rev.getSlider(2));
    revTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "revType", rev.getTypeBox());
    revBypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "revBypass", rev.getBypassButton());

    // Comp2 attachments
    auto& comp2 = *sections[PDLBRDAudioProcessor::COMP2];
    comp2ThresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "comp2Threshold", comp2.getSlider(0));
    comp2RatioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "comp2Ratio", comp2.getSlider(1));
    comp2AttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "comp2Attack", comp2.getSlider(2));
    comp2ReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "comp2Release", comp2.getSlider(3));
    comp2MakeupAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "comp2Makeup", comp2.getSlider(4));
    comp2BlendAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "comp2Blend", comp2.getSlider(5));
    comp2BypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "comp2Bypass", comp2.getBypassButton());

    setSize(820, 940);
    startTimerHz(30);
}

PDLBRDAudioProcessorEditor::~PDLBRDAudioProcessorEditor()
{
    stopTimer();
}

void PDLBRDAudioProcessorEditor::timerCallback()
{
    // Smooth the levels for display
    float targetIn = audioProcessor.getInputLevel();
    float targetOut = audioProcessor.getOutputLevel();

    smoothedInputLevel = smoothedInputLevel * 0.8f + targetIn * 0.2f;
    smoothedOutputLevel = smoothedOutputLevel * 0.8f + targetOut * 0.2f;

    // Decay
    if (targetIn < smoothedInputLevel)
        smoothedInputLevel *= 0.92f;
    if (targetOut < smoothedOutputLevel)
        smoothedOutputLevel *= 0.92f;

    inputMeter.setLevel(smoothedInputLevel);
    outputMeter.setLevel(smoothedOutputLevel);

    // Repaint effect sections for LED updates
    for (auto& section : sections)
        if (section) section->repaint();
}

void PDLBRDAudioProcessorEditor::createSections()
{
    // Compressor 1
    sections[PDLBRDAudioProcessor::COMP1] = std::make_unique<EffectSection>(
        *this, PDLBRDAudioProcessor::COMP1, "COMPRESSOR 1", juce::Colour(0xff4ecdc4));
    auto& comp1 = *sections[PDLBRDAudioProcessor::COMP1];
    comp1.setupSlider(0, "Thresh");
    comp1.setupSlider(1, "Ratio");
    comp1.setupSlider(2, "Attack");
    comp1.setupSlider(3, "Release");
    comp1.setupSlider(4, "Makeup");
    comp1.setupSlider(5, "Blend");
    addAndMakeVisible(comp1);

    // Distortion
    sections[PDLBRDAudioProcessor::DIST] = std::make_unique<EffectSection>(
        *this, PDLBRDAudioProcessor::DIST, "DISTORTION", juce::Colour(0xffff6b6b));
    auto& dist = *sections[PDLBRDAudioProcessor::DIST];
    dist.setupSlider(0, "Drive");
    dist.setupSlider(1, "Tone");
    dist.setupSlider(2, "Level");
    dist.setupTypeBox("Type", {"TS9", "RAT", "Blues", "Fuzz", "Muff"});
    addAndMakeVisible(dist);

    // Amp Sim
    sections[PDLBRDAudioProcessor::AMP] = std::make_unique<EffectSection>(
        *this, PDLBRDAudioProcessor::AMP, "AMP SIM", juce::Colour(0xffffd93d));
    auto& amp = *sections[PDLBRDAudioProcessor::AMP];
    amp.setupSlider(0, "Gain");
    amp.setupSlider(1, "Bass");
    amp.setupSlider(2, "Mid");
    amp.setupSlider(3, "MidFrq");
    amp.setupSlider(4, "Treble");
    amp.setupSlider(5, "Master");
    amp.setupTypeBox("Type", {"Clean", "Crunch", "Lead", "Modern", "Vintage"});
    addAndMakeVisible(amp);

    // Modulation
    sections[PDLBRDAudioProcessor::MOD] = std::make_unique<EffectSection>(
        *this, PDLBRDAudioProcessor::MOD, "MODULATION", juce::Colour(0xffbb86fc));
    auto& mod = *sections[PDLBRDAudioProcessor::MOD];
    mod.setupSlider(0, "Rate");
    mod.setupSlider(1, "Depth");
    mod.setupSlider(2, "Blend");
    mod.setupTypeBox("Type", {"Phaser", "Flanger", "Chorus", "Tremolo", "Vibrato"});
    addAndMakeVisible(mod);

    // Reverb
    sections[PDLBRDAudioProcessor::REV] = std::make_unique<EffectSection>(
        *this, PDLBRDAudioProcessor::REV, "REVERB", juce::Colour(0xff6bcf7f));
    auto& rev = *sections[PDLBRDAudioProcessor::REV];
    rev.setupSlider(0, "Decay");
    rev.setupSlider(1, "Tone");
    rev.setupSlider(2, "Blend");
    rev.setupTypeBox("Type", {"Spring", "Plate", "Hall"});
    addAndMakeVisible(rev);

    // Compressor 2
    sections[PDLBRDAudioProcessor::COMP2] = std::make_unique<EffectSection>(
        *this, PDLBRDAudioProcessor::COMP2, "COMPRESSOR 2", juce::Colour(0xff45b7d1));
    auto& comp2 = *sections[PDLBRDAudioProcessor::COMP2];
    comp2.setupSlider(0, "Thresh");
    comp2.setupSlider(1, "Ratio");
    comp2.setupSlider(2, "Attack");
    comp2.setupSlider(3, "Release");
    comp2.setupSlider(4, "Makeup");
    comp2.setupSlider(5, "Blend");
    addAndMakeVisible(comp2);
}

void PDLBRDAudioProcessorEditor::handleReorder(int fromEffectId, int toEffectId)
{
    auto order = audioProcessor.getEffectOrder();

    int fromPos = -1, toPos = -1;
    for (int i = 0; i < PDLBRDAudioProcessor::NUM_EFFECTS; ++i)
    {
        if (order[i] == fromEffectId) fromPos = i;
        if (order[i] == toEffectId) toPos = i;
    }

    if (fromPos < 0 || toPos < 0 || fromPos == toPos)
        return;

    int movingEffect = order[fromPos];
    if (fromPos < toPos)
    {
        for (int i = fromPos; i < toPos; ++i)
            order[i] = order[i + 1];
    }
    else
    {
        for (int i = fromPos; i > toPos; --i)
            order[i] = order[i - 1];
    }
    order[toPos] = movingEffect;

    audioProcessor.setEffectOrder(order);
    layoutSections();
    repaint();
}

void PDLBRDAudioProcessorEditor::layoutSections()
{
    auto order = audioProcessor.getEffectOrder();
    const int sectionHeight = 140;
    const int sectionGap = 6;
    int y = 50;

    for (int i = 0; i < PDLBRDAudioProcessor::NUM_EFFECTS; ++i)
    {
        int effectId = order[i];
        sections[effectId]->setBounds(30, y, getWidth() - 60, sectionHeight);
        y += sectionHeight + sectionGap;
    }
}

void PDLBRDAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Background gradient
    juce::ColourGradient bgGradient(juce::Colour(0xff12121c), 0.0f, 0.0f,
                                     juce::Colour(0xff1a1a2e), 0.0f, (float)getHeight(),
                                     false);
    g.setGradientFill(bgGradient);
    g.fillAll();

    // Header
    g.setColour(juce::Colour(0xff0e0e18));
    g.fillRect(0, 0, getWidth(), 42);

    // Header accent line
    g.setColour(juce::Colour(0xff4ecdc4));
    g.fillRect(0, 42, getWidth(), 2);

    // Title
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(22.0f).withStyle("Bold"));
    g.drawText("PDLBRD", 15, 8, 150, 28, juce::Justification::left);

    // Subtitle
    g.setColour(juce::Colours::white.withAlpha(0.5f));
    g.setFont(juce::FontOptions(10.0f));
    g.drawText("PEDALBOARD", 15, 28, 100, 12, juce::Justification::left);

    // Meter labels
    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.setFont(juce::FontOptions(9.0f));
    g.drawText("IN", getWidth() - 55, 6, 20, 12, juce::Justification::centred);
    g.drawText("OUT", getWidth() - 30, 6, 20, 12, juce::Justification::centred);
}

void PDLBRDAudioProcessorEditor::resized()
{
    // Level meters in header
    inputMeter.setBounds(getWidth() - 55, 18, 12, 20);
    outputMeter.setBounds(getWidth() - 30, 18, 12, 20);

    layoutSections();
}
