#pragma once

#include "EffectModule.h"
#include "DSPUtils.h"

enum class AmpType
{
    Clean = 0,    // Fender-style clean
    Crunch,       // Marshall-style edge of breakup
    Lead,         // High-gain lead
    Modern,       // Mesa-style tight high-gain
    Vintage       // Vox-style chimey breakup
};

class AmpSim : public EffectModule
{
public:
    AmpSim();

    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;

    void setGain(float gain);           // 0-100
    void setBass(float bass);           // 0-100 (maps to -12 to +12 dB)
    void setMid(float mid);             // 0-100
    void setMidFreq(float freq);        // 0-100 (maps to 200-2000 Hz)
    void setTreble(float treble);       // 0-100
    void setMaster(float master);       // 0-100
    void setType(int type);             // 0-4 for AmpType

private:
    float processSample(float input, int channel);
    float applyPreampSaturation(float input);
    void updateFilters();

    // Parameters
    float gain = 30.0f;
    float bass = 50.0f;
    float mid = 50.0f;
    float midFreq = 50.0f;
    float treble = 50.0f;
    float master = 70.0f;
    AmpType type = AmpType::Clean;

    // Derived values
    float preampGain = 1.0f;
    float masterGain = 1.0f;
    float bassGainDb = 0.0f;
    float midGainDb = 0.0f;
    float trebleGainDb = 0.0f;
    float midFreqHz = 800.0f;

    // EQ filter states (simple biquad coefficients)
    // Bass (low shelf)
    float bassB0 = 1.0f, bassB1 = 0.0f, bassB2 = 0.0f;
    float bassA1 = 0.0f, bassA2 = 0.0f;
    float bassZ1L = 0.0f, bassZ2L = 0.0f;
    float bassZ1R = 0.0f, bassZ2R = 0.0f;

    // Mid (peaking)
    float midB0 = 1.0f, midB1 = 0.0f, midB2 = 0.0f;
    float midA1 = 0.0f, midA2 = 0.0f;
    float midZ1L = 0.0f, midZ2L = 0.0f;
    float midZ1R = 0.0f, midZ2R = 0.0f;

    // Treble (high shelf)
    float trebB0 = 1.0f, trebB1 = 0.0f, trebB2 = 0.0f;
    float trebA1 = 0.0f, trebA2 = 0.0f;
    float trebZ1L = 0.0f, trebZ2L = 0.0f;
    float trebZ1R = 0.0f, trebZ2R = 0.0f;

    // Cabinet sim (lowpass)
    float cabCoeffL = 0.0f, cabStateL = 0.0f;
    float cabCoeffR = 0.0f, cabStateR = 0.0f;

    // High pass for rumble removal
    float hpCoeff = 0.0f;
    float hpStateL = 0.0f, hpPrevL = 0.0f;
    float hpStateR = 0.0f, hpPrevR = 0.0f;
};
