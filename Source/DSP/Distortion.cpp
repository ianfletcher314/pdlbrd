#include "Distortion.h"
#include <cmath>

Distortion::Distortion()
{
}

void Distortion::prepare(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;
    updateFilter();
    reset();
}

void Distortion::reset()
{
    filterStateL = 0.0f;
    filterStateR = 0.0f;
}

void Distortion::setDrive(float newDrive)
{
    drive = std::clamp(newDrive, 0.0f, 100.0f);
    // Map 0-100 to useful drive range (1 to 50)
    driveAmount = 1.0f + (drive / 100.0f) * 49.0f;
}

void Distortion::setTone(float newTone)
{
    tone = std::clamp(newTone, 0.0f, 100.0f);
    updateFilter();
}

void Distortion::setLevel(float newLevel)
{
    level = std::clamp(newLevel, 0.0f, 100.0f);
    // Map 0-100 to 0.0 to 1.5 output gain
    levelGain = (level / 100.0f) * 1.5f;
}

void Distortion::setType(int newType)
{
    type = static_cast<DistortionType>(std::clamp(newType, 0, 4));
    updateFilter();
}

void Distortion::updateFilter()
{
    // Different cutoff frequencies per distortion type
    float baseCutoff;
    switch (type)
    {
        case DistortionType::TS9:   baseCutoff = 3000.0f; break;  // Mid-focused
        case DistortionType::RAT:   baseCutoff = 5000.0f; break;  // Wider range
        case DistortionType::Blues: baseCutoff = 6000.0f; break;  // Brighter
        case DistortionType::Fuzz:  baseCutoff = 2500.0f; break;  // Vintage dark
        case DistortionType::Muff:  baseCutoff = 4000.0f; break;  // Variable
        default:                    baseCutoff = 4000.0f; break;
    }

    // Tone control adjusts cutoff: 0% = dark, 100% = bright
    float cutoff = baseCutoff * (0.2f + (tone / 100.0f) * 1.5f);
    cutoff = std::clamp(cutoff, 200.0f, 16000.0f);

    // One-pole lowpass coefficient
    float w = 2.0f * 3.14159f * cutoff / static_cast<float>(currentSampleRate);
    filterCoeff = w / (1.0f + w);
}

float Distortion::applyDistortion(float input, float k)
{
    float x = input * k;

    switch (type)
    {
        case DistortionType::TS9:
        {
            // Tube Screamer - asymmetric soft clipping
            if (x >= 0.0f)
                return std::tanh(0.7f * x) / 0.7f;
            else
                return std::tanh(x);
        }

        case DistortionType::RAT:
        {
            // RAT - hard clipping with slight softness
            float threshold = 0.7f;
            if (std::abs(x) < threshold)
                return x;
            else
            {
                float sign = (x > 0.0f) ? 1.0f : -1.0f;
                float excess = std::abs(x) - threshold;
                return sign * (threshold + std::tanh(excess * 2.0f) * 0.3f);
            }
        }

        case DistortionType::Blues:
        {
            // Blues Breaker - very soft, transparent
            return (3.0f * x) / (1.0f + 2.0f * std::abs(x));
        }

        case DistortionType::Fuzz:
        {
            // Fuzz Face - germanium asymmetric
            if (x >= 0.0f)
            {
                // Soft positive clipping
                return std::tanh(x * 1.2f);
            }
            else
            {
                // Harder negative clipping (germanium character)
                float clipped = std::tanh(x * 2.0f);
                return clipped * 0.8f;
            }
        }

        case DistortionType::Muff:
        {
            // Big Muff - sustaining fuzz, squared waveform
            float soft = std::tanh(x);
            float hard = DSPUtils::hardClip(x * 1.5f, 1.0f);
            // Blend based on drive
            float blend = std::min(1.0f, drive / 70.0f);
            return soft * (1.0f - blend) + hard * blend;
        }

        default:
            return std::tanh(x);
    }
}

float Distortion::processSample(float input)
{
    // Apply distortion
    float distorted = applyDistortion(input, driveAmount);

    // Normalize output (compensate for drive boost)
    float normalized = distorted / std::sqrt(driveAmount);

    // Apply output level
    return normalized * levelGain;
}

void Distortion::process(juce::AudioBuffer<float>& buffer)
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
        float processed = processSample(leftChannel[i]);
        // Apply tone filter
        filterStateL += filterCoeff * (processed - filterStateL);
        leftChannel[i] = filterStateL;
    }

    // Process right channel if stereo
    if (numChannels > 1)
    {
        float* rightChannel = buffer.getWritePointer(1);
        for (int i = 0; i < numSamples; ++i)
        {
            float processed = processSample(rightChannel[i]);
            filterStateR += filterCoeff * (processed - filterStateR);
            rightChannel[i] = filterStateR;
        }
    }
}
