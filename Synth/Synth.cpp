/*
  ==============================================================================

    Synth.cpp
    Created: 7 Mar 2023 3:37:55pm
    Author:  Colin Raab

  ==============================================================================
*/

#include "Synth.h"

namespace Colin  {

Synth::Synth() {
    //default
}

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
    if(this->pitch != pitch) {
        this->pitch = pitch;
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

void Synth::processBuffer(juce::AudioBuffer<float>* buffer, juce::MidiBuffer& midiMessages, int i)
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

void Synth::processBufferFM(juce::AudioBuffer<float>* carrierBuffer, juce::AudioBuffer<float>* modBuffer, juce::MidiBuffer& midiMessages, int i)
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

void Synth::processDist(juce::AudioBuffer<float>* buffer, int i) {
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
    return A4_FREQ * std::powf(2, (static_cast<float>(midiNote) - A4_MIDINOTE + pitch) / NOTES_IN_OCTAVE);
}

void Synth::updateFreqs() {
    for(int i=0; i<voices.size(); i++) {
        voices[i]->setPitch(static_cast<int>(pitch));
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
        v->setPitch(pitch);
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






//-------------------------------------------------
/*
 
 void Synth::renderBuffers(std::vector<juce::AudioBuffer<float>*>& buffers, int startSample, int endSample) {
     for(int i=0; i<voices.size(); i++) {
         voices[i]->renderVoice(buffers[i], startSample, endSample);
     }
 }
 
 void Synth::processBuffersFM(std::vector<juce::AudioBuffer<float>*>& carrierBuffers, std::vector<juce::AudioBuffer<float>*>& modBuffers, juce::MidiBuffer& midiMessages)
 {
     auto currentSample = 0;
     for (const auto midiMessage : midiMessages) {
         const auto midiEvent = midiMessage.getMessage();
         const auto midiEventSample = static_cast<int>(midiEvent.getTimeStamp());
         renderFM(carrierBuffers, modBuffers, currentSample, midiEventSample);
         currentSample = midiEventSample;
         handleMidiEvent(midiEvent);
     }
     renderFM(carrierBuffers, modBuffers, currentSample, carrierBuffers[0]->getNumSamples());
     processDist(carrierBuffers);
     //processFilters(carrierBuffers);
 }
 
 void Synth::renderFM(std::vector<juce::AudioBuffer<float>*>& carrierBuffers, std::vector<juce::AudioBuffer<float>*>& modBuffers, int startSample, int endSample) {
     for (int i=0; i<oscillators.size(); i++) {
         if(oscillators[i].isPlaying()) {
             if(!envs[i].isActive()) {
                 if(envToVol) {
                     oscillators[i].stop();
                     curVoices.remove(i);
                     continue;
                 }
             }
             auto* firstChannel = carrierBuffers[i]->getWritePointer(0);
             for (auto sample = startSample; sample < endSample; sample++) {
                 if(envToVol) {
                     float envSample = envs[i].getNextSample();
                     float fmSample = modBuffers[i]->getReadPointer(0)[sample];
                     //oscillators[i].setFM(fmSample * FMdepth * oscillators[i].getFrequency());
                     oscillators[i].setPM(fmSample * FMdepth);
                     firstChannel[sample] += oscillators[i].getSample() * envSample * oscVol;
                 }
                 else {
                     float fmSample = modBuffers[i]->getReadPointer(0)[sample];
                     oscillators[i].setPM(fmSample * FMdepth);
                     firstChannel[sample] += oscillators[i].getSample() * oscVol;
                 }
             }
             for (auto channel = 1; channel < carrierBuffers[i]->getNumChannels(); channel++) {
                 std::copy(firstChannel + startSample, firstChannel + endSample, carrierBuffers[i]->getWritePointer(channel) + startSample);
             }
         }
     }
 }
 
 void Synth::processBuffers(std::vector<juce::AudioBuffer<float>*>& buffers, juce::MidiBuffer& midiMessages)
 {
     auto currentSample = 0;
     for (const auto midiMessage : midiMessages) {
         const auto midiEvent = midiMessage.getMessage();
         const auto midiEventSample = static_cast<int>(midiEvent.getTimeStamp());
         renderBuffers(buffers, currentSample, midiEventSample);
         currentSample = midiEventSample;
         handleMidiEvent(midiEvent);
     }
     renderBuffers(buffers, currentSample, buffers[0]->getNumSamples());
     processDist(buffers);
     for(int i=0; i<voices.size(); i++) {
         voices[i]->processFilter(buffers[i]);
     }
 }
 
 void Synth::processDist(std::vector<juce::AudioBuffer<float>*>& buffers) {
     if(distType == 1) return;
     for(int i=0; i<voices.size(); i++) {
         if(envToDist) {
             float envSample = voices[i]->getEnvSample();
             dist.setEnv(envSample, ADSRDepth);
             if(!envToVol && !envToFilter) {
                 for(int j=0; j<buffers[i]->getNumSamples() - 2; j++) {
                     envSample = voices[i]->getEnvSample();
                 }
             }
         }
         if(distType == 2) dist.processBufferWaveshaper(*buffers[i], bezier);
         else dist.processBuffer(*buffers[i]);
     }
 }
 
 void Synth::setNoteOff(int note) {
     if(oscillators[note].isPlaying()) {
         oscillators[note].stop();
         curVoices.remove(note);
         noiseIsPlaying[note] = false;
     }
     else if(oscillators[note+128].isPlaying()) {
         oscillators[note+128].stop();
         curVoices.remove(note+128);
         noiseIsPlaying[note+128] = false;
     }
 }
 
 
void Synth::processBuffer(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    auto currentSample = 0;
    for (const auto midiMessage : midiMessages) {
        const auto midiEvent = midiMessage.getMessage();
        const auto midiEventSample = static_cast<int>(midiEvent.getTimeStamp());
        //if(isNoise) renderNoise(buffer, currentSample, midiEventSample);
        render(buffer, currentSample, midiEventSample);
        currentSample = midiEventSample;
        handleMidiEvent(midiEvent);
    }
    //if(isNoise) renderNoise(buffer, currentSample, buffer.getNumSamples());
    render(buffer, currentSample, buffer.getNumSamples());
}

void Synth::render(juce::AudioBuffer<float>& buffer, int startSample, int endSample) {
    auto* firstChannel = buffer.getWritePointer(0);
    for (int i=0; i<oscillators.size(); i++) {
        if(oscillators[i].isPlaying()) {
            if(!envs[i].isActive()) {
                oscillators[i].stop();
                continue;
            }
            for (auto sample = startSample; sample < endSample; sample++) {
                float envSample = envs[i].getNextSample();
                firstChannel[sample] += TPTs[i].processSample(0, oscillators[i].getSample() * envSample * 0.2);
            }
        }
    }
    for (auto channel = 1; channel < buffer.getNumChannels(); channel++) {
        std::copy(firstChannel + startSample, firstChannel + endSample, buffer.getWritePointer(channel) + startSample);
    }
}

void Synth::renderFM(juce::dsp::AudioBlock<float>& block, juce::dsp::AudioBlock<float>& modBlock, int startSample, int endSample) {
    auto* firstChannel = block.getChannelPointer(0);
    auto* firstChannelMod = modBlock.getChannelPointer(0);
    for (auto& oscillator : oscillators) {
        if(oscillator.isPlaying()) {
            for (auto sample = startSample; sample < endSample; sample++) {
                oscillator.setFM(firstChannelMod[sample]);
                firstChannel[sample] += oscillator.getSample() * 0.2;
            }
        }
    }
    for (auto channel = 1; channel < block.getNumChannels(); channel++) {
        std::copy(firstChannel + startSample, firstChannel + endSample, block.getChannelPointer(channel) + startSample);
    }
}

void Synth::processBlockFM(juce::dsp::AudioBlock<float>& block, juce::dsp::AudioBlock<float>& modBlock, juce::MidiBuffer& midiMessages)
{
    auto currentSample = 0;
    for (const auto midiMessage : midiMessages) {
        const auto midiEvent = midiMessage.getMessage();
        const auto midiEventSample = static_cast<int>(midiEvent.getTimeStamp());
        renderFM(block, modBlock, currentSample, midiEventSample);
        currentSample = midiEventSample;
        handleMidiEvent(midiEvent);
    }
    renderFM(block, modBlock, currentSample, block.getNumSamples());
}

void Synth::processBlock(juce::dsp::AudioBlock<float>& block, juce::MidiBuffer& midiMessages)
{
    auto currentSample = 0;
    for (const auto midiMessage : midiMessages) {
        const auto midiEvent = midiMessage.getMessage();
        const auto midiEventSample = static_cast<int>(midiEvent.getTimeStamp());
        render(block, currentSample, midiEventSample);
        currentSample = midiEventSample;
        handleMidiEvent(midiEvent);
    }
    render(block, currentSample, block.getNumSamples());
}

void Synth::render(juce::dsp::AudioBlock<float>& block, int startSample, int endSample) {
    auto* firstChannel = block.getChannelPointer(0);
    for (auto& oscillator : oscillators) {
        if(oscillator.isPlaying()) {
            for (auto sample = startSample; sample < endSample; sample++) {
                firstChannel[sample] += oscillator.getSample() * 0.2;
            }
        }
    }
    for (auto channel = 1; channel < block.getNumChannels(); channel++) {
        std::copy(firstChannel + startSample, firstChannel + endSample, block.getChannelPointer(channel) + startSample);
    }
}
*/
 
 
}
