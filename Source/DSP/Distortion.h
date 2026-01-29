#pragma once

#include "EffectModule.h"
#include "DSPUtils.h"

enum class DistortionType
{
    TS9 = 0,    // Tube Screamer - mid-focused overdrive
    RAT,        // ProCo RAT - hard clipping
    Blues,      // Blues Breaker - transparent overdrive
    Fuzz,       // Fuzz Face - germanium fuzz
    Muff        // Big Muff - sustaining fuzz
};

class Distortion : public EffectModule
{
public:
    Distortion();

    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;

    void setDrive(float drive);         // 0 to 100
    void setTone(float tone);           // 0 to 100
    void setLevel(float level);         // 0 to 100
    void setType(int type);             // 0-4 for DistortionType

private:
    float processSample(float input);
    float applyDistortion(float input, float k);
    void updateFilter();

    // Parameters
    float drive = 50.0f;     // 0-100
    float tone = 50.0f;      // 0-100
    float level = 50.0f;     // 0-100
    DistortionType type = DistortionType::TS9;

    // Derived values
    float driveAmount = 1.0f;
    float levelGain = 1.0f;

    // Tone filter state (simple one-pole lowpass)
    float filterCoeff = 0.5f;
    float filterStateL = 0.0f;
    float filterStateR = 0.0f;
};
