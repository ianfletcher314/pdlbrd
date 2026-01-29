#include "PluginProcessor.h"
#include "PluginEditor.h"

// Compstortion earth tone colors
namespace PedalColors {
    const juce::Colour comp1      = juce::Colour(0xff8b4513);  // Saddle brown
    const juce::Colour distortion = juce::Colour(0xff556b2f);  // Olive
    const juce::Colour amp        = juce::Colour(0xff4a5568);  // Gray
    const juce::Colour modulation = juce::Colour(0xff2c5545);  // Dark teal
    const juce::Colour reverb     = juce::Colour(0xff5d3a3a);  // Burgundy
    const juce::Colour comp2      = juce::Colour(0xff6b4423);  // Darker brown
    const juce::Colour background = juce::Colour(0xff1a1a1a);  // Dark background
    const juce::Colour metal      = juce::Colour(0xffc0c0c0);  // Brushed metal
    const juce::Colour screw      = juce::Colour(0xff505050);  // Screw color
    const juce::Colour led        = juce::Colour(0xffff3333);  // LED red
}

//==============================================================================
// Helper functions for drawing pedal components
//==============================================================================
static void drawScrew(juce::Graphics& g, float x, float y, float size)
{
    // Outer ring
    g.setColour(PedalColors::screw.darker(0.3f));
    g.fillEllipse(x - size/2, y - size/2, size, size);

    // Inner hex pattern (simplified as star)
    g.setColour(PedalColors::screw.brighter(0.2f));
    g.fillEllipse(x - size/3, y - size/3, size * 0.66f, size * 0.66f);

    // Hex slot
    juce::Path hex;
    float r = size * 0.22f;
    for (int i = 0; i < 6; ++i)
    {
        float angle = i * juce::MathConstants<float>::pi / 3.0f - juce::MathConstants<float>::pi / 6.0f;
        float px = x + r * std::cos(angle);
        float py = y + r * std::sin(angle);
        if (i == 0)
            hex.startNewSubPath(px, py);
        else
            hex.lineTo(px, py);
    }
    hex.closeSubPath();
    g.setColour(PedalColors::screw.darker(0.5f));
    g.fillPath(hex);
}

static void drawBrushedMetalTexture(juce::Graphics& g, juce::Rectangle<float> bounds, juce::Colour baseColour)
{
    // Base color
    g.setColour(baseColour);
    g.fillRoundedRectangle(bounds, 12.0f);

    // Subtle diagonal brush strokes
    g.setColour(juce::Colours::white.withAlpha(0.03f));
    for (float i = -bounds.getHeight(); i < bounds.getWidth() + bounds.getHeight(); i += 3.0f)
    {
        g.drawLine(bounds.getX() + i, bounds.getY(),
                   bounds.getX() + i + bounds.getHeight(), bounds.getBottom(), 0.5f);
    }

    // Top highlight
    juce::ColourGradient topHighlight(baseColour.brighter(0.15f), bounds.getX(), bounds.getY(),
                                       baseColour, bounds.getX(), bounds.getY() + 30.0f, false);
    g.setGradientFill(topHighlight);
    g.fillRoundedRectangle(bounds.getX(), bounds.getY(), bounds.getWidth(), 30.0f, 12.0f);
}

