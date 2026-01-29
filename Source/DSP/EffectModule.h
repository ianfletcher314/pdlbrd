#pragma once

#include <JuceHeader.h>

class EffectModule
{
public:
    virtual ~EffectModule() = default;

    virtual void prepare(double sampleRate, int samplesPerBlock) = 0;
    virtual void process(juce::AudioBuffer<float>& buffer) = 0;
    virtual void reset() = 0;

    void setBypass(bool shouldBypass) { bypassed = shouldBypass; }
    bool isBypassed() const { return bypassed; }

protected:
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;
    bool bypassed = false;
};
