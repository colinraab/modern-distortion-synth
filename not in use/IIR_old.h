#ifndef COLIN_IIR_H
#define COLIN_IIR_H
#include <math.h>

/*
  ==============================================================================

    IIR.h
    Created: 12 Mar 2023 2:57:48pm
    Author:  Colin Raab

  ==============================================================================
*/

namespace Colin {

enum class IIR_Type {
    LPF, HPF
};

class IIR_Filter {
public:
    IIR_Filter() {
        z1l = z2l = z1r = z2r = 0.0;
        type = IIR_Type::LPF;
        calculateCoeffs();
    }
    
    void prepareToPlay(float cutoff, float q, IIR_Type type, float sampleRate) {
        this->sampleRate = sampleRate;
        this->cutoff = cutoff / sampleRate;
        this->q = q;
        this->type = type;
        calculateCoeffs();
    }
    
    void processBufferStereo(juce::AudioBuffer<float>& buffer) {
        auto *leftIn = buffer.getReadPointer(0);
        auto *rightIn = buffer.getReadPointer(1);
        auto *leftOut = buffer.getWritePointer(0);
        auto *rightOut = buffer.getWritePointer(1);
        for(int sample=0; sample<buffer.getNumSamples(); sample++) {
            float outL = leftIn[sample] * a0 + z1l;
            z1l = leftIn[sample] * coeffArray[a1] + z2l - coeffArray[b1] * outL;
            z2l = leftIn[sample] * coeffArray[a2] - coeffArray[b2] * outL;
            leftOut[sample] = outL;
            float outR = rightIn[sample] * coeffArray[a0] + z1r;
            z1r = rightIn[sample] * coeffArray[a1] + z2r - coeffArray[b1] * outR;
            z2r = rightIn[sample] * coeffArray[a2] - coeffArray[b2] * outR;
            rightOut[sample] = outR;
        }
    }
    
private:
    float z1l, z2l, z1r, z2r;
    float sampleRate = 44100;
    float cutoff;
    float q;
    IIR_Type type;
    float coeffArray[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
    int a0 = 0, a1 = 1, a2 = 2, b1 = 3, b2 = 4;
    const double kSqrtTwo = std::pow(2.0, 0.5);
    
    void calculateCoeffs() {
        // butterworth 2nd order implementation
        if(type == IIR_Type::LPF) {
            double theta_c = M_PI * cutoff;
            double C = 1.0 / std::tan(theta_c);
            coeffArray[a0] = 1.0 / (1.0 + kSqrtTwo * C + C*C);
            coeffArray[a1] = 2.0 * coeffArray[a0];
            coeffArray[a2] = coeffArray[a0];
            coeffArray[b1] = 2.0 * coeffArray[a0] * (1.0 - C*C);
            coeffArray[b2] = coeffArray[a0] * (1.0 - kSqrtTwo*C + C*C);
        }
        else if(type == IIR_Type::HPF) {
            double theta_c = M_PI * cutoff;
            double C = 1.0 / std::tan(theta_c);
            coeffArray[a0] = 1.0 / (1.0 + kSqrtTwo * C + C*C);
            coeffArray[a1] = -2.0 * coeffArray[a0];
            coeffArray[a2] = coeffArray[a0];
            coeffArray[b1] = 2.0 * coeffArray[a0] * (C*C - 1.0);
            coeffArray[b2] = coeffArray[a0] * (1.0 - kSqrtTwo*C + C*C);
        }
    }
};

}

#endif
