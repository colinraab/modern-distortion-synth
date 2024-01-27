#ifndef Colin_Synth_H
#define Colin_Synth_H

#include "WavetableOsc.h"
#include "JuceHeader.h"
#include "../Biquad.h"
#include "../Conversions.h"
#include "WavetableVectors.h"
#include "../Distortion.h"
#include "../AuxShaper/AuxBezier.h"

namespace Colin
{

enum class Oscillator_Type {
    sine = 1, triangle, saw, square, fourtyFour, echo, juliett, crayon, gritty, end
};

enum class Noise_Type {
    gauss = 1, binary, lp, hp, end
};

class Synth {
public:
    Synth();
    ~Synth();
    void prepareToPlay(juce::dsp::ProcessSpec spec);
    void processBlock(juce::dsp::AudioBlock<float>&, juce::MidiBuffer&);
    void processBuffer(juce::AudioBuffer<float>&, juce::MidiBuffer&);
    void processBuffers(std::vector<juce::AudioBuffer<float>*>&, juce::MidiBuffer&);
    void processBlockFM(juce::dsp::AudioBlock<float>&, juce::dsp::AudioBlock<float>&, juce::MidiBuffer&);
    void processBuffersFM(std::vector<juce::AudioBuffer<float>*>&, std::vector<juce::AudioBuffer<float>*>&, juce::MidiBuffer&);
    void setOscillator(int type);
    void setFMDepth(float depth);
    void setPitch(float pitch);
    void setFilter(int type, float cutoff, float res, bool key, float ktA);
    void setDistortion(int type, float input, float output, float coeff, float mix, AuxPort::Bezier* b);
    std::vector<float> getWavetable();
    std::vector<float> getNoise();
    void setADSR(float atk, float dec, float sus, float rel, float depth);
    void setNoise(bool isNoise);
    void setKeytrack(bool key);
    void setEnvRouting(bool v, bool d, bool f);
    void setNoteOff(int note);
    static constexpr auto WAVETABLE_LENGTH = 1024;
    static constexpr auto OSCILLATORS_COUNT = 256;
    Distortion dist;
    void setOscVol(float newVol) { oscVol = newVol; }
    
private:
    void initializeOscillators();
    void initializeNoise();
    void handleMidiEvent(const juce::MidiMessage& midiEvent);
    float midiToFreq(int midiNote);
    void updateFreqs();
    void render(juce::dsp::AudioBlock<float>&, int, int);
    void render(juce::AudioBuffer<float>&, int, int);
    void renderBuffers(std::vector<juce::AudioBuffer<float>*>&, int, int);
    float renderNoise();
    void renderFM(juce::dsp::AudioBlock<float>&, juce::dsp::AudioBlock<float>&, int, int);
    void renderFM(std::vector<juce::AudioBuffer<float>*>&, std::vector<juce::AudioBuffer<float>*>&, int, int);
    void processFilters(std::vector<juce::AudioBuffer<float>*>&);
    void processDist(std::vector<juce::AudioBuffer<float>*>&);
    
    std::vector<bool> noiseIsPlaying;
    float FMdepth = 0.f;
    float pitch = 0.f;
    uint32 sampleRate;
    std::vector<WavetableOscillator> oscillators;
    Oscillator_Type oscType = Oscillator_Type::sine;
    Noise_Type noiseType = Noise_Type::gauss;
    std::vector<juce::ADSR> envs;
    juce::ADSR::Parameters envParams;
    bool isNoise = false;
    std::vector<juce::dsp::StateVariableTPTFilter<float>> TPTs;
    std::vector<juce::dsp::LadderFilter<float>*> ladders;
    float ADSRDepth = 0.f;
    std::list<int> curVoices;  // max 8 voices, make custom data typee
    int distType = 1; //h elp
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
};

}
#endif
