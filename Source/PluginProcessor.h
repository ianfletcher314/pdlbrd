#pragma once

#include <JuceHeader.h>
#include "DSP/Compressor.h"
#include "DSP/Distortion.h"

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

    // Parameter state
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    // For UI metering
    float getCompressorGainReduction() const { return compressor.getGainReduction(); }

private:
    // Parameters
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // DSP
    Compressor compressor;
    Distortion distortion;

    // Compressor parameter pointers
    std::atomic<float>* compThreshold = nullptr;
    std::atomic<float>* compRatio = nullptr;
    std::atomic<float>* compAttack = nullptr;
    std::atomic<float>* compRelease = nullptr;
    std::atomic<float>* compMakeup = nullptr;
    std::atomic<float>* compBlend = nullptr;
    std::atomic<float>* compBypass = nullptr;

    // Distortion parameter pointers
    std::atomic<float>* distDrive = nullptr;
    std::atomic<float>* distTone = nullptr;
    std::atomic<float>* distLevel = nullptr;
    std::atomic<float>* distType = nullptr;
    std::atomic<float>* distBypass = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PDLBRDAudioProcessor)
};
