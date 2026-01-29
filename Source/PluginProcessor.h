#pragma once

#include <JuceHeader.h>
#include "DSP/Compressor.h"
#include "DSP/Distortion.h"
#include "DSP/AmpSim.h"
#include "DSP/Modulation.h"

class PDLBRDAudioProcessor : public juce::AudioProcessor
{
public:
    PDLBRDAudioProcessor();
    ~PDLBRDAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    float getCompressorGainReduction() const { return compressor.getGainReduction(); }

private:
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // DSP
    Compressor compressor;
    Distortion distortion;
    AmpSim ampSim;
    Modulation modulation;

    // Compressor
    std::atomic<float>* compThreshold = nullptr;
    std::atomic<float>* compRatio = nullptr;
    std::atomic<float>* compAttack = nullptr;
    std::atomic<float>* compRelease = nullptr;
    std::atomic<float>* compMakeup = nullptr;
    std::atomic<float>* compBlend = nullptr;
    std::atomic<float>* compBypass = nullptr;

    // Distortion
    std::atomic<float>* distDrive = nullptr;
    std::atomic<float>* distTone = nullptr;
    std::atomic<float>* distLevel = nullptr;
    std::atomic<float>* distType = nullptr;
    std::atomic<float>* distBypass = nullptr;

    // Amp Sim
    std::atomic<float>* ampGain = nullptr;
    std::atomic<float>* ampBass = nullptr;
    std::atomic<float>* ampMid = nullptr;
    std::atomic<float>* ampMidFreq = nullptr;
    std::atomic<float>* ampTreble = nullptr;
    std::atomic<float>* ampMaster = nullptr;
    std::atomic<float>* ampType = nullptr;
    std::atomic<float>* ampBypass = nullptr;

    // Modulation
    std::atomic<float>* modRate = nullptr;
    std::atomic<float>* modDepth = nullptr;
    std::atomic<float>* modBlend = nullptr;
    std::atomic<float>* modType = nullptr;
    std::atomic<float>* modBypass = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PDLBRDAudioProcessor)
};
