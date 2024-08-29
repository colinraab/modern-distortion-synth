#ifndef Colin_Voice_H
#define Colin_Voice_H

#include <JuceHeader.h>
#include "WavetableVectors.h"
#include "WavetableOsc.h"

/*
  ==============================================================================

    Voice.h
    Created: 28 Jan 2024 4:55:52pm
    Author:  Colin Raab

  ==============================================================================
*/

namespace Colin
{

enum class Oscillator_Type {
    sine = 1, triangle, saw, square, fourtyFour, echo, juliett, crayon, gritty, end
};

enum class Noise_Type {
    gauss = 1, binary, lp, hp, end
};

class Voice {
public:
    Voice(int p, int v, bool n);
    ~Voice();
    void prepareToPlay(juce::dsp::ProcessSpec spec);
    void initializeOscillator(Oscillator_Type osc);
    void initializeNoise(Noise_Type noi);
    std::vector<float> getWavetable();
    std::vector<float> getNoise();
    void setPitch(int p);
    int getPitch();
    void setVelocity(int v);
    bool isPlaying();
    bool isNoise();
    void setADSR(juce::ADSR::Parameters envParams, float depth);
    void setNoise(bool isNoise);
    void setFilter(int type, float cutoff, float res, bool key, float ktA);
    void setEnvRouting(bool v, bool d, bool f);
    float getEnvSample();
    void noteOn();
    void noteOff();
    void setVol(float v);
    
    float renderNoise();
    void renderVoice(juce::AudioBuffer<float>* buffer, int startSample, int endSample);
    void renderVoiceFM(juce::AudioBuffer<float>* carrierBuffer, juce::AudioBuffer<float>* modBuffer, int startSample, int endSample, float depth);
    void processFilter(juce::AudioBuffer<float>* buffer);
    float midiToFreq(int midiNote);
    bool isRelease();
    
    float normVelocity(int vel);
    void getEnvSamples(int numSamples);

        
private:
    float sampleRate;
    int pitch;
    int pitchOffset = 0;
    int vel;
    bool active = true;
    bool noise = false;
    bool release = false;
    bool cycleEnv = false;
    
    float envSampleStart = 0.f;
    float envSampleEnd = 0.f;
    
    WavetableOscillator* oscillator;
    Oscillator_Type oscType = Oscillator_Type::sine;
    Noise_Type noiseType = Noise_Type::gauss;
    
    juce::dsp::StateVariableTPTFilter<float> TPT;
    juce::dsp::LadderFilter<float>* ladder;
    float filterType = 1;
    int curTPTMode = 1;
    int curLadderMode = 1;
    float curCutoff = 19000;
    float curRes = 1;
    bool keytrack = false;
    float keytrackAmount = 1;
    
    bool envToFilter = false;
    bool envToVol = false;
    bool envToDist = false;
    
    float prevHPNoiseSample = 0;
    float prevLPNoiseSample = 0;

    juce::ADSR env;
    float ADSRDepth = 0.f;

};

}
#endif
