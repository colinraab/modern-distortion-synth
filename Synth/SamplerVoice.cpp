/*
  ==============================================================================

    SamplerVoice.cpp
    Created: 5 Feb 2024 3:39:06pm
    Author:  Colin Raab

  ==============================================================================
*/

#include "SamplerVoice.h"

namespace Colin {

SamplerVoice::SamplerVoice(int pitch, int vel) {
    this->pitch = pitch;
    this->vel = vel;
    sample = new juce::AudioBuffer<float>();
    ladder = new juce::dsp::LadderFilter<float>();
}

SamplerVoice::~SamplerVoice() {
    delete ladder;
    delete sample;
}

void SamplerVoice::prepareToPlay(juce::dsp::ProcessSpec spec)
{
    sampleRate = spec.sampleRate;
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

void SamplerVoice::addSound(std::unique_ptr<juce::AudioFormatReader>& formatReader) {
    sample->setSize(2, formatReader->lengthInSamples);
    formatReader->read(sample, 0, formatReader->lengthInSamples, 0, true, true);
    sampleSampleRate = formatReader->sampleRate;
    setFrequency(midiToFreq(pitch + pitchOffset));
}

void SamplerVoice::renderVoice(std::unique_ptr<juce::AudioBuffer<float>>& buffer, juce::MidiBuffer& midiMessages, int startSample, int endSample) {
    if(!active) return;
        
    if(!env.isActive()) {
        if(envToVol) {
            active = false;
            return;
        }
    }
    
    getEnvSamples(buffer->getNumSamples());
    
    auto* firstChannel = buffer->getWritePointer(0);

    for (auto sample = startSample; sample < endSample; sample++) {
        firstChannel[sample] += getSample() * normVelocity(vel);
    }
    for (auto channel = 1; channel < buffer->getNumChannels(); channel++) {
        std::copy(firstChannel + startSample, firstChannel + endSample, buffer->getWritePointer(channel) + startSample);
    }
    
    if(envToVol) {
        buffer->applyGainRamp(0, buffer->getNumSamples(), envSampleStart, envSampleEnd);
    }
}

void SamplerVoice::setFilter(int type, float cutoff, float res, bool key, float ktA) {
    keytrack = key;
    keytrackAmount = (ktA + 1) / 100;
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
    return A4_FREQ * std::powf(2, (static_cast<float>(midiNote) - A4_MIDINOTE) / NOTES_IN_OCTAVE);
}

void SamplerVoice::processFilter(std::unique_ptr<juce::AudioBuffer<float>>& buffer) {
    if(!active) return;
    float cutoff = curCutoff;
    if(keytrack) {
        float newcutoff = midiToFreq(pitch) * keytrackAmount + curCutoff;
        cutoff = newcutoff < 20000 ? newcutoff : 20000;
        ladder->setCutoffFrequencyHz(cutoff);
        TPT.setCutoffFrequency(cutoff);
    }
    if(envToFilter) {
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

void SamplerVoice::setPitchOffset(int offset) {
    if(pitchOffset == offset) return;
    pitchOffset = offset;
    setFrequency(midiToFreq(pitch + pitchOffset));
}

void SamplerVoice::setLoop(bool isLoop) {
    loop = isLoop;
}

float SamplerVoice::normVelocity(int vel) {
    return (vel / 160.f) + 0.2f;
}

void SamplerVoice::setFrequency(float frequency) {
    indexIncrement = (frequency / 440.0) * sampleSampleRate / static_cast<float>(sampleRate);
}

float SamplerVoice::interpolateLinearly() {
    const auto truncatedIndex = static_cast<int>(index) % sample->getNumSamples();
    const auto nextIndex = (truncatedIndex + 1) % sample->getNumSamples();
    const auto nextIndexWeight = index - static_cast<float>(truncatedIndex);
    const auto truncatedIndexWeight = 1.f - nextIndexWeight;
    return truncatedIndexWeight * sample->getSample(0,truncatedIndex) + nextIndexWeight * sample->getSample(0,nextIndex);
}

float SamplerVoice::getSample() {
    if(static_cast<int>(index + 2) > sample->getNumSamples() && loop == false) {
        active = false;
        return 0.f;
    }
    auto newSample = interpolateLinearly();
    index += indexIncrement;
    index = std::fmod(index, static_cast<float>(sample->getNumSamples()));
    return newSample;
}

void SamplerVoice::setRepitch(bool shouldRepitch) {
    repitch = shouldRepitch;
    if(!repitch) {
        // middle C = 60
        setFrequency(midiToFreq(60+pitchOffset));
    }
}

}
