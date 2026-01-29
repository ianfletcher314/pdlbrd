#include "PluginProcessor.h"
#include "PluginEditor.h"

PDLBRDAudioProcessor::PDLBRDAudioProcessor()
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
       apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    // Compressor
    compThreshold = apvts.getRawParameterValue("compThreshold");
    compRatio = apvts.getRawParameterValue("compRatio");
    compAttack = apvts.getRawParameterValue("compAttack");
    compRelease = apvts.getRawParameterValue("compRelease");
    compMakeup = apvts.getRawParameterValue("compMakeup");
    compBlend = apvts.getRawParameterValue("compBlend");
    compBypass = apvts.getRawParameterValue("compBypass");

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

    // === COMPRESSOR ===
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("compThreshold", 1), "Comp Threshold",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -20.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("compRatio", 1), "Comp Ratio",
        juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f, 0.5f), 4.0f,
        juce::AudioParameterFloatAttributes().withLabel(":1")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("compAttack", 1), "Comp Attack",
        juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f, 0.4f), 10.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("compRelease", 1), "Comp Release",
        juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f, 0.4f), 100.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("compMakeup", 1), "Comp Makeup",
        juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("compBlend", 1), "Comp Blend",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("compBypass", 1), "Comp Bypass", false));

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
        juce::ParameterID("distBypass", 1), "Dist Bypass", false));

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
        juce::ParameterID("modBypass", 1), "Mod Bypass", false));

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
        juce::ParameterID("revBypass", 1), "Rev Bypass", false));

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
    compressor.prepare(sampleRate, samplesPerBlock);
    distortion.prepare(sampleRate, samplesPerBlock);
    ampSim.prepare(sampleRate, samplesPerBlock);
    modulation.prepare(sampleRate, samplesPerBlock);
    reverb.prepare(sampleRate, samplesPerBlock);
}

void PDLBRDAudioProcessor::releaseResources()
{
    compressor.reset();
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

void PDLBRDAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Update compressor
    compressor.setThreshold(compThreshold->load());
    compressor.setRatio(compRatio->load());
    compressor.setAttack(compAttack->load());
    compressor.setRelease(compRelease->load());
    compressor.setMakeupGain(compMakeup->load());
    compressor.setBlend(compBlend->load() / 100.0f);
    compressor.setBypass(compBypass->load() > 0.5f);

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

    // Signal chain: Compressor -> Distortion -> Amp Sim -> Modulation -> Reverb
    compressor.process(buffer);
    distortion.process(buffer);
    ampSim.process(buffer);
    modulation.process(buffer);
    reverb.process(buffer);
}

void PDLBRDAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PDLBRDAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorEditor* PDLBRDAudioProcessor::createEditor()
{
    return new PDLBRDAudioProcessorEditor(*this);
}

bool PDLBRDAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PDLBRDAudioProcessor();
}
