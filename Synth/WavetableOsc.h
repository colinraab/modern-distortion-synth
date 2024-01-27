/*
  ==============================================================================

    WavetableOsc.h
    Created: 5 Mar 2023 12:32:49pm
    Author:  Colin Raab

  ==============================================================================
*/

#ifndef Colin_WOSC_H
#define Colin_WOSC_H

#include <cmath>
#include <vector>
#include "JuceHeader.h"

namespace Colin
{

class WavetableOscillator
{
public:
    WavetableOscillator(std::vector<float> wavetable, double sampleRate, int midi);
    void setFrequency(float frequency);
    void setPitch(int midi);
    void setFM(float modSample);
    void setPM(float modSample);
    void isNoise();
    float getSample();
    float getSampleRaw();
    float interpolateLinearly();
    void stop();
    bool isPlaying();
    float midiToFreq(int midiNote);
    float getFrequency();
    
private:
    std::vector<float> wavetable;
    double sampleRate;
    float index = 0.f;
    float indexIncrement = 0.f;
    bool oversample = false;
    float prevSample = 0.f;
    int midi;
    float freq;
};

}

#endif

