#ifndef Colin_DISTORTION_H
#define Colin_DISTORTION_H

#include <cmath>
#include "Conversions.h"
#include <vector>
#include "../AuxShaper/AuxBezier.h"
#include "../AuxShaper/AuxWaveShape.h"

/*
  ==============================================================================

    Distortion.h
    Created: 26 Oct 2022 4:06:18pm
    Author:  Colin Raab

  ==============================================================================
*/

namespace Colin
{

enum class Distortion_Type {
    none = 1, arbitrary, exponential, softClip, fuzz, bitcrush, end
};

class Distortion {
private:
    Distortion_Type type;
    float inputGain = 0;
    float outputGain = 0;
    float mix = 1; // 0 is dry, 1 is wet
    float threshold = 0.8; // for soft clip
    float coeff = 0; // for other stuff
    AuxPort::Auxshape waveshaper;
    
public:
    Distortion() = default;
    ~Distortion() = default;
    Distortion(Distortion_Type type, float inputGain, float mix) {
        this->type = type;
        this->inputGain = inputGain;
        this->mix = mix;
    }
    
    void setType(Distortion_Type type) {
        this->type = type;
    }
    
    void setType(int type) {
        this->type = static_cast<Distortion_Type>(type);
    }
    
    void setEnv(float sample, float depth) {
        inputGain = DBtoLinear((linearToDB(inputGain) * (1-depth)) + (sample * linearToDB(inputGain) * depth));
    }
    
    void setInputGain(float inputGain) {
        this->inputGain = DBtoLinear(inputGain);
    }
    
    void setOutputGain(float outputGain) {
        this->outputGain = DBtoLinear(outputGain);
    }
    
    void setMix(float mix) {
        this->mix = mix;
    }
    
    void setThreshold(float threshold) {
        this->threshold = threshold;
    }
    
    void setCoeff(float c) {
        coeff = c;
        if(type == Distortion_Type::softClip) {
            threshold = 1-c/1.5;
        }
    }
    
    void normalize(bool isNormalized) {
        if(isNormalized) outputGain = DBtoLinear(-1.f * linearToDB(inputGain));
    }
    
    float exponential(float sample) {
        float q = sample;
        float sign = 0;
        if(q > 0) sign = 1;
        else if(q < 0) sign = -1;
        return sign * (1-exp((-1 - coeff) * fabs(q * 3)));
    }
    
    float softClip(float sample) {
        float temp = sample;
        if (fabs(temp)>=threshold) {
            if (temp > 0)
                sample = (3-pow((2-sample*3),2))/3;
            if (temp < 0)
                sample = -1*(3-pow((2-fabs(sample)*3),2))/3;
        }
        if (fabs(temp)>2*threshold) {
            if (temp > 0)
                sample = 1;
            if (temp < 0)
                sample = -1;
        }
        return sample;
    }
    
    float fuzz(float sample) {
        float q = sample;
        float sign = 0;
        if(q > 0) sign = -1;
        else if(q < 0) sign = 1;
        return sign * (1-exp(fabs(sample))) / (exp(1.5-coeff/2) - 1);
    }
    
    float bitcrush(float sample) {
        float QL = 2.0 / (std::powf(2.0, std::floor(8-2.5*coeff)) - 1.0);
        return QL*(int(sample / QL));
    }
    
    float processSample(float sample) {
        if(type == Distortion_Type::exponential) {
            sample = mix * outputGain * exponential(sample * inputGain) + (1-mix) * sample;
        }
        else if(type == Distortion_Type::softClip) {
            sample = mix * outputGain * softClip(sample * inputGain) + (1-mix) * sample;
        }
        else if(type == Distortion_Type::fuzz) {
            sample = mix * outputGain * fuzz(sample * inputGain) + (1-mix) * sample;
        }
        else if(type == Distortion_Type::bitcrush) {
            sample = mix * outputGain * bitcrush(sample * inputGain) + (1-mix) * sample;
        }
        return sample;
    }
    
    std::vector<float> getCurve() {
        std::vector<float> curve;
        for(int i=0; i<64; i++) {
            float val = std::abs(processSample(float(i) / 65.f));
            if(val > 1.f) val = 1.f;
            curve.push_back(val);
        }
        return curve;
    }
    
    void processBuffer(juce::AudioBuffer<float>& buffer)
    {
        auto* firstChannel = buffer.getWritePointer(0);
        auto* secondChannel = buffer.getWritePointer(1);
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            firstChannel[i] = processSample(firstChannel[i]);
            secondChannel[i] = processSample(secondChannel[i]);
        }
    }
    
    void processBufferWaveshaper(juce::AudioBuffer<float>& buffer, AuxPort::Bezier *bezier) {
        waveshaper.process(buffer, *bezier, inputGain, outputGain, mix);
    }
    
    void processBlock(juce::dsp::AudioBlock<float>& block)
    {
        auto* firstChannel = block.getChannelPointer(0);
        auto* secondChannel = block.getChannelPointer(1);
        for (int i = 0; i < block.getNumSamples(); i++)
        {
            firstChannel[i] = processSample(firstChannel[i]);
            secondChannel[i] = processSample(secondChannel[i]);
        }
    }
};

}

#endif