static void drawFootswitch(juce::Graphics& g, juce::Rectangle<float> bounds, bool isOn, juce::Colour ledColour)
{
    float cx = bounds.getCentreX();
    float cy = bounds.getCentreY();

    // Metal housing/bezel
    auto housingBounds = bounds.expanded(2.0f);
    g.setColour(juce::Colour(0xff2a2a2a));
    g.fillRoundedRectangle(housingBounds, 4.0f);
    g.setColour(juce::Colour(0xff3a3a3a));
    g.drawRoundedRectangle(housingBounds, 4.0f, 1.0f);

    // Switch base (recessed area)
    g.setColour(juce::Colour(0xff0a0a0a));
    g.fillRoundedRectangle(bounds, 3.0f);

    // The actual switch button - 3D raised look
    auto buttonBounds = bounds.reduced(4.0f);
    float pressOffset = isOn ? 1.0f : 0.0f;
    buttonBounds = buttonBounds.translated(0, pressOffset);

    // Button shadow (when raised/off)
    if (!isOn)
    {
        g.setColour(juce::Colour(0xff000000));
        g.fillRoundedRectangle(buttonBounds.translated(0, 2), 3.0f);
    }

    // Button top surface gradient
    juce::ColourGradient buttonGrad(
        isOn ? juce::Colour(0xff353535) : juce::Colour(0xff484848),
        buttonBounds.getX(), buttonBounds.getY(),
        isOn ? juce::Colour(0xff252525) : juce::Colour(0xff383838),
        buttonBounds.getX(), buttonBounds.getBottom(), false);
    g.setGradientFill(buttonGrad);
    g.fillRoundedRectangle(buttonBounds, 3.0f);

    // Button edge highlight
    g.setColour(isOn ? juce::Colour(0xff404040) : juce::Colour(0xff555555));
    g.drawRoundedRectangle(buttonBounds, 3.0f, 1.0f);

    // Center ridge/bump on button
    float ridgeWidth = buttonBounds.getWidth() * 0.5f;
    float ridgeHeight = 4.0f;
    auto ridgeBounds = juce::Rectangle<float>(
        cx - ridgeWidth / 2, cy - ridgeHeight / 2 + pressOffset,
        ridgeWidth, ridgeHeight);
    g.setColour(isOn ? juce::Colour(0xff303030) : juce::Colour(0xff4a4a4a));
    g.fillRoundedRectangle(ridgeBounds, 2.0f);

    // LED to the LEFT of footswitch (only if ledColour is not transparent)
    if (ledColour.getAlpha() > 0)
    {
        float ledSize = 10.0f;
        float ledX = bounds.getX() - 28.0f;  // Moved further left
        float ledY = cy - ledSize / 2.0f;

        if (isOn)
        {
            // Glow
            g.setColour(ledColour.withAlpha(0.4f));
            g.fillEllipse(ledX - ledSize * 0.5f, ledY - ledSize * 0.5f, ledSize * 2.0f, ledSize * 2.0f);
            g.setColour(ledColour.withAlpha(0.6f));
            g.fillEllipse(ledX - ledSize * 0.2f, ledY - ledSize * 0.2f, ledSize * 1.4f, ledSize * 1.4f);

            // Main LED
            juce::ColourGradient ledGradient(ledColour.brighter(0.5f), ledX + ledSize/2, ledY,
                                              ledColour, ledX + ledSize/2, ledY + ledSize, true);
            g.setGradientFill(ledGradient);
            g.fillEllipse(ledX, ledY, ledSize, ledSize);
        }
        else
        {
            g.setColour(ledColour.withAlpha(0.15f));
            g.fillEllipse(ledX, ledY, ledSize, ledSize);
            g.setColour(ledColour.withAlpha(0.3f));
            g.drawEllipse(ledX, ledY, ledSize, ledSize, 1.0f);
        }
    }
}

//==============================================================================
// LevelMeter implementation
//==============================================================================
void LevelMeter::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Background
    g.setColour(juce::Colour(0xff151515));
    g.fillRoundedRectangle(bounds, 3.0f);

    // Calculate level width (convert to dB, then normalize)
    float db = juce::Decibels::gainToDecibels(level, -60.0f);
    float normalized = juce::jmap(db, -60.0f, 0.0f, 0.0f, 1.0f);
    normalized = juce::jlimit(0.0f, 1.0f, normalized);

    // Level bar (horizontal, left to right)
    float barWidth = bounds.getWidth() * normalized;
    auto barBounds = bounds.removeFromLeft(barWidth);

    // Gradient from green to yellow to red
    juce::Colour barColour;
    if (normalized < 0.6f)
        barColour = juce::Colour(0xff22c55e);  // Green
    else if (normalized < 0.85f)
        barColour = juce::Colour(0xffeab308);  // Yellow
    else
        barColour = juce::Colour(0xffef4444);  // Red

    g.setColour(barColour);
    g.fillRoundedRectangle(barBounds.reduced(1), 2.0f);

    // Outline
    g.setColour(juce::Colour(0xff333333));
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 3.0f, 1.0f);
}

