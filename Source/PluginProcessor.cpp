#include "PluginProcessor.h"
#include "PluginEditor.h"

PDLBRDAudioProcessor::PDLBRDAudioProcessor()
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
       apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    // Initialize effect pointers array
    effects[COMP1] = &compressor1;
    effects[DIST] = &distortion;
    effects[AMP] = &ampSim;
    effects[MOD] = &modulation;
    effects[REV] = &reverb;
    effects[COMP2] = &compressor2;

    // Compressor 1
    comp1Threshold = apvts.getRawParameterValue("comp1Threshold");
    comp1Ratio = apvts.getRawParameterValue("comp1Ratio");
    comp1Attack = apvts.getRawParameterValue("comp1Attack");
    comp1Release = apvts.getRawParameterValue("comp1Release");
    comp1Makeup = apvts.getRawParameterValue("comp1Makeup");
    comp1Blend = apvts.getRawParameterValue("comp1Blend");
    comp1Bypass = apvts.getRawParameterValue("comp1Bypass");

    // Compressor 2
    comp2Threshold = apvts.getRawParameterValue("comp2Threshold");
    comp2Ratio = apvts.getRawParameterValue("comp2Ratio");
    comp2Attack = apvts.getRawParameterValue("comp2Attack");
    comp2Release = apvts.getRawParameterValue("comp2Release");
    comp2Makeup = apvts.getRawParameterValue("comp2Makeup");
    comp2Blend = apvts.getRawParameterValue("comp2Blend");
    comp2Bypass = apvts.getRawParameterValue("comp2Bypass");

    // Distortion
    distDrive = apvts.getRawParameterValue("distDrive");
    distTone = apvts.getRawParameterValue("distTone");
    distLevel = apvts.getRawParameterValue("distLevel");
    distType = apvts.getRawParameterValue("distType");
    distBypass = apvts.getRawParameterValue("distBypass");

    // Amp Sim
    ampGain = apvts.getRawParameterValue("ampGain");
    ampBass = apvts.getRawParameterValue("ampBass");
    ampMid = apvts.getRawParameterValue("ampMid");
    ampMidFreq = apvts.getRawParameterValue("ampMidFreq");
    ampTreble = apvts.getRawParameterValue("ampTreble");
    ampMaster = apvts.getRawParameterValue("ampMaster");
    ampType = apvts.getRawParameterValue("ampType");
    ampBypass = apvts.getRawParameterValue("ampBypass");

    // Modulation
    modRate = apvts.getRawParameterValue("modRate");
    modDepth = apvts.getRawParameterValue("modDepth");
    modBlend = apvts.getRawParameterValue("modBlend");
    modType = apvts.getRawParameterValue("modType");
    modBypass = apvts.getRawParameterValue("modBypass");

    // Reverb
    revDecay = apvts.getRawParameterValue("revDecay");
    revTone = apvts.getRawParameterValue("revTone");
    revBlend = apvts.getRawParameterValue("revBlend");
    revType = apvts.getRawParameterValue("revType");
    revBypass = apvts.getRawParameterValue("revBypass");
}

