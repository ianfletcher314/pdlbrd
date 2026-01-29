#include "Compressor.h"

Compressor::Compressor()
{
}

void Compressor::prepare(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;
    updateCoefficients();
    reset();
}

void Compressor::reset()
{
    envelopeL = 0.0f;
    envelopeR = 0.0f;
    currentGainReduction = 0.0f;
}

void Compressor::updateCoefficients()
{
    attackCoeff = DSPUtils::calculateCoefficient(currentSampleRate, attackMs);
    releaseCoeff = DSPUtils::calculateCoefficient(currentSampleRate, releaseMs);
    makeupLinear = DSPUtils::decibelsToLinear(makeupGain);
}

void Compressor::setThreshold(float thresholdDb)
{
    threshold = std::clamp(thresholdDb, -60.0f, 0.0f);
}

void Compressor::setRatio(float newRatio)
{
    ratio = std::clamp(newRatio, 1.0f, 20.0f);
}

void Compressor::setAttack(float newAttackMs)
{
    attackMs = std::clamp(newAttackMs, 0.1f, 100.0f);
    updateCoefficients();
}

void Compressor::setRelease(float newReleaseMs)
{
    releaseMs = std::clamp(newReleaseMs, 10.0f, 1000.0f);
    updateCoefficients();
}

void Compressor::setMakeupGain(float gainDb)
{
    makeupGain = std::clamp(gainDb, 0.0f, 24.0f);
    makeupLinear = DSPUtils::decibelsToLinear(makeupGain);
}

void Compressor::setBlend(float newBlend)
{
    blend = std::clamp(newBlend, 0.0f, 1.0f);
}

float Compressor::processSample(float input, float& envelope)
{
    // Get input level (absolute value for envelope detection)
    float inputLevel = std::abs(input);

    // Envelope follower with attack/release
    if (inputLevel > envelope)
        envelope += attackCoeff * (inputLevel - envelope);
    else
        envelope += releaseCoeff * (inputLevel - envelope);

    // Convert to dB for gain calculation
    float inputDb = DSPUtils::linearToDecibels(envelope);

    // Calculate gain reduction
    float gainReductionDb = 0.0f;
    if (inputDb > threshold)
    {
        // Amount over threshold
        float overDb = inputDb - threshold;
        // Gain reduction based on ratio
        gainReductionDb = overDb * (1.0f - 1.0f / ratio);
    }

    // Store for metering
    currentGainReduction = gainReductionDb;

    // Convert gain reduction to linear
    float gainLinear = DSPUtils::decibelsToLinear(-gainReductionDb);

    // Apply compression
    float compressed = input * gainLinear * makeupLinear;

    // Parallel compression (wet/dry blend)
    return input * (1.0f - blend) + compressed * blend;
}

void Compressor::process(juce::AudioBuffer<float>& buffer)
{
    if (bypassed)
        return;

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    if (numChannels == 0)
        return;

    // Process left channel (or mono)
    float* leftChannel = buffer.getWritePointer(0);
    for (int i = 0; i < numSamples; ++i)
    {
        leftChannel[i] = processSample(leftChannel[i], envelopeL);
    }

    // Process right channel if stereo
    if (numChannels > 1)
    {
        float* rightChannel = buffer.getWritePointer(1);
        for (int i = 0; i < numSamples; ++i)
        {
            rightChannel[i] = processSample(rightChannel[i], envelopeR);
        }
    }
}
