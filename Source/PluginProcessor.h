#pragma once

#include <JuceHeader.h>
#include "DSP/Compressor.h"
#include "DSP/Distortion.h"
#include "DSP/AmpSim.h"
#include "DSP/Modulation.h"
#include "DSP/Reverb.h"

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
    float getCompressor1GainReduction() const { return compressor1.getGainReduction(); }
    float getCompressor2GainReduction() const { return compressor2.getGainReduction(); }

    // Level metering
    float getInputLevel() const { return inputLevel.load(); }
    float getOutputLevel() const { return outputLevel.load(); }

    // Signal chain ordering
    static constexpr int NUM_EFFECTS = 6;
    enum EffectID { COMP1 = 0, DIST, AMP, MOD, REV, COMP2 };
    const std::array<juce::String, NUM_EFFECTS> effectNames = { "Comp1", "Dist", "Amp", "Mod", "Rev", "Comp2" };

    std::array<int, NUM_EFFECTS> getEffectOrder() const { return effectOrder; }
    void setEffectOrder(const std::array<int, NUM_EFFECTS>& newOrder);
    void moveEffectUp(int effectId);
    void moveEffectDown(int effectId);

private:
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // DSP
    Compressor compressor1;
    Compressor compressor2;
    Distortion distortion;
    AmpSim ampSim;
    Modulation modulation;
    Reverb reverb;

    // Signal chain order (indices into effect array)
    std::array<int, NUM_EFFECTS> effectOrder = { COMP1, DIST, AMP, MOD, REV, COMP2 };
    std::array<EffectModule*, NUM_EFFECTS> effects;

    // Compressor 1
    std::atomic<float>* comp1Threshold = nullptr;
    std::atomic<float>* comp1Ratio = nullptr;
    std::atomic<float>* comp1Attack = nullptr;
    std::atomic<float>* comp1Release = nullptr;
    std::atomic<float>* comp1Makeup = nullptr;
    std::atomic<float>* comp1Blend = nullptr;
    std::atomic<float>* comp1Bypass = nullptr;

    // Compressor 2
    std::atomic<float>* comp2Threshold = nullptr;
    std::atomic<float>* comp2Ratio = nullptr;
    std::atomic<float>* comp2Attack = nullptr;
    std::atomic<float>* comp2Release = nullptr;
    std::atomic<float>* comp2Makeup = nullptr;
    std::atomic<float>* comp2Blend = nullptr;
    std::atomic<float>* comp2Bypass = nullptr;

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

    // Reverb
    std::atomic<float>* revDecay = nullptr;
    std::atomic<float>* revTone = nullptr;
    std::atomic<float>* revBlend = nullptr;
    std::atomic<float>* revType = nullptr;
    std::atomic<float>* revBypass = nullptr;

    // Level metering
    std::atomic<float> inputLevel { 0.0f };
    std::atomic<float> outputLevel { 0.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PDLBRDAudioProcessor)
};