PDLBRDAudioProcessor::~PDLBRDAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout PDLBRDAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // === COMPRESSOR 1 ===
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("comp1Threshold", 1), "Comp1 Threshold",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -20.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("comp1Ratio", 1), "Comp1 Ratio",
        juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f, 0.5f), 4.0f,
        juce::AudioParameterFloatAttributes().withLabel(":1")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("comp1Attack", 1), "Comp1 Attack",
        juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f, 0.4f), 10.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("comp1Release", 1), "Comp1 Release",
        juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f, 0.4f), 100.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("comp1Makeup", 1), "Comp1 Makeup",
        juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("comp1Blend", 1), "Comp1 Blend",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("comp1Bypass", 1), "Comp1 Bypass", true));

    // === COMPRESSOR 2 ===
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("comp2Threshold", 1), "Comp2 Threshold",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -12.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("comp2Ratio", 1), "Comp2 Ratio",
        juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f, 0.5f), 2.0f,
        juce::AudioParameterFloatAttributes().withLabel(":1")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("comp2Attack", 1), "Comp2 Attack",
        juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f, 0.4f), 20.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("comp2Release", 1), "Comp2 Release",
        juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f, 0.4f), 200.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("comp2Makeup", 1), "Comp2 Makeup",
        juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("comp2Blend", 1), "Comp2 Blend",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("comp2Bypass", 1), "Comp2 Bypass", true));

    // === DISTORTION ===
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("distDrive", 1), "Dist Drive",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("distTone", 1), "Dist Tone",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("distLevel", 1), "Dist Level",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("distType", 1), "Dist Type",
        juce::StringArray{ "TS9", "RAT", "Blues", "Fuzz", "Muff" }, 0));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("distBypass", 1), "Dist Bypass", true));

    // === AMP SIM ===
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("ampGain", 1), "Amp Gain",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 30.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("ampBass", 1), "Amp Bass",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("ampMid", 1), "Amp Mid",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("ampMidFreq", 1), "Amp Mid Freq",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("ampTreble", 1), "Amp Treble",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("ampMaster", 1), "Amp Master",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 70.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("ampType", 1), "Amp Type",
        juce::StringArray{ "Clean", "Crunch", "Lead", "Modern", "Vintage" }, 0));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("ampBypass", 1), "Amp Bypass", false));

    // === MODULATION ===
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("modRate", 1), "Mod Rate",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("modDepth", 1), "Mod Depth",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("modBlend", 1), "Mod Blend",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("modType", 1), "Mod Type",
        juce::StringArray{ "Phaser", "Flanger", "Chorus", "Tremolo", "Vibrato" }, 2));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("modBypass", 1), "Mod Bypass", true));

    // === REVERB ===
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("revDecay", 1), "Rev Decay",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("revTone", 1), "Rev Tone",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("revBlend", 1), "Rev Blend",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 30.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("revType", 1), "Rev Type",
        juce::StringArray{ "Spring", "Plate", "Hall" }, 0));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("revBypass", 1), "Rev Bypass", true));

    return { params.begin(), params.end() };
}

const juce::String PDLBRDAudioProcessor::getName() const { return JucePlugin_Name; }
bool PDLBRDAudioProcessor::acceptsMidi() const { return false; }
bool PDLBRDAudioProcessor::producesMidi() const { return false; }
bool PDLBRDAudioProcessor::isMidiEffect() const { return false; }
double PDLBRDAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int PDLBRDAudioProcessor::getNumPrograms() { return 1; }
int PDLBRDAudioProcessor::getCurrentProgram() { return 0; }
void PDLBRDAudioProcessor::setCurrentProgram(int index) { juce::ignoreUnused(index); }
const juce::String PDLBRDAudioProcessor::getProgramName(int index) { juce::ignoreUnused(index); return {}; }
void PDLBRDAudioProcessor::changeProgramName(int index, const juce::String& newName) { juce::ignoreUnused(index, newName); }

void PDLBRDAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    compressor1.prepare(sampleRate, samplesPerBlock);
    compressor2.prepare(sampleRate, samplesPerBlock);
    distortion.prepare(sampleRate, samplesPerBlock);
    ampSim.prepare(sampleRate, samplesPerBlock);
    modulation.prepare(sampleRate, samplesPerBlock);
    reverb.prepare(sampleRate, samplesPerBlock);
}

void PDLBRDAudioProcessor::releaseResources()
{
    compressor1.reset();
    compressor2.reset();
    distortion.reset();
    ampSim.reset();
    modulation.reset();
    reverb.reset();
}

bool PDLBRDAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    return true;
}

void PDLBRDAudioProcessor::setEffectOrder(const std::array<int, NUM_EFFECTS>& newOrder)
{
    effectOrder = newOrder;
}

