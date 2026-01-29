#include "AmpSim.h"
#include <cmath>

AmpSim::AmpSim()
{
}

void AmpSim::prepare(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;
    updateFilters();
    reset();
}

void AmpSim::reset()
{
    bassZ1L = bassZ2L = bassZ1R = bassZ2R = 0.0f;
    midZ1L = midZ2L = midZ1R = midZ2R = 0.0f;
    trebZ1L = trebZ2L = trebZ1R = trebZ2R = 0.0f;
    cabStateL = cabStateR = 0.0f;
    hpStateL = hpPrevL = hpStateR = hpPrevR = 0.0f;
}

void AmpSim::setGain(float newGain)
{
    gain = std::clamp(newGain, 0.0f, 100.0f);
    // Map 0-100 to useful preamp gain range
    preampGain = 0.5f + (gain / 100.0f) * 15.0f;
}

void AmpSim::setBass(float newBass)
{
    bass = std::clamp(newBass, 0.0f, 100.0f);
    bassGainDb = (bass - 50.0f) * 0.24f;  // -12 to +12 dB
    updateFilters();
}

void AmpSim::setMid(float newMid)
{
    mid = std::clamp(newMid, 0.0f, 100.0f);
    midGainDb = (mid - 50.0f) * 0.24f;
    updateFilters();
}

void AmpSim::setMidFreq(float newFreq)
{
    midFreq = std::clamp(newFreq, 0.0f, 100.0f);
    // Map 0-100 to 200-2000 Hz logarithmically
    midFreqHz = 200.0f * std::pow(10.0f, midFreq / 100.0f);
    updateFilters();
}

void AmpSim::setTreble(float newTreble)
{
    treble = std::clamp(newTreble, 0.0f, 100.0f);
    trebleGainDb = (treble - 50.0f) * 0.24f;
    updateFilters();
}

void AmpSim::setMaster(float newMaster)
{
    master = std::clamp(newMaster, 0.0f, 100.0f);
    masterGain = (master / 100.0f) * (master / 100.0f);  // Quadratic taper
}

void AmpSim::setType(int newType)
{
    type = static_cast<AmpType>(std::clamp(newType, 0, 4));
    updateFilters();
}

void AmpSim::updateFilters()
{
    const float sr = static_cast<float>(currentSampleRate);
    const float pi = 3.14159265f;

    // === Bass Low Shelf (100 Hz) ===
    {
        float freq = 100.0f;
        float A = std::pow(10.0f, bassGainDb / 40.0f);
        float w0 = 2.0f * pi * freq / sr;
        float cosw0 = std::cos(w0);
        float sinw0 = std::sin(w0);
        float alpha = sinw0 / 2.0f * std::sqrt((A + 1.0f/A) * (1.0f/0.7f - 1.0f) + 2.0f);

        float a0 = (A + 1.0f) + (A - 1.0f) * cosw0 + 2.0f * std::sqrt(A) * alpha;
        bassB0 = (A * ((A + 1.0f) - (A - 1.0f) * cosw0 + 2.0f * std::sqrt(A) * alpha)) / a0;
        bassB1 = (2.0f * A * ((A - 1.0f) - (A + 1.0f) * cosw0)) / a0;
        bassB2 = (A * ((A + 1.0f) - (A - 1.0f) * cosw0 - 2.0f * std::sqrt(A) * alpha)) / a0;
        bassA1 = (-2.0f * ((A - 1.0f) + (A + 1.0f) * cosw0)) / a0;
        bassA2 = ((A + 1.0f) + (A - 1.0f) * cosw0 - 2.0f * std::sqrt(A) * alpha) / a0;
    }

    // === Mid Peaking (variable frequency) ===
    {
        float freq = midFreqHz;
        float A = std::pow(10.0f, midGainDb / 40.0f);
        float Q = 1.5f;
        float w0 = 2.0f * pi * freq / sr;
        float cosw0 = std::cos(w0);
        float sinw0 = std::sin(w0);
        float alpha = sinw0 / (2.0f * Q);

        float a0 = 1.0f + alpha / A;
        midB0 = (1.0f + alpha * A) / a0;
        midB1 = (-2.0f * cosw0) / a0;
        midB2 = (1.0f - alpha * A) / a0;
        midA1 = (-2.0f * cosw0) / a0;
        midA2 = (1.0f - alpha / A) / a0;
    }

    // === Treble High Shelf (3 kHz) ===
    {
        float freq = 3000.0f;
        float A = std::pow(10.0f, trebleGainDb / 40.0f);
        float w0 = 2.0f * pi * freq / sr;
        float cosw0 = std::cos(w0);
        float sinw0 = std::sin(w0);
        float alpha = sinw0 / 2.0f * std::sqrt((A + 1.0f/A) * (1.0f/0.7f - 1.0f) + 2.0f);

        float a0 = (A + 1.0f) - (A - 1.0f) * cosw0 + 2.0f * std::sqrt(A) * alpha;
        trebB0 = (A * ((A + 1.0f) + (A - 1.0f) * cosw0 + 2.0f * std::sqrt(A) * alpha)) / a0;
        trebB1 = (-2.0f * A * ((A - 1.0f) + (A + 1.0f) * cosw0)) / a0;
        trebB2 = (A * ((A + 1.0f) + (A - 1.0f) * cosw0 - 2.0f * std::sqrt(A) * alpha)) / a0;
        trebA1 = (2.0f * ((A - 1.0f) - (A + 1.0f) * cosw0)) / a0;
        trebA2 = ((A + 1.0f) - (A - 1.0f) * cosw0 - 2.0f * std::sqrt(A) * alpha) / a0;
    }

    // === Cabinet sim lowpass (5.5 kHz) ===
    {
        float cutoff = 5500.0f;
        float w = 2.0f * pi * cutoff / sr;
        cabCoeffL = cabCoeffR = w / (1.0f + w);
    }

    // === High pass for rumble (80 Hz) ===
    {
        float cutoff = 80.0f;
        float RC = 1.0f / (2.0f * pi * cutoff);
        float dt = 1.0f / sr;
        hpCoeff = RC / (RC + dt);
    }
}

