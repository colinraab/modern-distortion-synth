#ifndef AUXFILTER_H
#define AUXFILTER_H

/*
  ==============================================================================

    AuxFilter.h
    Created: 11 Mar 2023 4:25:58pm
    Author:  satya

  ==============================================================================
*/
#include <vector>
#define _USE_MATH_DEFINES

#include <math.h>


#ifdef JUCE_SHARED_CODE
#include "JuceHeader.h"
#endif


namespace AuxPort
{
    class FIR
    {
    public:
        FIR() = default;
        ~FIR() = default;
        FIR(const FIR& fir) = default;
        void setImpulseResponse(const std::vector<float>& impulseResponse);
#ifdef JUCE_SHARED_CODE
        void process(juce::AudioBuffer<float>& buffer);
#endif
        float process(const float& inputSample);
    private:
#ifdef JUCE_SHARED_CODE
        std::vector<std::vector<float>> filterBuffer;
        std::vector<float> outputBuffer;
#endif
        std::vector<float> sampleFilterBuffer;
        float output;
        std::vector<float> impulseResponse;
        int32_t readIndex = 0;
        int32_t writeIndex = 0;
    };


    class CDFilters
    {
    public:
        static std::vector<float> computeJ1(uint32_t order);
        static std::vector<float> computeJ2(uint32_t order);
    };

    class Derivative
    {
    public:
        Derivative() = default;
        ~Derivative() = default;
        Derivative(const Derivative& derivative) = default;
        float compute(const float& input)
        {
            output = input - prevSample;
            prevSample = input;
            return output;
        }
        void reset();
    private:
        float prevSample = 0;
        float output = 0;
    };
}

namespace AuxPort
{
    
#define AuxPI 3.14159265358979323846
    class Utility
    {
    public:
        static float Sinc(const float& x)
        {
            
            return x == 0 ?  1 : sinf(AuxPI*x) / (AuxPI *x);
        }
    };

    class Hamming
    {
    public:
        enum Type
        {
            LowPass,HighPass,BandPass
        };
        Hamming(const Type& type);
        ~Hamming() = default;
        Hamming(const Hamming& hamming) = default;
        void compute(float passBand, float stopBand, uint32_t samplingRate);
        std::vector<float>* getImpulseResponse();

    private:
        std::vector<float> impulseResponse;
        std::vector<float> hammingWindow;
        std::vector<float> hLowpass;
        std::vector<float> hHighpass;
        float Wp;
        float Ws;
        float Wc;
        Type type;
    };
}

#endif