//==============================================================================
// EffectSection implementation
//==============================================================================
EffectSection::EffectSection(PDLBRDAudioProcessorEditor& editor, int id, const juce::String& n, juce::Colour c)
    : parentEditor(editor), effectId(id), name(n), colour(c)
{
    // Create custom look and feel for this section's knobs
    lookAndFeel = std::make_unique<CompstortionLookAndFeel>();
    lookAndFeel->setAccentColour(juce::Colours::white);

    // Bypass button - we'll draw it ourselves as a footswitch
    bypassButton.setClickingTogglesState(true);
    bypassButton.setAlpha(0.0f);  // Invisible, we draw custom
    addAndMakeVisible(bypassButton);
}

void EffectSection::setupSlider(int index, const juce::String& labelText)
{
    if (index >= MAX_SLIDERS) return;

    auto& slider = sliders[index];
    auto& label = labels[index];

    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 14);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white.withAlpha(0.9f));
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    slider.setLookAndFeel(lookAndFeel.get());
    addAndMakeVisible(slider);

    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.85f));
    label.setFont(juce::FontOptions(10.0f).withStyle("Bold"));
    addAndMakeVisible(label);

    if (index >= numSliders)
        numSliders = index + 1;
}

void EffectSection::setupTypeBox(const juce::String& labelText, const juce::StringArray& items)
{
    juce::ignoreUnused(labelText);

    // LCD-style display - larger and more readable
    typeBox.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff0a0a0a));
    typeBox.setColour(juce::ComboBox::textColourId, juce::Colour(0xff88cc88));  // Green LCD
    typeBox.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff333333));
    typeBox.setColour(juce::ComboBox::arrowColourId, juce::Colour(0xff88cc88));
    for (int i = 0; i < items.size(); ++i)
        typeBox.addItem(items[i], i + 1);
    addAndMakeVisible(typeBox);

    // No label needed anymore
    hasTypeBox = true;
}

void EffectSection::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().reduced(4).toFloat();

    // Brushed metal pedal body
    drawBrushedMetalTexture(g, bounds, colour);

    // Beveled edge effect
    g.setColour(colour.brighter(0.2f));
    g.drawRoundedRectangle(bounds, 12.0f, 2.0f);
    g.setColour(colour.darker(0.3f));
    g.drawRoundedRectangle(bounds.reduced(2), 10.0f, 1.0f);

    // Drag highlight
    if (isDragOver)
    {
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.drawRoundedRectangle(bounds, 12.0f, 3.0f);
    }

    // Corner screws
    float screwSize = 10.0f;
    float screwInset = 14.0f;
    drawScrew(g, bounds.getX() + screwInset, bounds.getY() + screwInset, screwSize);
    drawScrew(g, bounds.getRight() - screwInset, bounds.getY() + screwInset, screwSize);
    drawScrew(g, bounds.getX() + screwInset, bounds.getBottom() - screwInset, screwSize);
    drawScrew(g, bounds.getRight() - screwInset, bounds.getBottom() - screwInset, screwSize);

    // Vertical title on left side (reading upward)
    g.saveState();
    g.setColour(juce::Colours::white.withAlpha(0.9f));
    g.setFont(juce::FontOptions(11.0f).withStyle("Bold"));

    // Center title between screw (at ~19 from bounds edge) and first knob (at ~46 from bounds edge)
    float screwRightEdge = screwInset + screwSize / 2.0f;  // 14 + 5 = 19
    float knobLeftEdge = 46.0f;  // knobs start at x=50 in component coords, bounds is reduced by 4
    float titleX = bounds.getX() + (screwRightEdge + knobLeftEdge) / 2.0f;
    float titleY = bounds.getCentreY();
    g.addTransform(juce::AffineTransform::rotation(-juce::MathConstants<float>::halfPi, titleX, titleY));
    g.drawText(name, (int)(titleX - 50), (int)(titleY - 7), 100, 14, juce::Justification::centred);
    g.restoreState();

    // Draw footswitch (vertically centered on right side)
    bool isActive = !bypassButton.getToggleState();
    float footswitchY = bounds.getCentreY() - 22;
    auto footswitchBounds = juce::Rectangle<float>(bounds.getRight() - 60, footswitchY, 44, 44);

    if (isCompressor)
    {
        // Draw horizontal gain reduction meter with segmented LEDs
        float meterX = 430;
        float meterY = bounds.getCentreY() - 12;
        float meterWidth = bounds.getRight() - 145 - meterX;  // Leave room for LED
        float meterHeight = 24;

        // Meter background
        g.setColour(juce::Colour(0xff0a0a0a));
        g.fillRoundedRectangle(meterX, meterY, meterWidth, meterHeight, 4.0f);
        g.setColour(juce::Colour(0xff333333));
        g.drawRoundedRectangle(meterX, meterY, meterWidth, meterHeight, 4.0f, 1.0f);

        // Segmented LED meter (like classic VU meters)
        // 12 segments: 6 green (0-6dB), 3 yellow (6-12dB), 3 red (12-18dB)
        const int numSegments = 12;
        float segmentWidth = (meterWidth - 10) / numSegments;
        float segmentHeight = meterHeight - 8;
        float segmentGap = 2.0f;

        // Gain reduction is in dB (0 to ~18dB for full scale)
        float grDb = juce::jlimit(0.0f, 18.0f, gainReduction);
        int litSegments = (int)((grDb / 18.0f) * numSegments);

        for (int i = 0; i < numSegments; ++i)
        {
            float segX = meterX + 5 + i * segmentWidth;

            // Determine segment color
            juce::Colour segColour;
            if (i < 6)
                segColour = juce::Colour(0xff22c55e);  // Green
            else if (i < 9)
                segColour = juce::Colour(0xffeab308);  // Yellow
            else
                segColour = juce::Colour(0xffef4444);  // Red

            bool isLit = isActive && (i < litSegments);

            if (isLit)
            {
                g.setColour(segColour);
            }
            else
            {
                g.setColour(segColour.withAlpha(0.15f));  // Dim unlit segments
            }

            g.fillRoundedRectangle(segX, meterY + 4, segmentWidth - segmentGap, segmentHeight, 2.0f);
        }
    }

    // Draw footswitch with LED for all pedals (moved further left)
    drawFootswitch(g, footswitchBounds, isActive, PedalColors::led);

    // Drag handle indicator (subtle dots at top right)
    g.setColour(juce::Colours::white.withAlpha(0.2f));
    float dotX = bounds.getRight() - 26;
    float dotY = bounds.getY() + 10;
    for (int row = 0; row < 2; ++row)
    {
        for (int col = 0; col < 2; ++col)
        {
            g.fillEllipse(dotX + col * 5, dotY + row * 5, 2.5f, 2.5f);
        }
    }
}

