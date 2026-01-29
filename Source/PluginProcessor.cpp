#include "PluginProcessor.h"
#include "PluginEditor.h"

PDLBRDAudioProcessor::PDLBRDAudioProcessor()
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
}

PDLBRDAudioProcessor::~PDLBRDAudioProcessor()
{
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
    juce::ignoreUnused (sampleRate, samplesPerBlock);
}

void PDLBRDAudioProcessor::releaseResources()
{
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

    // Pass-through for now - audio goes in and out unchanged
    // This is where DSP processing will be added
}

void PDLBRDAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::ignoreUnused (destData);
}

void PDLBRDAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    juce::ignoreUnused (data, sizeInBytes);
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
