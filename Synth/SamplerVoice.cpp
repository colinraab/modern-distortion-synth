/*
  ==============================================================================

    SamplerVoice.cpp
    Created: 5 Feb 2024 3:39:06pm
    Author:  Colin Raab

  ==============================================================================
*/

#include "SamplerVoice.h"

namespace Colin {

SamplerVoice::SamplerVoice(int pitch) {
    this->pitch = pitch;
    ladder = new juce::dsp::LadderFilter<float>();
    jucesampler = new juce::Synthesiser;
    jucesampler->addVoice(new juce::SamplerVoice());
}

SamplerVoice::~SamplerVoice() {
    delete jucesampler;
    delete ladder;
}

void SamplerVoice::prepareToPlay(juce::dsp::ProcessSpec spec)
{
    sampleRate = spec.sampleRate;
    jucesampler->setCurrentPlaybackSampleRate(sampleRate);
    env.setSampleRate(sampleRate);
    TPT.reset();
    TPT.prepare(spec);
    TPT.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    TPT.setCutoffFrequency(curCutoff);
    TPT.setCutoffFrequency(curRes);
    ladder->reset();
    ladder->prepare(spec);
    ladder->setMode(juce::dsp::LadderFilterMode::LPF12);
    ladder->setCutoffFrequencyHz(curCutoff);
    ladder->setResonance(curRes);
    ladder->setDrive(3.f);
}

void SamplerVoice::addSound(juce::AudioFormatReader* formatReader) {
    juce::BigInteger range;
    range.setRange(0, 128, true);
    jucesampler->addSound(new juce::SamplerSound("Sample", *formatReader, range, 60, 0.01, 0.3, 10.0));
}

void SamplerVoice::renderVoice(juce::AudioBuffer<float>* buffer, juce::MidiBuffer& midiMessages, int startSample, int endSample) {
    if(!active) return;
        
    if(!env.isActive()) {
        if(envToVol) {
            // DO SOMETHING
            active = false;
            return;
        }
    }
    
    getEnvSamples(buffer->getNumSamples());
    
    jucesampler->renderNextBlock(*buffer, midiMessages, startSample, endSample);
    
    if(envToVol) {
        buffer->applyGainRamp(0, buffer->getNumSamples(), envSampleStart, envSampleEnd);
    }
}

void SamplerVoice::setFilter(int type, float cutoff, float res, bool key, float ktA) {
    keytrack = key;
    keytrackAmount = ktA;
    filterType = type;
    if(type <= 3) {
        if(type != curTPTMode) {
            curTPTMode = type;
            auto t = juce::dsp::StateVariableTPTFilterType::lowpass; // type == 1
            if(type == 2) t = juce::dsp::StateVariableTPTFilterType::highpass;
            else if(type == 3) t = juce::dsp::StateVariableTPTFilterType::bandpass;
            TPT.setType(t);
        }
    }
    else {
        if(type != curLadderMode) {
            curLadderMode = type;
            auto t = juce::dsp::LadderFilterMode::LPF12; // type == 4
            if(type == 5) t = juce::dsp::LadderFilterMode::LPF24;
            else if(type == 6) t = juce::dsp::LadderFilterMode::HPF12;
            else if(type == 7) t = juce::dsp::LadderFilterMode::HPF24;
            ladder->setMode(t);
        }
    }
    if(cutoff != curCutoff) {
        curCutoff = cutoff;
        TPT.setCutoffFrequency(cutoff);
        ladder->setCutoffFrequencyHz(cutoff);
    }
    if(res != curRes) {
        curRes = res;
        TPT.setResonance(res);
        ladder->setResonance(res);
    }
}

void SamplerVoice::setADSR(juce::ADSR::Parameters envParams, float depth) {
    env.setParameters(envParams);
    ADSRDepth = depth;
}

float SamplerVoice::midiToFreq(int midiNote)
{
    constexpr float A4_FREQ = 440;
    constexpr float A4_MIDINOTE = 69;
    constexpr float NOTES_IN_OCTAVE = 12.f;
    return A4_FREQ * std::powf(2, (static_cast<float>(midiNote) - A4_MIDINOTE + pitch) / NOTES_IN_OCTAVE);
}

void SamplerVoice::processFilter(juce::AudioBuffer<float>* buffer) {
    if(!active) return;
    float cutoff = curCutoff;
    if(keytrack) {
        float newcutoff = midiToFreq(pitch) * keytrackAmount + curCutoff;
        cutoff = newcutoff < 20000 ? newcutoff : 20000;
        ladder->setCutoffFrequencyHz(cutoff);
        TPT.setCutoffFrequency(cutoff);
    }
    if(envToFilter) {
        //float envSample = env.getNextSample();
        ladder->setCutoffFrequencyHz((cutoff * (1-ADSRDepth)) + (cutoff * envSampleStart * ADSRDepth));
        TPT.setCutoffFrequency((cutoff * (1-ADSRDepth)) + (cutoff * envSampleStart * ADSRDepth));
    }
    juce::dsp::AudioBlock<float> block(*buffer);
    auto pc = juce::dsp::ProcessContextReplacing<float>(block);
    if(filterType <= 3) TPT.process(pc);
    else ladder->process(pc);
}

void SamplerVoice::setEnvRouting(bool v, bool d, bool f) {
    envToVol = v;
    envToFilter = f;
    envToDist = d;
}

void SamplerVoice::getEnvSamples(int numSamples) {
    envSampleStart = env.getNextSample();
    for(int j=0; j<numSamples-2; j++) {
        env.getNextSample();
    }
    envSampleEnd = env.getNextSample();
    //cycleEnv = true;
}

float SamplerVoice::returnEnvSample() {
    return envSampleStart;
}

void SamplerVoice::noteOn() {
    env.noteOn();
}

void SamplerVoice::noteOff() {
    env.noteOff();
    release = true;
}

bool SamplerVoice::isRelease() {
    return release;
}

int SamplerVoice::getPitch() {
    return pitch;
}

void SamplerVoice::setLoop(bool isLoop) {
    loop = isLoop;
}

}
