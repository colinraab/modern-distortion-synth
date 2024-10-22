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
#include "SamplerVoice.h"

namespace Colin {

class Sampler {
public:
    Sampler();
    ~Sampler();
    void loadFile();
    void setPitch(float p, bool re);
    void prepareToPlay(juce::dsp::ProcessSpec spec);
    void setFilter(int type, float cutoff, float res, bool key, float ktA);
    void setDistortion(int type, float input, float output, float coeff, float mix, AuxPort::Bezier* b);
    void setADSR(float atk, float dec, float sus, float rel, float depth);
    void setEnvRouting(bool filt, bool vol, bool dist);
    bool isSampleLoaded();
    void setNoteOff(int note);
    void processBuffers(std::vector<juce::AudioBuffer<float>*>& buffers, juce::MidiBuffer& midiMessages);
    void processBuffer(std::unique_ptr<juce::AudioBuffer<float>>& buffer, juce::MidiBuffer& midiMessages, int i);
    juce::MidiBuffer repitchMessages(juce::MidiBuffer& midiMessages);
    juce::MidiBuffer sortMessages(juce::MidiBuffer& midiMessages, int voice);
    juce::AudioBuffer<float>& getWaveform() { return waveform; }
    const static int NUM_VOICES { 8 };
    int enabled [128] = {0};
    int curPitch [NUM_VOICES] = {-1, -1, -1, -1, -1, -1, -1, -1};
    Distortion dist;
    void setLoop(bool isLoop);
    int sampleLength = 0;
    void setSampleLength(float newLenPercent);
    void deleteVoice(int i);
    
private:
    bool sampleLoaded = false;
    void processDist(std::unique_ptr<juce::AudioBuffer<float>>& buffer, float envSample);
    void processFilters(std::unique_ptr<juce::AudioBuffer<float>>& buffer, int i);
    void handleMidiEvent(const juce::MidiMessage& midiEvent);

    juce::dsp::ProcessSpec spec;
    std::vector<std::unique_ptr<SamplerVoice>> voices;

    std::unique_ptr<juce::AudioFormatReader> formatReader;
    
    float midiToFreq(int midiNote);
    std::vector<juce::Synthesiser*> jucesamplers;
    juce::AudioFormatManager formatManager;
    double sampleRate = 44100;
    int pitch = 0;
    juce::AudioBuffer<float> waveform;
    juce::ADSR::Parameters envParams;
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
