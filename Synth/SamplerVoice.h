#ifndef Colin_SamplerVoice_H
#define Colin_SamplerVoice_H

#include <JuceHeader.h>

/*
  ==============================================================================

    SamplerVoice.h
    Created: 5 Feb 2024 3:39:06pm
    Author:  Colin Raab

  ==============================================================================
*/

namespace Colin
{

class SamplerVoice {
public:
    SamplerVoice(int pitch, int vel);
    ~SamplerVoice();
    void prepareToPlay(juce::dsp::ProcessSpec spec);
    void renderVoice(std::unique_ptr<juce::AudioBuffer<float>>& buffer, juce::MidiBuffer& midiMessages, int startSample, int endSample);
    void setFilter(int type, float cutoff, float res, bool key, float ktA);
    void setEnvRouting(bool v, bool d, bool f);
    void processFilter(std::unique_ptr<juce::AudioBuffer<float>>& buffer);
    void setADSR(juce::ADSR::Parameters envParams, float depth);
    void noteOn();
    void noteOff();
    bool isRelease();
    int getPitch();
    void setPitchOffset(int offset);
    void getEnvSamples(int numSamples);
    void addSound(std::unique_ptr<juce::AudioFormatReader>& formatReader);
    void setLoop(bool isLoop);
    float returnEnvSample();
    void setRepitch(bool shouldRepitch);
    
private:
    float midiToFreq(int midiNote);
    float normVelocity(int vel);
    void setFrequency(float frequency);
    float interpolateLinearly();
    float getSample();
    
    juce::AudioBuffer<float>* sample;
    float index = 0.f;
    float prevSample = 0.f;
    float indexIncrement = 0.f;
    
    float sampleRate;
    float sampleSampleRate;
    int pitch;
    int pitchOffset = 0;
    bool repitch = true;
    int vel;
    bool active = true;
    bool noise = false;
    bool release = false;
    bool cycleEnv = false;
    float envSampleStart = 0.f;
    float envSampleEnd = 0.f;
    bool loop = false;
    int curSample;
    int sampleLength;
    
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
    
    juce::ADSR env;
    float ADSRDepth = 0.f;
};

}

#endif
