#ifndef Colin_Sampler_H
#define Colin_Sampler_H
/*
  ==============================================================================

    Sampler.h
    Created: 23 Mar 2023 2:46:57pm
    Author:  Colin Raab

  ==============================================================================
*/

#include <vector>
#include "JuceHeader.h"
#include "../Distortion.h"
#include "../AuxShaper/AuxBezier.h"

namespace Colin {

class Sampler {
public:
    Sampler();
    ~Sampler();
    void loadFile();
    void setPitch(float pitch, bool re);
    void prepareToPlay(juce::dsp::ProcessSpec spec);
    void setFilter(int type, float cutoff, float res, bool key, float ktA);
    void setDistortion(int type, float input, float output, float coeff, float mix, AuxPort::Bezier* b);
    void setADSR(float atk, float dec, float sus, float rel, float depth);
    void setEnvRouting(bool filt, bool vol, bool dist);
    bool isSampleLoaded();
    void setNoteOff(int note);
    void processBuffers(std::vector<juce::AudioBuffer<float>*>& buffers, juce::MidiBuffer& midiMessages);
    juce::MidiBuffer repitchMessages(juce::MidiBuffer& midiMessages);
    juce::MidiBuffer sortMessages(juce::MidiBuffer& midiMessages, int voice);
    juce::AudioBuffer<float>& getWaveform() { return waveform; }
    const static int NUM_VOICES { 3 };
    int enabled [128] = {0};
    int curPitch [NUM_VOICES] = {-1, -1, -1};
    Distortion dist;
    void setLoop(bool isLooping) { loop = isLooping; }
    int sampleLength = 0;
    void setSampleLength(float newLenPercent);
    
private:
    bool sampleLoaded = false;
    void processDist(std::vector<juce::AudioBuffer<float>*>& buffers);
    void processFilters(std::vector<juce::AudioBuffer<float>*>& buffers);
    void processVol(std::vector<juce::AudioBuffer<float>*>& buffers);
    float midiToFreq(int midiNote);
    std::vector<juce::Synthesiser*> jucesamplers;
    juce::AudioFormatManager formatManager;
    double sampleRate = 44100;
    float pitch = 0.f;
    juce::AudioBuffer<float> waveform;
    std::vector<juce::ADSR> envs;
    juce::ADSR::Parameters envParams;
    std::vector<juce::dsp::StateVariableTPTFilter<float>> TPTs;
    std::vector<juce::dsp::LadderFilter<float>*> ladders;
    int voices = 0;
    bool keytrack = false;
    float keytrackAmount = 1;
    float ADSRDepth = 0.f;
    int distType = 1;
    int type = 1;
    int curTPTMode = 1;
    int curLadderMode = 1;
    float curCutoff = 19000;
    float curRes = 1;
    bool envToFilter = false;
    bool envToVol = false;
    bool envToDist = false;
    bool loop = false;
    int nextVoice = 0;
    bool repitch = true;
    juce::uint8 lastVel[NUM_VOICES] = {0};
    int curSample[NUM_VOICES] = {0};
    AuxPort::Bezier* bezier;
};

}

#endif