void PDLBRDAudioProcessor::moveEffectUp(int effectId)
{
    for (int i = 1; i < NUM_EFFECTS; ++i)
    {
        if (effectOrder[i] == effectId)
        {
            std::swap(effectOrder[i], effectOrder[i - 1]);
            return;
        }
    }
}

void PDLBRDAudioProcessor::moveEffectDown(int effectId)
{
    for (int i = 0; i < NUM_EFFECTS - 1; ++i)
    {
        if (effectOrder[i] == effectId)
        {
            std::swap(effectOrder[i], effectOrder[i + 1]);
            return;
        }
    }
}

void PDLBRDAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Measure input level
    float inLevel = 0.0f;
    for (int ch = 0; ch < totalNumInputChannels; ++ch)
        inLevel = std::max(inLevel, buffer.getMagnitude(ch, 0, buffer.getNumSamples()));
    inputLevel.store(inLevel);

    // Update compressor 1
    compressor1.setThreshold(comp1Threshold->load());
    compressor1.setRatio(comp1Ratio->load());
    compressor1.setAttack(comp1Attack->load());
    compressor1.setRelease(comp1Release->load());
    compressor1.setMakeupGain(comp1Makeup->load());
    compressor1.setBlend(comp1Blend->load() / 100.0f);
    compressor1.setBypass(comp1Bypass->load() > 0.5f);

    // Update compressor 2
    compressor2.setThreshold(comp2Threshold->load());
    compressor2.setRatio(comp2Ratio->load());
    compressor2.setAttack(comp2Attack->load());
    compressor2.setRelease(comp2Release->load());
    compressor2.setMakeupGain(comp2Makeup->load());
    compressor2.setBlend(comp2Blend->load() / 100.0f);
    compressor2.setBypass(comp2Bypass->load() > 0.5f);

    // Update distortion
    distortion.setDrive(distDrive->load());
    distortion.setTone(distTone->load());
    distortion.setLevel(distLevel->load());
    distortion.setType(static_cast<int>(distType->load()));
    distortion.setBypass(distBypass->load() > 0.5f);

    // Update amp sim
    ampSim.setGain(ampGain->load());
    ampSim.setBass(ampBass->load());
    ampSim.setMid(ampMid->load());
    ampSim.setMidFreq(ampMidFreq->load());
    ampSim.setTreble(ampTreble->load());
    ampSim.setMaster(ampMaster->load());
    ampSim.setType(static_cast<int>(ampType->load()));
    ampSim.setBypass(ampBypass->load() > 0.5f);

    // Update modulation
    modulation.setRate(modRate->load());
    modulation.setDepth(modDepth->load());
    modulation.setBlend(modBlend->load());
    modulation.setType(static_cast<int>(modType->load()));
    modulation.setBypass(modBypass->load() > 0.5f);

    // Update reverb
    reverb.setDecay(revDecay->load());
    reverb.setTone(revTone->load());
    reverb.setBlend(revBlend->load());
    reverb.setType(static_cast<int>(revType->load()));
    reverb.setBypass(revBypass->load() > 0.5f);

    // Process effects in order
    for (int i = 0; i < NUM_EFFECTS; ++i)
    {
        effects[effectOrder[i]]->process(buffer);
    }

    // Measure output level
    float outLevel = 0.0f;
    for (int ch = 0; ch < totalNumInputChannels; ++ch)
        outLevel = std::max(outLevel, buffer.getMagnitude(ch, 0, buffer.getNumSamples()));
    outputLevel.store(outLevel);
}

void PDLBRDAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();

    // Save effect order
    juce::String orderStr;
    for (int i = 0; i < NUM_EFFECTS; ++i)
    {
        if (i > 0) orderStr += ",";
        orderStr += juce::String(effectOrder[i]);
    }
    state.setProperty("effectOrder", orderStr, nullptr);

    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PDLBRDAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
    {
        auto state = juce::ValueTree::fromXml(*xml);

        // Restore effect order
        if (state.hasProperty("effectOrder"))
        {
            juce::String orderStr = state.getProperty("effectOrder").toString();
            juce::StringArray tokens;
            tokens.addTokens(orderStr, ",", "");
            if (tokens.size() == NUM_EFFECTS)
            {
                for (int i = 0; i < NUM_EFFECTS; ++i)
                {
                    effectOrder[i] = tokens[i].getIntValue();
                }
            }
        }

        apvts.replaceState(state);
    }
}

