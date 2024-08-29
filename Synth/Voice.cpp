/*
  ==============================================================================

    Voice.cpp
    Created: 28 Jan 2024 4:55:52pm
    Author:  Colin Raab

  ==============================================================================
*/

#include "Voice.h"

namespace Colin  {

Voice::Voice(int p, int v, bool n) {
    pitch = p;
    vel = v;
    noise = n;
    ladder = new juce::dsp::LadderFilter<float>();
}

Voice::~Voice() {
    delete oscillator;
    delete ladder;
}

void Voice::prepareToPlay(juce::dsp::ProcessSpec spec)
{
    sampleRate = spec.sampleRate;
    initializeOscillator(Oscillator_Type::sine);
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

void Voice::initializeOscillator(Oscillator_Type osc) {
    oscType = osc;
    const auto wavetable = getWavetable();
    oscillator = new WavetableOscillator(wavetable, sampleRate, pitch);
}

void Voice::initializeNoise(Noise_Type noi) {
    noise = true;
    noiseType = noi;
}

std::vector<float> Voice::getWavetable() {
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

std::vector<float> Voice::getNoise() {
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

void Voice::setPitch(int p) {
    pitchOffset = p;
    oscillator->setPitch(p + pitch);
}

int Voice::getPitch() {
    return pitch;
}

void Voice::setVelocity(int v) {
    vel = v;
}

bool Voice::isPlaying() {
    return active;
}

bool Voice::isNoise() {
    return noise;
}

void Voice::setNoise(bool isNoise) {
    noise = isNoise;
}

void Voice::setADSR(juce::ADSR::Parameters envParams, float depth) {
    env.setParameters(envParams);
    ADSRDepth = depth;
}

void Voice::noteOn() {
    env.noteOn();
}

void Voice::noteOff() {
    env.noteOff();
    release = true;
}

bool Voice::isRelease() {
    return release;
}

void Voice::setVol(float v) {
    //vol = v;
}

float Voice::normVelocity(int vel) {
    return (vel / 160.f) + 0.2f;
}

void Voice::setFilter(int type, float cutoff, float res, bool key, float ktA) {
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

void Voice::setEnvRouting(bool v, bool d, bool f) {
    envToVol = v;
    envToFilter = f;
    envToDist = d;
}

float Voice::getEnvSample() {
    return envSampleStart;
}

float Voice::renderNoise() {
    if(noiseType == Noise_Type::gauss) {
        return (((float)rand() / (float)RAND_MAX ) - 0.5);
    }
    else if(noiseType == Noise_Type::binary) {
        float ran = (((float)rand() / (float)RAND_MAX ) - 0.5);
        float sample;
        if(ran >= 0) sample = 0.5;
        else sample = -0.5;
        return sample;
    }
    else if(noiseType == Noise_Type::hp) {
        float ran = (((float)rand() / (float)RAND_MAX ) - 0.5);
        float y = -0.5 * ran + 0.5 * prevHPNoiseSample;
        prevHPNoiseSample = ran;
        return y;
    }
    else if(noiseType == Noise_Type::lp) {
        float ran = (((float)rand() / (float)RAND_MAX ) - 0.5);
        float y = 0.5 * ran - 0.5 * prevLPNoiseSample;
        prevLPNoiseSample = y;
        return y;
    }
}

void Voice::renderVoice(juce::AudioBuffer<float>* buffer, int startSample, int endSample) {
    if(!active) return;
    
    auto* firstChannel = buffer->getWritePointer(0);
    getEnvSamples(endSample - startSample);

    if(!noise) {
        if(!env.isActive()) {
            if(envToVol) {
                oscillator->stop();
                active = false;
                return;
            }
        }
        for (auto sample = startSample; sample < endSample; sample++) {
            firstChannel[sample] += oscillator->getSample() * normVelocity(vel);
        }
        for (auto channel = 1; channel < buffer->getNumChannels(); channel++) {
            std::copy(firstChannel + startSample, firstChannel + endSample, buffer->getWritePointer(channel) + startSample);
        }
    }
        
    if(noise) {
        if(!env.isActive()) {
            if(envToVol) {
                active = false;
                return;
            }
        }
        for (auto sample = startSample; sample < endSample; sample++) {
            firstChannel[sample] += renderNoise() * normVelocity(vel);
        }
        for (auto channel = 1; channel < buffer->getNumChannels(); channel++) {
            std::copy(firstChannel + startSample, firstChannel + endSample, buffer->getWritePointer(channel) + startSample);
        }
    }
    if(envToVol) {
        buffer->applyGainRamp(startSample, endSample-startSample, envSampleStart, envSampleEnd);
    }
}

void Voice::renderVoiceFM(juce::AudioBuffer<float>* carrierBuffer, juce::AudioBuffer<float>* modBuffer, int startSample, int endSample, float depth) {
    if(!active) return;
    
    auto* firstChannel = carrierBuffer->getWritePointer(0);
    float fmSample = 0.f;
    getEnvSamples(endSample - startSample);
    
    if(!env.isActive()) {
        if(envToVol) {
            oscillator->stop();
            active = false;
        }
    }
    for (auto sample = startSample; sample < endSample; sample++) {
        fmSample = modBuffer->getSample(0, sample);
        oscillator->setPM(fmSample * depth);
        firstChannel[sample] += oscillator->getSample() * normVelocity(vel);
        
    }
    for (auto channel = 1; channel < carrierBuffer->getNumChannels(); channel++) {
        std::copy(firstChannel + startSample, firstChannel + endSample, carrierBuffer->getWritePointer(channel) + startSample);
    }
    if(envToVol) {
        carrierBuffer->applyGainRamp(startSample, endSample-startSample, envSampleStart, envSampleEnd);
    }
}

void Voice::getEnvSamples(int numSamples) {
    envSampleStart = env.getNextSample();
    for(int j=0; j<numSamples-2; j++) {
        env.getNextSample();
    }
    envSampleEnd = env.getNextSample();
    //cycleEnv = true;
}

float Voice::midiToFreq(int midiNote)
{
    constexpr float A4_FREQ = 440;
    constexpr float A4_MIDINOTE = 69;
    constexpr float NOTES_IN_OCTAVE = 12.f;
    return A4_FREQ * std::powf(2, (static_cast<float>(midiNote) - A4_MIDINOTE + pitch) / NOTES_IN_OCTAVE);
}

void Voice::processFilter(juce::AudioBuffer<float>* buffer) {
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

}
