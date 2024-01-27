/*
  ==============================================================================

    IIR.cpp
    Created: 12 Mar 2023 5:03:32pm
    Author:  Colin Raab

  ==============================================================================
*/

#include "IIR.h"

Colin::IIR::Butterworth::Butterworth()
{
    z1 = 0;
    z2 = 0;
    output = 0;
    coefficients.resize(5);
    for (uint32_t i = 0; i < coefficients.size(); i++)
        coefficients[i] = 0;
}

void Colin::IIR::Butterworth::prepareToPlay(float fc, float q, float sampleRate, Type type)
{
    if (type == Type::Lowpass)
    {
        float theta_c = M_PI * fc / sampleRate;
        float c = 1.0 / std::tanf(theta_c);
        coefficients[index::a0] = 1.0 / (1.0 + sqrt2 * c + powf(c, 2));
        coefficients[index::a1] = 2.0 * coefficients[index::a0];
        coefficients[index::a2] = coefficients[index::a0];
        coefficients[index::b1] = 2.0 * coefficients[index::a0] * (1.0 - powf(c, 2));
        coefficients[index::b2] = coefficients[a0] * (1.0 - sqrt2 * c + powf(c, 2));
    }

    if (type == Type::Highpass)
    {
        float theta_c = M_PI * fc / sampleRate;
        float c = 1.0 / std::tan(theta_c);
        coefficients[index::a0] = 1.0 / (1.0 + sqrt2 * c + powf(c, 2));
        coefficients[index::a1] = -2.0 * coefficients[index::a0];
        coefficients[index::a2] = coefficients[index::a0];
        coefficients[index::b1] = 2.0 * coefficients[index::a0] * (powf(c, 2) - 1.0);
        coefficients[index::b2] = coefficients[index::a0] * (1.0 - sqrt2 * c + powf(c, 2));
    }
}

float Colin::IIR::Butterworth::processSample(float sample)
{
    output = sample * coefficients[index::a0] + z1;
    z1 = sample * coefficients[index::a1] + z2 - coefficients[index::b1] * output;
    z2 = sample * coefficients[index::a2] - coefficients[index::b2] * output;
    return output;
}

Colin::IIR::Engine::Engine(Filter filter)
{
    this->filter = filter;
    if (filter == ButterLPF6dB || filter == ButterHPF6dB)
        butter1.resize(2);
    if (filter == ButterLPF12dB || filter == ButterHPF12dB)
    {
        butter1.resize(2);
        butter2.resize(2);
    }
}

void Colin::IIR::Engine::prepareToPlay(float fc, float q, float sampleRate, uint32_t channels)
{
    if (filter == ButterLPF6dB)
    {
        if (channels > butter1.size())
            butter1.resize(channels);
        for (uint32_t i = 0; i < butter1.size(); i++)
        {
            butter1[i].prepareToPlay(fc, q, sampleRate, Colin::IIR::Type::Lowpass);
        }
    }


    if (filter == ButterHPF6dB)
    {
        if (channels > butter1.size())
            butter1.resize(channels);
        for (uint32_t i = 0; i < butter1.size(); i++)
        {
            butter1[i].prepareToPlay(fc, q, sampleRate, Colin::IIR::Type::Highpass);
        }
    }


    if (filter == ButterHPF12dB)
    {
        if (channels > butter1.size())
        {
            butter1.resize(channels);
            butter2.resize(channels);
        }
            
        for (uint32_t i = 0; i < butter1.size(); i++)
        {
            butter1[i].prepareToPlay(fc, q, sampleRate, Colin::IIR::Type::Highpass);
            butter2[i].prepareToPlay(fc, q, sampleRate, Colin::IIR::Type::Highpass);
        }
    }

    if (filter == ButterLPF12dB)
    {
        if (channels > butter1.size())
        {
            butter1.resize(channels);
            butter2.resize(channels);
        }
        for (uint32_t i = 0; i < butter1.size(); i++)
        {
            butter1[i].prepareToPlay(fc, q, sampleRate, Colin::IIR::Type::Lowpass);
            butter2[i].prepareToPlay(fc, q, sampleRate, Colin::IIR::Type::Lowpass);
        }
    }

}

void Colin::IIR::Engine::process(juce::AudioBuffer<float>& buffer)
{
    for (uint32_t i = 0; i < buffer.getNumSamples(); i++)
    {
        for (uint32_t j = 0; j < buffer.getNumChannels(); j++)
        {
            if (filter == ButterLPF6dB || filter == ButterHPF6dB)
                buffer.setSample(j,i,butter1[j].processSample(buffer.getSample(j, i)));
            if (filter == ButterLPF12dB || filter == ButterHPF12dB)
                buffer.setSample(j, i, butter2[j].processSample(butter1[j].processSample(buffer.getSample(j, i))));
        }
    }
}
