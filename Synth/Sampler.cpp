/*
  ==============================================================================

    Sampler.cpp
    Created: 23 Mar 2023 2:46:57pm
    Author:  Colin Raab

  ==============================================================================
*/

#include "Sampler.h"

namespace Colin {

Sampler::Sampler() {
    formatManager.registerBasicFormats();
}

Sampler::~Sampler() {
    voices.clear();
}

void Sampler::prepareToPlay(juce::dsp::ProcessSpec spec) {
    this->sampleRate = spec.sampleRate;
    this->spec = spec;
    dist.setType(Distortion_Type::none);
    dist.setOutputGain(-3.f);
}

bool Sampler::isSampleLoaded() {
    return sampleLoaded;
}

void Sampler::setDistortion(int type, float input, float output, float coeff, float mix, AuxPort::Bezier* b) {
    distType = type;
    dist.setType(static_cast<Distortion_Type>(type));
    dist.setInputGain(input);
    dist.setOutputGain(output);
    dist.setCoeff(coeff);
    dist.setMix(mix);
    bezier = b;
}

void Sampler::setEnvRouting(bool vol, bool dist, bool filt) {
    if(envToVol != vol) {
        envToVol = vol;
    }
    if(envToFilter != filt) {
        envToFilter = filt;
    }
    envToDist = dist;
}

void Sampler::setPitch(float p, bool re) {
    pitch = static_cast<int>(std::roundf(p));
    repitch = re;
    for(int i=0; i<voices.size(); i++) {
        voices[i]->setPitchOffset(pitch);
        voices[i]->setRepitch(repitch);
    }
}

void Sampler::loadFile() {
    juce::File f = (juce::File::getSpecialLocation(juce::File::tempDirectory));
    juce::FileChooser chooser { "Load a sample", f};
    if(chooser.browseForFileToOpen()) {
        auto file = chooser.getResult();
        formatReader.reset(formatManager.createReaderFor(file));
        if (formatReader)
        {
            auto len = static_cast<int>(formatReader->lengthInSamples);
            waveform.setSize(1, len);
            sampleLength = len;
            formatReader->read(&waveform, 0, len, 0, true, false);
        }
    }
    sampleLoaded = true;
}

void Sampler::setNoteOff(int note) {
    for(int i=0; i<NUM_VOICES; i++) {
        if(curPitch[i] == note) {
            if(repitch) jucesamplers[i]->noteOff(1, 60 + pitch, 0, true);
            else jucesamplers[i]->noteOff(1, note, 0, true);
            curPitch[i] = -1;
            //envs[i].reset();
            enabled[note] = 0;
            lastVel[i] = 0;
            curSample[i] = 0;
        }
    }
}

void Sampler::processBuffer(std::unique_ptr<juce::AudioBuffer<float>>& buffer, juce::MidiBuffer& midiMessages, int i) {
    auto currentSample = 0;
    //std::unique_ptr<juce::AudioBuffer<float>> temp(new juce::AudioBuffer<float>{buffer->getNumChannels(), buffer->getNumSamples()});
    for (const auto midiMessage : midiMessages) {
        const auto midiEvent = midiMessage.getMessage();
        const auto midiEventSample = static_cast<int>(midiEvent.getTimeStamp());
        if(voices.size() >= 1) voices[i]->renderVoice(buffer, midiMessages, currentSample, midiEventSample);
        currentSample = midiEventSample;
        handleMidiEvent(midiEvent);
    }
    if(curSample[i] >= sampleLength) {
        juce::MidiMessage noteOff = juce::MidiMessage::noteOff(1, voices[i]->getPitch(), juce::uint8(0));
        midiMessages.addEvent(noteOff, 1);
        curSample[i] = 0;
        if(loop) {
            juce::MidiMessage noteOn = juce::MidiMessage::noteOn(1, voices[i]->getPitch(), lastVel[i]);
            midiMessages.addEvent(noteOn, 2);
        }
        else {
            curPitch[i] = -1;
        }
    }
    voices[i]->renderVoice(buffer, midiMessages, currentSample, buffer->getNumSamples());
    processDist(buffer, i);
    voices[i]->processFilter(buffer);
    curSample[i] += buffer->getNumSamples();
}

void Sampler::handleMidiEvent(const juce::MidiMessage& midiEvent) {
    if(midiEvent.isNoteOn()) {
        const auto note = midiEvent.getNoteNumber();
        int vel = midiEvent.getVelocity();
        for(int i=0; i<voices.size(); i++) {
            if(voices[i]->getPitch() == note && !voices[i]->isRelease()) {
                return;
            }
        }
        std::unique_ptr<SamplerVoice> v = std::make_unique<SamplerVoice>(note, vel);
        v->prepareToPlay(spec);
        v->addSound(formatReader);
        v->setADSR(envParams, ADSRDepth);
        v->setEnvRouting(envToVol, envToDist, envToFilter);
        v->setFilter(type, curCutoff, curRes, keytrack, keytrackAmount);
        v->setLoop(loop);
        v->setPitchOffset(pitch);
        v->setRepitch(repitch);
        v->noteOn();
        voices.push_back(std::move(v));
        if(voices.size() > 8) {
            voices.erase(voices.begin());
        }
        lastVel[voices.size()] = vel;
    }
    if(midiEvent.isNoteOff()) {
        const auto note = midiEvent.getNoteNumber();
        for(int i=0; i<voices.size(); i++) {
            if(voices[i]->getPitch() == note && !voices[i]->isRelease()) {
                voices[i]->noteOff();
                lastVel[i] = 0;
            }
        }
    }
    if(midiEvent.isAllNotesOff()) {
        for (int i=0; i<voices.size(); i++) {
            voices[i]->noteOff();
        }
    }
}

void Sampler::deleteVoice(int i) {
    if(voices.size() == 0) return;
    int note = voices[i]->getPitch();
    if(i+1>voices.size()) return;
    voices.erase(voices.begin()+i);
    curPitch[i] = -1;
    enabled[note] = 0;
    curSample[i] = 0;
}

void Sampler::processDist(std::unique_ptr<juce::AudioBuffer<float>>& buffer, float envSample) {
    if(distType == 1) return;
    if(envToDist) {
        dist.setEnv(envSample, ADSRDepth);
    }
    if(distType == 2) dist.processBufferWaveshaper(*buffer, bezier);
    else dist.processBuffer(*buffer);
}

void Sampler::setFilter(int type, float cutoff, float res, bool key, float ktA) {
    this->type = type;
    curCutoff = cutoff;
    curRes = res;
    keytrack = key;
    keytrackAmount = ktA;
    for(int i=0; i<voices.size(); i++) {
        voices[i]->setFilter(type, cutoff, res, key, ktA);
    }
}

void Sampler::setLoop(bool isLoop) {
    if(loop == isLoop) return;
    loop = isLoop;
    for(int i=0; i<voices.size(); i++) {
        voices[i]->setLoop(loop);
    }
}

void Sampler::setADSR(float atk, float dec, float sus, float rel, float depth) {
    ADSRDepth = depth;
    if(atk == envParams.attack && dec == envParams.decay && sus == envParams.sustain && rel == envParams.release)
        return;
    envParams.attack = atk;
    envParams.decay = dec;
    envParams.sustain = sus;
    envParams.release = rel;
}

float Sampler::midiToFreq(int midiNote)
{
    constexpr float A4_FREQ = 440;
    constexpr float A4_MIDINOTE = 69;
    constexpr float NOTES_IN_OCTAVE = 12.f;
    return A4_FREQ * std::powf(2, (static_cast<float>(midiNote) - A4_MIDINOTE + pitch) / NOTES_IN_OCTAVE);
}

void Sampler::setSampleLength(float newLenPercent)
{
    sampleLength = waveform.getNumSamples() * newLenPercent;
    if(sampleLength < 100) sampleLength = 100;
}

}
