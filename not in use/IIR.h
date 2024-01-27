#ifndef COLIN_IIR_H
#define COLIN_IIR_H
/*
  ==============================================================================

    IIR.h
    Created: 12 Mar 2023 5:03:32pm
    Author:  Colin Raab

  ==============================================================================
*/
#include "JuceHeader.h"
#define _USE_MATH_DEFINES  // To enable Math constants in MSVC 
#include <math.h>
namespace Colin
{
    namespace IIR
    {
        enum class Type
        {
            Lowpass,Highpass
        };
        class Butterworth
        {
        public:
            Butterworth();
            ~Butterworth() = default;
            Butterworth(const Butterworth& butterworth) = default;
            void prepareToPlay(float fc, float q, float sampleRate,Type type = Type::Lowpass);
            float processSample(float sample);
        private:
            std::vector<float> coefficients;
            enum index
            {
                a0,a1,a2,b1,b2
            };
            float z1;
            float z2;
            const float sqrt2 = sqrtf(2);
            float output;
        };
        
        
        class Engine
        {
        public:
            enum Filter
            {
                ButterLPF6dB,ButterHPF6dB,ButterLPF12dB,ButterHPF12dB
            };
            Engine() = default;
            Engine(Filter filter);
            ~Engine() = default;
            Engine(const Engine& engine) = default;
            void prepareToPlay(float fc, float q, float sampleRate,uint32_t channels = 2);
            void process(juce::AudioBuffer<float>& buffer);
        private:
            std::vector<Butterworth> butter1;
            std::vector<Butterworth> butter2;
            Filter filter;
        };



    }
}

#endif
