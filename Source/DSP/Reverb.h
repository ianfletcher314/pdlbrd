#pragma once

#include "EffectModule.h"
#include <JuceHeader.h>

class Reverb : public EffectModule
{
public:
    enum Type { Spring = 0, Plate, Hall };

    void prepare(double sampleRate, int samplesPerBlock) override
    {
        currentSampleRate = sampleRate;
        juce::ignoreUnused(samplesPerBlock);

        reverb.setSampleRate(sampleRate);

        // Prepare tone filter
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
        spec.numChannels = 2;
        toneFilter.prepare(spec);

        updateParameters();
    }

    void process(juce::AudioBuffer<float>& buffer) override
    {
        if (bypassed)
            return;

        updateParameters();

        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();

        // Store dry signal for blend
        juce::AudioBuffer<float> dryBuffer;
        dryBuffer.makeCopyOf(buffer);

        // Process reverb
        if (numChannels >= 2)
        {
            reverb.processStereo(buffer.getWritePointer(0),
                                 buffer.getWritePointer(1),
                                 numSamples);
        }
        else if (numChannels == 1)
        {
            reverb.processMono(buffer.getWritePointer(0), numSamples);
        }

        // Apply tone filter to wet signal
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        toneFilter.process(context);

        // Blend dry/wet
        float wetAmount = blend / 100.0f;
        float dryAmount = 1.0f - wetAmount;

        for (int ch = 0; ch < numChannels; ++ch)
        {
            float* wet = buffer.getWritePointer(ch);
            const float* dry = dryBuffer.getReadPointer(ch);

            for (int i = 0; i < numSamples; ++i)
            {
                wet[i] = dry[i] * dryAmount + wet[i] * wetAmount;
            }
        }
    }

    void reset() override
    {
        reverb.reset();
        toneFilter.reset();
    }

    // Parameters (0-100 range from UI)
    void setDecay(float value) { decay = value; }
    void setTone(float value) { tone = value; }
    void setBlend(float value) { blend = value; }
    void setType(int typeIndex) { reverbType = static_cast<Type>(typeIndex); }

private:
    void updateParameters()
    {
        juce::Reverb::Parameters params;

        // Base parameters that vary by type
        float roomSize, damping, width;

        switch (reverbType)
        {
            case Spring:
                // Spring reverb: shorter, brighter, narrower
                roomSize = 0.3f + (decay / 100.0f) * 0.4f;  // 0.3-0.7
                damping = 0.3f + ((100.0f - tone) / 100.0f) * 0.4f;
                width = 0.6f;
                break;

            case Plate:
                // Plate reverb: medium, smooth, wide
                roomSize = 0.4f + (decay / 100.0f) * 0.45f;  // 0.4-0.85
                damping = 0.2f + ((100.0f - tone) / 100.0f) * 0.5f;
                width = 0.9f;
                break;

            case Hall:
            default:
                // Hall reverb: long, dark, very wide
                roomSize = 0.5f + (decay / 100.0f) * 0.49f;  // 0.5-0.99
                damping = 0.4f + ((100.0f - tone) / 100.0f) * 0.5f;
                width = 1.0f;
                break;
        }

        params.roomSize = roomSize;
        params.damping = damping;
        params.width = width;
        params.wetLevel = 1.0f;  // We handle wet/dry ourselves
        params.dryLevel = 0.0f;
        params.freezeMode = 0.0f;

        reverb.setParameters(params);

        // Tone filter: lowpass that follows the tone parameter
        // tone 0 = dark (1kHz), tone 100 = bright (12kHz)
        float cutoff = 1000.0f + (tone / 100.0f) * 11000.0f;
        *toneFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(
            currentSampleRate, cutoff, 0.707f);
    }

    juce::Reverb reverb;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                    juce::dsp::IIR::Coefficients<float>> toneFilter;

    Type reverbType = Spring;
    float decay = 50.0f;
    float tone = 50.0f;
    float blend = 30.0f;
};