float AmpSim::applyPreampSaturation(float input)
{
    float x = input * preampGain;

    switch (type)
    {
        case AmpType::Clean:
        {
            // Fender-style: Very soft saturation
            return std::tanh(x * 0.5f) * 2.0f;
        }

        case AmpType::Crunch:
        {
            // Marshall-style: Asymmetric tube saturation
            if (x >= 0.0f)
                return std::tanh(x * 0.8f);
            else
                return std::tanh(x * 1.2f) * 0.9f;
        }

        case AmpType::Lead:
        {
            // High-gain: Double-stage saturation
            float stage1 = std::tanh(x * 1.5f);
            return std::tanh(stage1 * 2.0f);
        }

        case AmpType::Modern:
        {
            // Mesa-style: Tight, aggressive
            float shaped = std::tanh(x * 2.0f);
            // Add some asymmetry
            if (shaped < 0.0f)
                shaped *= 0.95f;
            return shaped;
        }

        case AmpType::Vintage:
        {
            // Vox-style: Chimey with even harmonics
            float soft = std::tanh(x * 0.7f);
            float even = x * x * 0.2f * (x > 0 ? 1.0f : -1.0f);
            return soft + even * 0.3f;
        }

        default:
            return std::tanh(x);
    }
}

float AmpSim::processSample(float input, int channel)
{
    // Preamp saturation
    float saturated = applyPreampSaturation(input);

    // Apply EQ (biquad filters)
    float* bassZ1 = (channel == 0) ? &bassZ1L : &bassZ1R;
    float* bassZ2 = (channel == 0) ? &bassZ2L : &bassZ2R;
    float* midZ1 = (channel == 0) ? &midZ1L : &midZ1R;
    float* midZ2 = (channel == 0) ? &midZ2L : &midZ2R;
    float* trebZ1 = (channel == 0) ? &trebZ1L : &trebZ1R;
    float* trebZ2 = (channel == 0) ? &trebZ2L : &trebZ2R;

    // Bass
    float bassOut = bassB0 * saturated + *bassZ1;
    *bassZ1 = bassB1 * saturated - bassA1 * bassOut + *bassZ2;
    *bassZ2 = bassB2 * saturated - bassA2 * bassOut;

    // Mid
    float midOut = midB0 * bassOut + *midZ1;
    *midZ1 = midB1 * bassOut - midA1 * midOut + *midZ2;
    *midZ2 = midB2 * bassOut - midA2 * midOut;

    // Treble
    float trebOut = trebB0 * midOut + *trebZ1;
    *trebZ1 = trebB1 * midOut - trebA1 * trebOut + *trebZ2;
    *trebZ2 = trebB2 * midOut - trebA2 * trebOut;

    // Cabinet simulation (lowpass)
    float* cabState = (channel == 0) ? &cabStateL : &cabStateR;
    float cabCoeff = (channel == 0) ? cabCoeffL : cabCoeffR;
    *cabState += cabCoeff * (trebOut - *cabState);
    float cabOut = *cabState;

    // High pass (rumble filter)
    float* hpState = (channel == 0) ? &hpStateL : &hpStateR;
    float* hpPrev = (channel == 0) ? &hpPrevL : &hpPrevR;
    float hpOut = hpCoeff * (*hpState + cabOut - *hpPrev);
    *hpPrev = cabOut;
    *hpState = hpOut;

    // Master volume
    return hpOut * masterGain;
}

void AmpSim::process(juce::AudioBuffer<float>& buffer)
{
    if (bypassed)
        return;

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    if (numChannels == 0)
        return;

    // Process left channel
    float* leftChannel = buffer.getWritePointer(0);
    for (int i = 0; i < numSamples; ++i)
    {
        leftChannel[i] = processSample(leftChannel[i], 0);
    }

    // Process right channel if stereo
    if (numChannels > 1)
    {
        float* rightChannel = buffer.getWritePointer(1);
        for (int i = 0; i < numSamples; ++i)
        {
            rightChannel[i] = processSample(rightChannel[i], 1);
        }
    }
}