juce::AudioProcessorEditor* PDLBRDAudioProcessor::createEditor()
{
    return new PDLBRDAudioProcessorEditor(*this);
}

bool PDLBRDAudioProcessor::hasEditor() const { return true; }

//==============================================================================
// Preset Management
//==============================================================================
juce::File PDLBRDAudioProcessor::getPresetsFolder()
{
    auto folder = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                      .getChildFile("Audio/Presets/PDLBRD");
    if (!folder.exists())
        folder.createDirectory();
    return folder;
}

juce::StringArray PDLBRDAudioProcessor::getPresetList()
{
    juce::StringArray presets;
    auto folder = getPresetsFolder();
    auto files = folder.findChildFiles(juce::File::findFiles, false, "*.pdlbrd");

    for (auto& file : files)
        presets.add(file.getFileNameWithoutExtension());

    presets.sort(true);
    return presets;
}

juce::String PDLBRDAudioProcessor::exportPresetToString()
{
    juce::DynamicObject::Ptr preset = new juce::DynamicObject();

    preset->setProperty("name", "Untitled");
    preset->setProperty("version", "1.0");

    // Effect order
    juce::Array<juce::var> orderArray;
    for (int i = 0; i < NUM_EFFECTS; ++i)
        orderArray.add(effectOrder[i]);
    preset->setProperty("effectOrder", orderArray);

    // All parameters
    juce::DynamicObject::Ptr params = new juce::DynamicObject();
    for (auto* param : getParameters())
    {
        if (auto* p = dynamic_cast<juce::RangedAudioParameter*>(param))
            params->setProperty(p->getParameterID(), p->getValue());
    }
    preset->setProperty("parameters", params.get());

    return juce::JSON::toString(preset.get(), true);
}

void PDLBRDAudioProcessor::importPresetFromString(const juce::String& presetData)
{
    auto parsed = juce::JSON::parse(presetData);
    if (parsed.isVoid())
        return;

    auto* preset = parsed.getDynamicObject();
    if (preset == nullptr)
        return;

    // Load effect order
    if (preset->hasProperty("effectOrder"))
    {
        auto orderArray = preset->getProperty("effectOrder").getArray();
        if (orderArray != nullptr && orderArray->size() == NUM_EFFECTS)
        {
            for (int i = 0; i < NUM_EFFECTS; ++i)
                effectOrder[i] = (*orderArray)[i];
        }
    }

    // Load parameters
    if (preset->hasProperty("parameters"))
    {
        auto* params = preset->getProperty("parameters").getDynamicObject();
        if (params != nullptr)
        {
            for (auto* param : getParameters())
            {
                if (auto* p = dynamic_cast<juce::RangedAudioParameter*>(param))
                {
                    if (params->hasProperty(p->getParameterID()))
                    {
                        float value = params->getProperty(p->getParameterID());
                        p->setValueNotifyingHost(value);
                    }
                }
            }
        }
    }
}

void PDLBRDAudioProcessor::savePreset(const juce::File& file)
{
    auto presetString = exportPresetToString();

    // Update the name in the JSON
    auto parsed = juce::JSON::parse(presetString);
    if (auto* preset = parsed.getDynamicObject())
    {
        preset->setProperty("name", file.getFileNameWithoutExtension());
        presetString = juce::JSON::toString(preset, true);
    }

    file.replaceWithText(presetString);
}

void PDLBRDAudioProcessor::loadPreset(const juce::File& file)
{
    if (!file.existsAsFile())
        return;

    auto presetData = file.loadFileAsString();
    importPresetFromString(presetData);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PDLBRDAudioProcessor();
}
