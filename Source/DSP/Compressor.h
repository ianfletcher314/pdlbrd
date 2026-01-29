#pragma once

#include "EffectModule.h"
#include "DSPUtils.h"

class Compressor : public EffectModule
{
public:
    Compressor();

    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;

    // Parameters (all normalized 0-1 for now, will map to real values)
    void setThreshold(float thresholdDb);      // -60 to 0 dB
    void setRatio(float ratio);                 // 1 to 20
    void setAttack(float attackMs);             // 0.1 to 100 ms
    void setRelease(float releaseMs);           // 10 to 1000 ms
    void setMakeupGain(float gainDb);           // 0 to 24 dB
    void setBlend(float blend);                 // 0 to 1 (parallel compression)

    float getGainReduction() const { return currentGainReduction; }

private:
    float processSample(float input, float& envelope);
    void updateCoefficients();

    // Parameters
    float threshold = -20.0f;    // dB
    float ratio = 4.0f;          // :1
    float attackMs = 10.0f;      // ms
    float releaseMs = 100.0f;    // ms
    float makeupGain = 0.0f;     // dB
    float blend = 1.0f;          // wet/dry mix

    // Coefficients (calculated from parameters)
    float attackCoeff = 0.0f;
    float releaseCoeff = 0.0f;
    float makeupLinear = 1.0f;

    // State
    float envelopeL = 0.0f;
    float envelopeR = 0.0f;
    float currentGainReduction = 0.0f;
};
