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
    SamplerVoice(int pitch);
    ~SamplerVoice();
    void prepareToPlay(juce::dsp::ProcessSpec spec);
    //void renderVoice(juce::AudioBuffer<float>* buffer, juce::MidiBuffer& midiMessages);
    void renderVoice(juce::AudioBuffer<float>* buffer, juce::MidiBuffer& midiMessages, int startSample, int endSample);
    void setFilter(int type, float cutoff, float res, bool key, float ktA);
    void setEnvRouting(bool v, bool d, bool f);
    void processFilter(juce::AudioBuffer<float>* buffer);
    void setADSR(juce::ADSR::Parameters envParams, float depth);
    void noteOn();
    void noteOff();
    bool isRelease();
    int getPitch();
    void getEnvSamples(int numSamples);
    void addSound(juce::AudioFormatReader* formatReader);
    void setLoop(bool isLoop);
    float returnEnvSample();
    
private:
    float midiToFreq(int midiNote);
    
    juce::Synthesiser* jucesampler;
    float sampleRate;
    int pitch;
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