void EffectSection::resized()
{
    auto bounds = getLocalBounds().reduced(4);
    const int knobSize = 54;
    const int labelHeight = 14;
    const int spacing = 64;
    const int startY = 10;
    int x = 50;  // More space for vertical title + screws

    for (int i = 0; i < numSliders; ++i)
    {
        labels[i].setBounds(x, startY, knobSize, labelHeight);
        sliders[i].setBounds(x, startY + labelHeight, knobSize, knobSize);
        x += spacing;
    }

    if (hasTypeBox)
    {
        // Larger type selector, vertically centered, no label
        int typeBoxHeight = 28;
        int typeBoxY = bounds.getCentreY() - typeBoxHeight / 2;
        typeBox.setBounds(x, typeBoxY, 90, typeBoxHeight);
    }

    // Footswitch hit area (vertically centered)
    int footswitchY = bounds.getCentreY() - 22;
    bypassButton.setBounds(bounds.getRight() - 60, footswitchY, 44, 44);
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
// PedalboardContent implementation
//==============================================================================
void PedalboardContent::paint(juce::Graphics& g)
{
    // Dark wood-tone background (like pedalboard surface)
    g.fillAll(juce::Colour(0xff1a1a1a));

    // Subtle wood grain texture
    g.setColour(juce::Colour(0xff222222));
    for (int y = 0; y < getHeight(); y += 6)
    {
        float offset = std::sin(y * 0.02f) * 3.0f;
        g.drawLine(0, (float)y, (float)getWidth(), (float)y + offset, 0.5f);
    }
}

//==============================================================================
// PDLBRDAudioProcessorEditor implementation
//==============================================================================
PDLBRDAudioProcessorEditor::PDLBRDAudioProcessorEditor(PDLBRDAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    addAndMakeVisible(inputMeter);
    addAndMakeVisible(outputMeter);

    // Set up scrollable viewport
    viewport.setViewedComponent(&contentComponent, false);
    viewport.setScrollBarsShown(true, false);  // Vertical scrollbar only
    viewport.getVerticalScrollBar().setColour(juce::ScrollBar::thumbColourId, juce::Colour(0xff555555));
    viewport.getVerticalScrollBar().setColour(juce::ScrollBar::trackColourId, juce::Colour(0xff222222));
    addAndMakeVisible(viewport);

    // Preset controls - dark metal style
    presetSelector.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff0a0a0a));
    presetSelector.setColour(juce::ComboBox::textColourId, juce::Colour(0xff88cc88));  // LCD green
    presetSelector.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff333333));
    presetSelector.setColour(juce::ComboBox::arrowColourId, juce::Colour(0xff88cc88));
    presetSelector.setTextWhenNothingSelected("Select Preset...");
    presetSelector.onChange = [this]() {
        int index = presetSelector.getSelectedItemIndex();
        if (index >= 0)
        {
            auto presetName = presetSelector.getText();
            auto file = PDLBRDAudioProcessor::getPresetsFolder().getChildFile(presetName + ".pdlbrd");
            audioProcessor.loadPreset(file);
            layoutSections();
            repaint();
        }
    };
    addAndMakeVisible(presetSelector);
    refreshPresetList();

    saveButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff2a2a2a));
    saveButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff3a3a3a));
    saveButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xffcccccc));
    saveButton.onClick = [this]() { savePreset(); };
    addAndMakeVisible(saveButton);

    loadButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff2a2a2a));
    loadButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff3a3a3a));
    loadButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xffcccccc));
    loadButton.onClick = [this]() { loadPreset(); };
    addAndMakeVisible(loadButton);

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

    setSize(720, 700);  // Wider and shorter (scrollable)
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

    // Update gain reduction meters for compressors
    sections[PDLBRDAudioProcessor::COMP1]->setGainReduction(audioProcessor.getCompressor1GainReduction());
    sections[PDLBRDAudioProcessor::COMP2]->setGainReduction(audioProcessor.getCompressor2GainReduction());

    // Repaint effect sections for LED/meter updates
    for (auto& section : sections)
        if (section) section->repaint();
}

