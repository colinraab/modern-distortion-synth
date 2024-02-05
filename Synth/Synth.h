#ifndef Colin_Synth_H
#define Colin_Synth_H

#include "WavetableOsc.h"
#include "JuceHeader.h"
#include "../Biquad.h"
#include "../Conversions.h"
#include "WavetableVectors.h"
#include "../Distortion.h"
#include "../AuxShaper/AuxBezier.h"
#include "Voice.h"

namespace Colin
{

class Synth {
public:
    Distortion dist;
    static constexpr auto WAVETABLE_LENGTH = 1024;
    static constexpr auto OSCILLATORS_COUNT = 256;
    
    Synth();
    ~Synth();
    void prepareToPlay(juce::dsp::ProcessSpec spec);
    std::vector<float> getWavetable();
    std::vector<float> getNoise();
    void setEnvRouting(bool v, bool d, bool f);
    void setDistortion(int type, float input, float output, float coeff, float mix, AuxPort::Bezier* b);
    void setOscillator(int type);
    void setNoise(bool isNoise);
    void setFMDepth(float depth);
    void setKeytrack(bool key);
    void setPitch(float pitch);
    void setFilter(int type, float cutoff, float res, bool key, float ktA);
    void processBuffer(juce::AudioBuffer<float>*, juce::MidiBuffer&, int i);
    void processBufferFM(juce::AudioBuffer<float>*, juce::AudioBuffer<float>*, juce::MidiBuffer&, int i);
    void setADSR(float atk, float dec, float sus, float rel, float depth);
    void deleteVoice(int i);
    void setOscVol(float newVol) { oscVol = newVol; }

    /*
    void processBlock(juce::dsp::AudioBlock<float>&, juce::MidiBuffer&);
    void processBuffers(std::vector<juce::AudioBuffer<float>*>&, juce::MidiBuffer&);
    void processBlockFM(juce::dsp::AudioBlock<float>&, juce::dsp::AudioBlock<float>&, juce::MidiBuffer&);
    void processBuffersFM(std::vector<juce::AudioBuffer<float>*>&, std::vector<juce::AudioBuffer<float>*>&, juce::MidiBuffer&);
    void setNoteOff(int note);
     */
    
private:
    void initializeVoices();
    std::vector<std::unique_ptr<Voice>> voices;
    juce::dsp::ProcessSpec spec;
    float FMdepth = 0.f;
    float pitch = 0.f;

    
    void processDist(juce::AudioBuffer<float>*, int i);
    void handleMidiEvent(const juce::MidiMessage& midiEvent);
    float midiToFreq(int midiNote);
    void updateFreqs();
    
    /*
    void render(juce::dsp::AudioBlock<float>&, int, int);
    void render(juce::AudioBuffer<float>&, int, int);
    void renderBuffers(std::vector<juce::AudioBuffer<float>*>&, int, int);
    float renderNoise();
    void renderFM(juce::dsp::AudioBlock<float>&, juce::dsp::AudioBlock<float>&, int, int);
    void renderFM(std::vector<juce::AudioBuffer<float>*>&, std::vector<juce::AudioBuffer<float>*>&, int, int);
    void processFilters(std::vector<juce::AudioBuffer<float>*>&);
    void processDist(std::vector<juce::AudioBuffer<float>*>&);
    */

    uint32 sampleRate;
    Oscillator_Type oscType = Oscillator_Type::sine;
    Noise_Type noiseType = Noise_Type::gauss;
    juce::ADSR::Parameters envParams;
    bool isNoise = false;
    float ADSRDepth = 0.f;
    int distType = 1;
    int filterType = 1;
    int curTPTMode = 1;
    int curLadderMode = 1;
    float curCutoff = 19000;
    float curRes = 1;
    bool envToFilter = false;
    bool envToVol = false;
    bool envToDist = false;
    bool keytrack = false;
    float keytrackAmount = 1;
    float oscVol = 0.2f;
    float prevHPNoiseSample = 0;
    float prevLPNoiseSample = 0;
    AuxPort::Bezier* bezier;
    
    /*
    std::list<int> curVoices;
    std::vector<juce::dsp::StateVariableTPTFilter<float>> TPTs;
    std::vector<juce::dsp::LadderFilter<float>*> ladders;
    std::vector<bool> noiseIsPlaying;
    std::vector<juce::ADSR> envs;
    std::vector<WavetableOscillator> oscillators;
    */
};
     

}
#endif
