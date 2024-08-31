/*
  ==============================================================================

    Synth.cpp
    Created: 7 Mar 2023 3:37:55pm
    Author:  Colin Raab

  ==============================================================================
*/

#include "Synth.h"

namespace Colin  {

Synth::Synth() {}

Synth::~Synth() {
    voices.clear();
}

void Synth::prepareToPlay(juce::dsp::ProcessSpec s)
{
    voices.clear();
    spec = s;
    this->sampleRate = s.sampleRate;
    dist.setType(Distortion_Type::none);
    dist.setOutputGain(-3.f);
}

std::vector<float> Synth::getWavetable() {
    if(oscType == Oscillator_Type::sine) {
        return sineWavetable();
    }
    else if(oscType == Oscillator_Type::triangle) {
        return triangleWavetable();
    }
    else if(oscType == Oscillator_Type::square) {
        return squareWavetable();
    }
    else if(oscType == Oscillator_Type::saw) {
        return sawWavetable();
    }
    else if(oscType == Oscillator_Type::fourtyFour) {
        return fourtyFour();
    }
    else if(oscType == Oscillator_Type::echo) {
        return echo();
    }
    else if(oscType == Oscillator_Type::juliett) {
        return juliett();
    }
    else if(oscType == Oscillator_Type::crayon) {
        return crayon();
    }
    else if(oscType == Oscillator_Type::gritty) {
        return gritty();
    }
}

std::vector<float> Synth::getNoise() {
    if(noiseType == Noise_Type::gauss) {
        return gauss();
    }
    else if(noiseType == Noise_Type::binary) {
        return binary();
    }
    else if(noiseType == Noise_Type::lp) {
        return lpNoise();
    }
    else if(noiseType == Noise_Type::hp) {
        return hpNoise();
    }
}

void Synth::setEnvRouting(bool v, bool d, bool f) {
    envToVol = v;
    envToDist = d;
    envToFilter = f;
    for(int i=0; i<voices.size(); i++) {
        voices[i]->setEnvRouting(v, d, f);
    }
}

void Synth::setDistortion(int type, float input, float output, float coeff, float mix, AuxPort::Bezier* b) {
    distType = type;
    dist.setType(type);
    dist.setInputGain(input);
    dist.setOutputGain(output);
    dist.setCoeff(coeff);
    dist.setMix(mix);
    bezier = b;
}

void Synth::setOscillator(int type) {
    if(!isNoise) {
        if(static_cast<Oscillator_Type>(type) == oscType) return;
        oscType = static_cast<Oscillator_Type>(type);
        for(int i=0; i<voices.size(); i++) {
            voices[i]->initializeOscillator(oscType);
        }
    }
    else {
        if(static_cast<Noise_Type>(type) == noiseType) return;
        noiseType = static_cast<Noise_Type>(type);
        for(int i=0; i<voices.size(); i++) {
            voices[i]->initializeNoise(noiseType);
        }
    }
}

void Synth::setNoise(bool isNoise) {
    this->isNoise = isNoise;
}

void Synth::setFMDepth(float depth) {
    FMdepth = depth;
}

void Synth::setKeytrack(bool key) {
    keytrack = key;
}

void Synth::setPitch(float pitch) {
    int p = static_cast<int>(std::roundf(pitch));
    if(pitchOffset != p) {
        pitchOffset = p;
        updateFreqs();
    }
}

void Synth::setFilter(int type, float cutoff, float res, bool key, float ktA) {
    curCutoff = cutoff;
    curRes = res;
    filterType = type;
    keytrack = key;
    keytrackAmount = ktA;
    for(int i=0; i<voices.size(); i++) {
        voices[i]->setFilter(type, cutoff, res, key, ktA);
    }
}

void Synth::processBuffer(std::unique_ptr<juce::AudioBuffer<float>>& buffer, juce::MidiBuffer& midiMessages, int i)
{
    auto currentSample = 0;
    for (const auto midiMessage : midiMessages) {
        const auto midiEvent = midiMessage.getMessage();
        const auto midiEventSample = static_cast<int>(midiEvent.getTimeStamp());
        if(voices.size() >= 1) voices[i]->renderVoice(buffer, currentSample, midiEventSample);
        currentSample = midiEventSample;
        handleMidiEvent(midiEvent);
    }
    voices[i]->renderVoice(buffer, currentSample, buffer->getNumSamples());
    buffer->applyGain(oscVol);
    processDist(buffer, i);
    voices[i]->processFilter(buffer);
}

void Synth::processBufferFM(std::unique_ptr<juce::AudioBuffer<float>>& carrierBuffer, std::unique_ptr<juce::AudioBuffer<float>>& modBuffer, juce::MidiBuffer& midiMessages, int i)
{
    auto currentSample = 0;
    for (const auto midiMessage : midiMessages) {
        const auto midiEvent = midiMessage.getMessage();
        const auto midiEventSample = static_cast<int>(midiEvent.getTimeStamp());
        if(voices.size() >= 1) voices[i]->renderVoiceFM(carrierBuffer, modBuffer, currentSample, midiEventSample, FMdepth);
        currentSample = midiEventSample;
        handleMidiEvent(midiEvent);
    }
    voices[i]->renderVoiceFM(carrierBuffer, modBuffer, currentSample, carrierBuffer->getNumSamples(), FMdepth);
    carrierBuffer->applyGain(oscVol);
    processDist(carrierBuffer, i);
    voices[i]->processFilter(carrierBuffer);
}

void Synth::deleteVoice(int i) {
    if(i+1>voices.size()) return;
    voices.erase(voices.begin()+i);
}

void Synth::setADSR(float atk, float dec, float sus, float rel, float depth) {
    ADSRDepth = depth;
    if(atk == envParams.attack && dec == envParams.decay && sus == envParams.sustain && rel == envParams.release)
        return;
    envParams.attack = atk;
    envParams.decay = dec;
    envParams.sustain = sus;
    envParams.release = rel;
    for(int i=0; i<voices.size(); i++) {
        voices[i]->setADSR(envParams, ADSRDepth);
    }
}

void Synth::processDist(std::unique_ptr<juce::AudioBuffer<float>>& buffer, int i) {
    if(distType == 1) return;
    if(envToDist) {
        float envSample = voices[i]->getEnvSample();
        dist.setEnv(envSample, ADSRDepth);
    }
    if(distType == 2) dist.processBufferWaveshaper(*buffer, bezier);
    else dist.processBuffer(*buffer);
}

float Synth::midiToFreq(int midiNote)
{
    constexpr float A4_FREQ = 440;
    constexpr float A4_MIDINOTE = 69;
    constexpr float NOTES_IN_OCTAVE = 12.f;
    return A4_FREQ * std::powf(2, (static_cast<float>(midiNote) - A4_MIDINOTE + pitchOffset) / NOTES_IN_OCTAVE);
}

void Synth::updateFreqs() {
    for(int i=0; i<voices.size(); i++) {
        voices[i]->setPitch(static_cast<int>(pitchOffset));
    }
}

void Synth::handleMidiEvent(const juce::MidiMessage& midiEvent) {
    if(midiEvent.isNoteOn()) {
        const auto note = midiEvent.getNoteNumber();
        int vel = midiEvent.getVelocity();
        for(int i=0; i<voices.size(); i++) {
            if(voices[i]->getPitch() == note && !voices[i]->isRelease()) {
                return;
            }
        }
        std::unique_ptr<Voice> v = std::make_unique<Voice>(note, vel, isNoise);
        //Voice* v = new Voice(note, vel, isNoise);
        v->prepareToPlay(spec);
        if(!isNoise) v->initializeOscillator(oscType);
        else v->initializeNoise(noiseType);
        v->setADSR(envParams, ADSRDepth);
        v->setEnvRouting(envToVol, envToDist, envToFilter);
        v->setFilter(filterType, curCutoff, curRes, keytrack, keytrackAmount);
        v->setPitch(pitchOffset);
        v->noteOn();
        voices.push_back(std::move(v));
        if(voices.size() > 8) {
            voices.erase(voices.begin());
        }
    }
    if(midiEvent.isNoteOff()) {
        const auto note = midiEvent.getNoteNumber();
        for(int i=0; i<voices.size(); i++) {
            if(voices[i]->getPitch() == note && !voices[i]->isRelease()) {
                voices[i]->noteOff();
            }
        }
    }
    if(midiEvent.isAllNotesOff()) {
        for (int i=0; i<voices.size(); i++) {
            voices[i]->noteOff();
        }
    }
}
 
}