void PDLBRDAudioProcessorEditor::createSections()
{
    // Compressor 1 - Saddle brown (like Compstortion)
    sections[PDLBRDAudioProcessor::COMP1] = std::make_unique<EffectSection>(
        *this, PDLBRDAudioProcessor::COMP1, "COMPRESSOR", PedalColors::comp1);
    auto& comp1 = *sections[PDLBRDAudioProcessor::COMP1];
    comp1.setIsCompressor(true);
    comp1.setupSlider(0, "THRESH");
    comp1.setupSlider(1, "RATIO");
    comp1.setupSlider(2, "ATTACK");
    comp1.setupSlider(3, "RELEASE");
    comp1.setupSlider(4, "MAKEUP");
    comp1.setupSlider(5, "BLEND");
    comp1.getSlider(0).setTextValueSuffix(" dB");
    comp1.getSlider(1).setTextValueSuffix(":1");
    comp1.getSlider(2).setTextValueSuffix(" ms");
    comp1.getSlider(3).setTextValueSuffix(" ms");
    comp1.getSlider(4).setTextValueSuffix(" dB");
    comp1.getSlider(5).setTextValueSuffix("%");
    contentComponent.addAndMakeVisible(comp1);

    // Distortion - Olive green
    sections[PDLBRDAudioProcessor::DIST] = std::make_unique<EffectSection>(
        *this, PDLBRDAudioProcessor::DIST, "DISTORTION", PedalColors::distortion);
    auto& dist = *sections[PDLBRDAudioProcessor::DIST];
    dist.setupSlider(0, "DRIVE");
    dist.setupSlider(1, "TONE");
    dist.setupSlider(2, "LEVEL");
    dist.setupTypeBox("TYPE", {"TS9", "RAT", "Blues", "Fuzz", "Muff"});
    dist.getSlider(0).setTextValueSuffix("%");
    dist.getSlider(1).setTextValueSuffix("%");
    dist.getSlider(2).setTextValueSuffix("%");
    contentComponent.addAndMakeVisible(dist);

    // Amp - Gray/metal
    sections[PDLBRDAudioProcessor::AMP] = std::make_unique<EffectSection>(
        *this, PDLBRDAudioProcessor::AMP, "AMP", PedalColors::amp);
    auto& amp = *sections[PDLBRDAudioProcessor::AMP];
    amp.setupSlider(0, "GAIN");
    amp.setupSlider(1, "BASS");
    amp.setupSlider(2, "MID");
    amp.setupSlider(3, "MID FRQ");
    amp.setupSlider(4, "TREBLE");
    amp.setupSlider(5, "MASTER");
    amp.setupTypeBox("TYPE", {"Clean", "Crunch", "Lead", "Modern", "Vintage"});
    amp.getSlider(0).setTextValueSuffix("%");
    amp.getSlider(1).setTextValueSuffix("%");
    amp.getSlider(2).setTextValueSuffix("%");
    amp.getSlider(3).setTextValueSuffix("%");
    amp.getSlider(4).setTextValueSuffix("%");
    amp.getSlider(5).setTextValueSuffix("%");
    contentComponent.addAndMakeVisible(amp);

    // Modulation - Dark teal
    sections[PDLBRDAudioProcessor::MOD] = std::make_unique<EffectSection>(
        *this, PDLBRDAudioProcessor::MOD, "MODULATION", PedalColors::modulation);
    auto& mod = *sections[PDLBRDAudioProcessor::MOD];
    mod.setupSlider(0, "RATE");
    mod.setupSlider(1, "DEPTH");
    mod.setupSlider(2, "BLEND");
    mod.setupTypeBox("TYPE", {"Phaser", "Flanger", "Chorus", "Tremolo", "Vibrato"});
    mod.getSlider(0).setTextValueSuffix("%");
    mod.getSlider(1).setTextValueSuffix("%");
    mod.getSlider(2).setTextValueSuffix("%");
    contentComponent.addAndMakeVisible(mod);

    // Reverb - Burgundy
    sections[PDLBRDAudioProcessor::REV] = std::make_unique<EffectSection>(
        *this, PDLBRDAudioProcessor::REV, "REVERB", PedalColors::reverb);
    auto& rev = *sections[PDLBRDAudioProcessor::REV];
    rev.setupSlider(0, "DECAY");
    rev.setupSlider(1, "TONE");
    rev.setupSlider(2, "BLEND");
    rev.setupTypeBox("TYPE", {"Spring", "Plate", "Hall"});
    rev.getSlider(0).setTextValueSuffix("%");
    rev.getSlider(1).setTextValueSuffix("%");
    rev.getSlider(2).setTextValueSuffix("%");
    contentComponent.addAndMakeVisible(rev);

    // Compressor 2 - Darker brown
    sections[PDLBRDAudioProcessor::COMP2] = std::make_unique<EffectSection>(
        *this, PDLBRDAudioProcessor::COMP2, "COMPRESSOR", PedalColors::comp2);
    auto& comp2 = *sections[PDLBRDAudioProcessor::COMP2];
    comp2.setIsCompressor(true);
    comp2.setupSlider(0, "THRESH");
    comp2.setupSlider(1, "RATIO");
    comp2.setupSlider(2, "ATTACK");
    comp2.setupSlider(3, "RELEASE");
    comp2.setupSlider(4, "MAKEUP");
    comp2.setupSlider(5, "BLEND");
    comp2.getSlider(0).setTextValueSuffix(" dB");
    comp2.getSlider(1).setTextValueSuffix(":1");
    comp2.getSlider(2).setTextValueSuffix(" ms");
    comp2.getSlider(3).setTextValueSuffix(" ms");
    comp2.getSlider(4).setTextValueSuffix(" dB");
    comp2.getSlider(5).setTextValueSuffix("%");
    contentComponent.addAndMakeVisible(comp2);
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
    const int sectionHeight = 95;  // Shorter pedals
    const int sectionGap = 6;
    const int contentWidth = viewport.getWidth() - viewport.getScrollBarThickness() - 10;
    int y = 8;

    for (int i = 0; i < PDLBRDAudioProcessor::NUM_EFFECTS; ++i)
    {
        int effectId = order[i];
        sections[effectId]->setBounds(8, y, contentWidth - 8, sectionHeight);
        y += sectionHeight + sectionGap;
    }

    // Set the content component size to fit all sections
    int totalHeight = y + 8;
    contentComponent.setSize(contentWidth, totalHeight);
}

void PDLBRDAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(juce::Colour(0xff1a1a1a));

    // Header bar
    juce::ColourGradient headerGradient(juce::Colour(0xff252525), 0.0f, 0.0f,
                                         juce::Colour(0xff1a1a1a), 0.0f, 50.0f, false);
    g.setGradientFill(headerGradient);
    g.fillRect(0, 0, getWidth(), 50);

    // Header bottom edge (subtle metal strip)
    g.setColour(juce::Colour(0xff333333));
    g.fillRect(0, 48, getWidth(), 2);
    g.setColour(juce::Colour(0xff404040));
    g.drawLine(0, 48, (float)getWidth(), 48, 1.0f);

    // Title with slight emboss effect
    g.setColour(juce::Colour(0xff111111));
    g.setFont(juce::FontOptions(24.0f).withStyle("Bold"));
    g.drawText("PDLBRD", 16, 16, 150, 30, juce::Justification::left);
    g.setColour(juce::Colours::white);
    g.drawText("PDLBRD", 15, 15, 150, 30, juce::Justification::left);

    // Meter labels (above the horizontal meters)
    g.setColour(juce::Colour(0xffaaaaaa));
    g.setFont(juce::FontOptions(9.0f).withStyle("Bold"));
    g.drawText("IN", 490, 10, 20, 12, juce::Justification::left);
    g.drawText("OUT", 490, 28, 24, 12, juce::Justification::left);
}

