#include "Modulation.h"
#include <cmath>

Modulation::Modulation()
{
    delayLineL.resize(MAX_DELAY_SAMPLES, 0.0f);
    delayLineR.resize(MAX_DELAY_SAMPLES, 0.0f);
}

void Modulation::prepare(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;

    // Resize delay lines for sample rate
    int maxSamples = static_cast<int>(sampleRate * 0.1);  // 100ms max
    delayLineL.resize(maxSamples, 0.0f);
    delayLineR.resize(maxSamples, 0.0f);

    reset();
}

void Modulation::reset()
{
    std::fill(delayLineL.begin(), delayLineL.end(), 0.0f);
    std::fill(delayLineR.begin(), delayLineR.end(), 0.0f);
    writeIndex = 0;
    lfoPhase = 0.0f;
    flangerFeedbackL = flangerFeedbackR = 0.0f;
    for (int i = 0; i < 6; ++i)
    {
        allpassStateL[i] = 0.0f;
        allpassStateR[i] = 0.0f;
    }
}

void Modulation::setRate(float newRate)
{
    rate = std::clamp(newRate, 0.0f, 100.0f);

    // Different rate ranges per effect type
    float minHz, maxHz;
    switch (type)
    {
        case ModulationType::Phaser:   minHz = 0.1f; maxHz = 4.0f; break;
        case ModulationType::Flanger:  minHz = 0.1f; maxHz = 5.0f; break;
        case ModulationType::Chorus:   minHz = 0.2f; maxHz = 5.0f; break;
        case ModulationType::Tremolo:  minHz = 1.0f; maxHz = 12.0f; break;
        case ModulationType::Vibrato:  minHz = 2.0f; maxHz = 10.0f; break;
        default:                       minHz = 0.1f; maxHz = 5.0f; break;
    }

    lfoFreq = minHz + (rate / 100.0f) * (maxHz - minHz);
    lfoIncrement = lfoFreq / static_cast<float>(currentSampleRate);
}

void Modulation::setDepth(float newDepth)
{
    depth = std::clamp(newDepth, 0.0f, 100.0f);
    depthAmount = depth / 100.0f;
}

void Modulation::setBlend(float newBlend)
{
    blend = std::clamp(newBlend, 0.0f, 100.0f);
    blendAmount = blend / 100.0f;
}

void Modulation::setType(int newType)
{
    type = static_cast<ModulationType>(std::clamp(newType, 0, 4));
    setRate(rate);  // Recalculate rate for new type
}

float Modulation::getLFOValue()
{
    // Sine wave LFO (0 to 1 range)
    return 0.5f + 0.5f * std::sin(2.0f * 3.14159f * lfoPhase);
}

void Modulation::advanceLFO()
{
    lfoPhase += lfoIncrement;
    if (lfoPhase >= 1.0f)
        lfoPhase -= 1.0f;
}

void Modulation::writeToDelayLine(float sample, int channel)
{
    if (channel == 0)
        delayLineL[writeIndex] = sample;
    else
        delayLineR[writeIndex] = sample;
}

float Modulation::readFromDelayLine(float delayMs, int channel)
{
    float delaySamples = delayMs * 0.001f * static_cast<float>(currentSampleRate);
    int maxDelay = static_cast<int>(channel == 0 ? delayLineL.size() : delayLineR.size());
    delaySamples = std::clamp(delaySamples, 1.0f, static_cast<float>(maxDelay - 1));

    // Linear interpolation for smooth modulation
    int index0 = static_cast<int>(delaySamples);
    int index1 = index0 + 1;
    float frac = delaySamples - static_cast<float>(index0);

    int readIndex0 = (writeIndex - index0 + maxDelay) % maxDelay;
    int readIndex1 = (writeIndex - index1 + maxDelay) % maxDelay;

    const auto& delayLine = (channel == 0) ? delayLineL : delayLineR;
    return delayLine[readIndex0] * (1.0f - frac) + delayLine[readIndex1] * frac;
}

