#include "PluginProcessor.h"
#include "PluginEditor.h"

PDLBRDAudioProcessor::PDLBRDAudioProcessor()
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
       apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    // Get parameter pointers for real-time access
    compThreshold = apvts.getRawParameterValue("compThreshold");
    compRatio = apvts.getRawParameterValue("compRatio");
    compAttack = apvts.getRawParameterValue("compAttack");
    compRelease = apvts.getRawParameterValue("compRelease");
    compMakeup = apvts.getRawParameterValue("compMakeup");
    compBlend = apvts.getRawParameterValue("compBlend");
    compBypass = apvts.getRawParameterValue("compBypass");
}

PDLBRDAudioProcessor::~PDLBRDAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout PDLBRDAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Compressor parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("compThreshold", 1),
        "Comp Threshold",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f),
        -20.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("compRatio", 1),
        "Comp Ratio",
        juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f, 0.5f), // skew for better control
        4.0f,
        juce::AudioParameterFloatAttributes().withLabel(":1")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("compAttack", 1),
        "Comp Attack",
        juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f, 0.4f),
        10.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("compRelease", 1),
        "Comp Release",
        juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f, 0.4f),
        100.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("compMakeup", 1),
        "Comp Makeup",
        juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("compBlend", 1),
        "Comp Blend",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID("compBypass", 1),
        "Comp Bypass",
        false));

    return { params.begin(), params.end() };
}

const juce::String PDLBRDAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PDLBRDAudioProcessor::acceptsMidi() const
{
    return false;
}

bool PDLBRDAudioProcessor::producesMidi() const
{
    return false;
}

bool PDLBRDAudioProcessor::isMidiEffect() const
{
    return false;
}

double PDLBRDAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PDLBRDAudioProcessor::getNumPrograms()
{
    return 1;
}

int PDLBRDAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PDLBRDAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String PDLBRDAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void PDLBRDAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void PDLBRDAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    compressor.prepare(sampleRate, samplesPerBlock);
}

void PDLBRDAudioProcessor::releaseResources()
{
    compressor.reset();
}

bool PDLBRDAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void PDLBRDAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Update compressor parameters from APVTS
    compressor.setThreshold(compThreshold->load());
    compressor.setRatio(compRatio->load());
    compressor.setAttack(compAttack->load());
    compressor.setRelease(compRelease->load());
    compressor.setMakeupGain(compMakeup->load());
    compressor.setBlend(compBlend->load() / 100.0f);  // Convert from % to 0-1
    compressor.setBypass(compBypass->load() > 0.5f);

    // Process through compressor
    compressor.process(buffer);
}

void PDLBRDAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PDLBRDAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorEditor* PDLBRDAudioProcessor::createEditor()
{
    return new PDLBRDAudioProcessorEditor (*this);
}

bool PDLBRDAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PDLBRDAudioProcessor();
}