void PDLBRDAudioProcessorEditor::refreshPresetList()
{
    presetSelector.clear();
    auto presets = audioProcessor.getPresetList();
    int id = 1;
    for (auto& name : presets)
        presetSelector.addItem(name, id++);
}

void PDLBRDAudioProcessorEditor::savePreset()
{
    auto folder = PDLBRDAudioProcessor::getPresetsFolder();

    auto chooser = std::make_shared<juce::FileChooser>("Save Preset", folder, "*.pdlbrd");
    chooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
        [this, chooser](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file != juce::File{})
            {
                auto finalFile = file.hasFileExtension(".pdlbrd") ? file : file.withFileExtension(".pdlbrd");
                audioProcessor.savePreset(finalFile);
                refreshPresetList();

                auto name = finalFile.getFileNameWithoutExtension();
                for (int i = 0; i < presetSelector.getNumItems(); ++i)
                {
                    if (presetSelector.getItemText(i) == name)
                    {
                        presetSelector.setSelectedItemIndex(i, juce::dontSendNotification);
                        break;
                    }
                }
            }
        });
}

void PDLBRDAudioProcessorEditor::loadPreset()
{
    auto folder = PDLBRDAudioProcessor::getPresetsFolder();

    auto chooser = std::make_shared<juce::FileChooser>("Load Preset", folder, "*.pdlbrd");
    chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this, chooser](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file.existsAsFile())
            {
                audioProcessor.loadPreset(file);
                layoutSections();
                repaint();

                auto name = file.getFileNameWithoutExtension();
                for (int i = 0; i < presetSelector.getNumItems(); ++i)
                {
                    if (presetSelector.getItemText(i) == name)
                    {
                        presetSelector.setSelectedItemIndex(i, juce::dontSendNotification);
                        break;
                    }
                }
            }
        });
}

void PDLBRDAudioProcessorEditor::resized()
{
    const int headerHeight = 50;

    // Preset controls in header
    presetSelector.setBounds(110, 14, 200, 26);
    saveButton.setBounds(320, 14, 55, 26);
    loadButton.setBounds(380, 14, 55, 26);

    // Horizontal level meters in header (between load button and right edge)
    int meterLeft = 515;
    int meterWidth = getWidth() - meterLeft - 15;
    inputMeter.setBounds(meterLeft, 10, meterWidth, 14);
    outputMeter.setBounds(meterLeft, 28, meterWidth, 14);

    // Viewport fills remaining space below header
    viewport.setBounds(0, headerHeight, getWidth(), getHeight() - headerHeight);

    layoutSections();
}