float Modulation::processPhaser(float input, int channel)
{
    float lfo = getLFOValue();
    float* state = (channel == 0) ? allpassStateL : allpassStateR;

    // 6-stage allpass filter cascade
    float baseFreqs[6] = {200.0f, 400.0f, 800.0f, 1600.0f, 3200.0f, 6400.0f};
    float output = input;

    for (int i = 0; i < 6; ++i)
    {
        // Modulate frequency with LFO
        float freq = baseFreqs[i] * (1.0f + lfo * depthAmount * 0.5f);
        float w = 2.0f * 3.14159f * freq / static_cast<float>(currentSampleRate);
        float coeff = (1.0f - w) / (1.0f + w);

        // Allpass filter: y = coeff * (x - y_prev) + x_prev
        float y = coeff * (output - state[i]) + state[i];
        state[i] = output;
        output = y;
    }

    return input * (1.0f - blendAmount) + output * blendAmount;
}

float Modulation::processFlanger(float input, int channel)
{
    float lfo = getLFOValue();

    // Modulated delay: 1-8ms
    float delayMs = 1.0f + lfo * depthAmount * 7.0f;

    float& feedback = (channel == 0) ? flangerFeedbackL : flangerFeedbackR;
    float delayedSample = readFromDelayLine(delayMs, channel);

    // Add feedback (creates comb filtering)
    float toDelay = input + feedback * 0.6f;
    writeToDelayLine(toDelay, channel);
    feedback = delayedSample;

    return input * (1.0f - blendAmount) + delayedSample * blendAmount;
}

float Modulation::processChorus(float input, int channel)
{
    float lfo = getLFOValue();

    // Longer delay than flanger: 15-30ms with stereo offset
    float baseDelay = 20.0f;
    float modAmount = depthAmount * 10.0f;
    float stereoOffset = (channel == 0) ? 0.0f : 0.25f;

    float phase = lfoPhase + stereoOffset;
    if (phase >= 1.0f) phase -= 1.0f;
    float lfoStereo = 0.5f + 0.5f * std::sin(2.0f * 3.14159f * phase);

    float delayMs = baseDelay + lfoStereo * modAmount;

    writeToDelayLine(input, channel);
    float delayedSample = readFromDelayLine(delayMs, channel);

    return input * (1.0f - blendAmount) + delayedSample * blendAmount;
}

float Modulation::processTremolo(float input, int channel)
{
    float lfo = getLFOValue();

    // Amplitude modulation
    float modulation = 1.0f - depthAmount * (1.0f - lfo);

    return input * modulation;
}

float Modulation::processVibrato(float input, int channel)
{
    float lfo = getLFOValue();

    // Pitch modulation via delay (3-7ms range)
    float delayMs = 5.0f + (lfo - 0.5f) * depthAmount * 4.0f;

    writeToDelayLine(input, channel);
    float delayedSample = readFromDelayLine(delayMs, channel);

    // Vibrato is 100% wet
    return delayedSample;
}

void Modulation::process(juce::AudioBuffer<float>& buffer)
{
    if (bypassed)
        return;

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    if (numChannels == 0)
        return;

    for (int i = 0; i < numSamples; ++i)
    {
        // Process left channel
        float* leftChannel = buffer.getWritePointer(0);
        switch (type)
        {
            case ModulationType::Phaser:  leftChannel[i] = processPhaser(leftChannel[i], 0); break;
            case ModulationType::Flanger: leftChannel[i] = processFlanger(leftChannel[i], 0); break;
            case ModulationType::Chorus:  leftChannel[i] = processChorus(leftChannel[i], 0); break;
            case ModulationType::Tremolo: leftChannel[i] = processTremolo(leftChannel[i], 0); break;
            case ModulationType::Vibrato: leftChannel[i] = processVibrato(leftChannel[i], 0); break;
        }

        // Process right channel if stereo
        if (numChannels > 1)
        {
            float* rightChannel = buffer.getWritePointer(1);
            switch (type)
            {
                case ModulationType::Phaser:  rightChannel[i] = processPhaser(rightChannel[i], 1); break;
                case ModulationType::Flanger: rightChannel[i] = processFlanger(rightChannel[i], 1); break;
                case ModulationType::Chorus:  rightChannel[i] = processChorus(rightChannel[i], 1); break;
                case ModulationType::Tremolo: rightChannel[i] = processTremolo(rightChannel[i], 1); break;
                case ModulationType::Vibrato: rightChannel[i] = processVibrato(rightChannel[i], 1); break;
            }
        }

        // Advance write index for delay-based effects
        if (type == ModulationType::Flanger || type == ModulationType::Chorus || type == ModulationType::Vibrato)
        {
            writeIndex = (writeIndex + 1) % static_cast<int>(delayLineL.size());
        }

        advanceLFO();
    }
}
