#pragma once

#include "EffectModule.h"
#include <vector>

enum class ModulationType
{
    Phaser = 0,
    Flanger,
    Chorus,
    Tremolo,
    Vibrato
};

class Modulation : public EffectModule
{
public:
    Modulation();

    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;

    void setRate(float rate);       // 0-100 (maps to Hz based on type)
    void setDepth(float depth);     // 0-100
    void setBlend(float blend);     // 0-100
    void setType(int type);         // 0-4 for ModulationType

private:
    // LFO
    float getLFOValue();
    void advanceLFO();

    // Processing per type
    float processPhaser(float input, int channel);
    float processFlanger(float input, int channel);
    float processChorus(float input, int channel);
    float processTremolo(float input, int channel);
    float processVibrato(float input, int channel);

    // Delay line helpers
    void writeToDelayLine(float sample, int channel);
    float readFromDelayLine(float delayMs, int channel);

    // Parameters
    float rate = 50.0f;
    float depth = 50.0f;
    float blend = 50.0f;
    ModulationType type = ModulationType::Chorus;

    // Derived
    float lfoFreq = 1.0f;
    float depthAmount = 0.5f;
    float blendAmount = 0.5f;

    // LFO state
    float lfoPhase = 0.0f;
    float lfoIncrement = 0.0f;

    // Delay lines (circular buffers)
    static const int MAX_DELAY_SAMPLES = 4410;  // ~100ms at 44.1kHz
    std::vector<float> delayLineL;
    std::vector<float> delayLineR;
    int writeIndex = 0;

    // Phaser allpass filter states (6 stages)
    float allpassStateL[6] = {0};
    float allpassStateR[6] = {0};

    // Flanger feedback
    float flangerFeedbackL = 0.0f;
    float flangerFeedbackR = 0.0f;
};
