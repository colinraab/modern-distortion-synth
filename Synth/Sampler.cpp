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
    /*
    jucesamplers.resize(NUM_VOICES);
    envs.resize(NUM_VOICES);
    TPTs.resize(NUM_VOICES);
    ladders.resize(NUM_VOICES);
    for(int i=0; i<NUM_VOICES; i++) {
        jucesamplers[i] = new juce::Synthesiser;
        jucesamplers[i]->addVoice(new juce::SamplerVoice());
        ladders[i] = new juce::dsp::LadderFilter<float>();
    }
     */
}

Sampler::~Sampler() {
    voices.clear();
    delete formatReader;
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
        /*
        if(!envToFilter) {
            for(int i=0; i<NUM_VOICES; i++) {
                ladders[i]->setCutoffFrequencyHz(curCutoff);
                TPTs[i].setCutoffFrequency(curCutoff);
            }
        }
         */
        envToFilter = filt;
    }
    envToDist = dist;
}

void Sampler::setPitch(float pitch, bool re) {
    this->pitch = pitch;
    repitch = re;
}

void Sampler::loadFile() {
    juce::File f = (juce::File::getSpecialLocation(juce::File::tempDirectory));
    //juce::BigInteger range;
    //range.setRange(0, 128, true);
    juce::FileChooser chooser { "Load a sample", f};
    if(chooser.browseForFileToOpen()) {
        auto file = chooser.getResult();
        //std::unique_ptr<juce::AudioFormatReader> formatReader(formatManager.createReaderFor(file));
        formatReader = formatManager.createReaderFor(file);
        if (formatReader)
        {
            for(int i=0; i<NUM_VOICES; i++) {
                //jucesamplers[i]->addSound(new juce::SamplerSound("Sample", *formatReader, range, 60, 0.01, 0.3, 10.0));
            }
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

void Sampler::processBuffer(juce::AudioBuffer<float>* buffer, juce::MidiBuffer& midiMessages, int i) {
    auto currentSample = 0;
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
    
    /*
    if(nextVoice < 0) nextVoice *= -1;
    if(!repitch || pitch != 0) {
        auto processedMidi = sortMessages(midiMessages, i);
        auto repitchedMidi = repitchMessages(processedMidi);
        if(curPitch[i] != -1) {
            voices[i]->renderVoice(buffer, repitchedMidi);
            //jucesamplers[i]->renderNextBlock(*buffer, repitchedMidi, 0, buffer->getNumSamples());
            curSample[i] += buffer->getNumSamples();
        }
    }
    else {
        auto processedMidi = sortMessages(midiMessages, i);
        if(curPitch[i] != -1) {
            voices[i]->renderVoice(buffer, processedMidi);

            //jucesamplers[i]->renderNextBlock(*buffer, processedMidi, 0, buffer->getNumSamples());
            curSample[i] += buffer->getNumSamples();
        }
    }
    float envSample = voices[i]->returnEnvSample();
    //processVol(buffer, i);
    processDist(buffer, envSample);
    voices[i]->processFilter(buffer);
     */
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
        std::unique_ptr<SamplerVoice> v = std::make_unique<SamplerVoice>(midiEvent.getNoteNumber());
        v->prepareToPlay(spec);
        v->addSound(formatReader);
        v->setADSR(envParams, ADSRDepth);
        v->setEnvRouting(envToVol, envToDist, envToFilter);
        v->setFilter(type, curCutoff, curRes, keytrack, keytrackAmount);
        //v->setPitch(pitch);
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

void Sampler::processDist(juce::AudioBuffer<float>* buffer, float envSample) {
    if(distType == 1) return;
    if(envToDist) {
        dist.setEnv(envSample, ADSRDepth);
    }
    if(distType == 2) dist.processBufferWaveshaper(*buffer, bezier);
    else dist.processBuffer(*buffer);
}

juce::MidiBuffer Sampler::sortMessages(juce::MidiBuffer& midiMessages, int voice) {
    juce::MidiBuffer processedMidi;
    
    if(curSample[voice] >= sampleLength && curPitch[voice] != -1) {
        juce::MidiMessage noteOff = juce::MidiMessage::noteOff(1, curPitch[voice], juce::uint8(0));
        processedMidi.addEvent(noteOff, 1);
        curSample[voice] = 0;
        if(loop) {
            juce::MidiMessage noteOn = juce::MidiMessage::noteOn(1, curPitch[voice], lastVel[voice]);
            processedMidi.addEvent(noteOn, 2);
        }
        else {
            curPitch[voice] = -1;
        }
    }
    
    for (const auto midiMessage : midiMessages) {
        const auto midiEvent = midiMessage.getMessage();
        const auto midiEventSample = static_cast<int>(midiEvent.getTimeStamp());
        if(midiEvent.isNoteOn()) {
            if(nextVoice == voice) {
                std::unique_ptr<SamplerVoice> v = std::make_unique<SamplerVoice>(midiEvent.getNoteNumber());
                v->prepareToPlay(spec);
                v->addSound(formatReader);
                v->setADSR(envParams, ADSRDepth);
                v->setEnvRouting(envToVol, envToDist, envToFilter);
                v->setFilter(type, curCutoff, curRes, keytrack, keytrackAmount);
                //v->setPitch(pitch);
                v->noteOn();
                voices.push_back(std::move(v));
                if(voices.size() > NUM_VOICES) {
                    voices.erase(voices.begin());
                    nextVoice--;
                }
                
                curPitch[voice] = midiEvent.getNoteNumber();
                processedMidi.addEvent(midiEvent, midiEventSample);
                enabled[midiEvent.getNoteNumber()] = 1;
                lastVel[voice] = midiEvent.getVelocity();
                nextVoice = (voice + 1) % NUM_VOICES;
            }
            /*
            if(nextVoice == voice) {
                if(curPitch[voice] != -1) {
                    envs[voice].reset();
                }
                curPitch[voice] = midiEvent.getNoteNumber();
                enabled[midiEvent.getNoteNumber()] = 1;
                processedMidi.addEvent(midiEvent, midiEventSample);
                envs[voice].noteOn();
                lastVel[voice] = midiEvent.getVelocity();
                nextVoice = -1 * ((voice + 2) % 3);
            }
             */
        }
        if(midiEvent.isNoteOff()) {
            auto note = midiEvent.getNoteNumber();
            if(voices[voice]->getPitch() == note && !voices[voice]->isRelease()) {
                voices[voice]->noteOff();
                curPitch[voice] = -1;
                processedMidi.addEvent(midiEvent, midiEventSample);
                enabled[midiEvent.getNoteNumber()] = 0;
                lastVel[voice] = 0;
            }
        }
    }
    return processedMidi;
}

juce::MidiBuffer Sampler::repitchMessages(juce::MidiBuffer& midiMessages) {
    juce::MidiBuffer processedMidi;
    for (const auto midiMessage : midiMessages) {
        const auto midiEvent = midiMessage.getMessage();
        const auto midiEventSample = static_cast<int>(midiEvent.getTimeStamp());
        if(midiEvent.isNoteOn()) {
            juce::MidiMessage noteOn;
            if(!repitch) noteOn = juce::MidiMessage::noteOn(midiEvent.getChannel(), 60 + pitch, midiEvent.getVelocity());
            else noteOn = juce::MidiMessage::noteOn(midiEvent.getChannel(), midiEvent.getNoteNumber() + pitch, midiEvent.getVelocity());
            processedMidi.addEvent(noteOn, midiEventSample);
        }
        if(midiEvent.isNoteOff()) {
            juce::MidiMessage noteOff;
            if(!repitch) noteOff = juce::MidiMessage::noteOff(midiEvent.getChannel(), 60 + pitch, midiEvent.getVelocity());
            else noteOff = juce::MidiMessage::noteOff(midiEvent.getChannel(), midiEvent.getNoteNumber() + pitch, midiEvent.getVelocity());
            processedMidi.addEvent(noteOff, midiEventSample);
        }
    }
    return processedMidi;
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






/*
 void Sampler::processVol(juce::AudioBuffer<float>* buffer, int i) {
     if(curPitch[i] != -1) {
         if(envToVol) {
             buffer->applyGainRamp(0, buffer->getNumSamples(), envSampleStart, envSampleEnd);
         }
     }
 }
 
 void Sampler::setFilter(int type, float cutoff, float res, bool key, float ktA) {
     this->type = type;
     keytrack = key;
     keytrackAmount = ktA;
     if(type <= 3) {
         if(type != curTPTMode) {
             curTPTMode = type;
             auto t = juce::dsp::StateVariableTPTFilterType::lowpass; // type == 1
             if(type == 2) t = juce::dsp::StateVariableTPTFilterType::highpass;
             else if(type == 3) t = juce::dsp::StateVariableTPTFilterType::bandpass;
             for (int i=0; i<NUM_VOICES; i++) {
                 TPTs[i].setType(t);
             }
         }
     }
     else {
         if(type != curLadderMode) {
             curLadderMode = type;
             auto t = juce::dsp::LadderFilterMode::LPF12; // type == 4
             if(type == 5) t = juce::dsp::LadderFilterMode::LPF24;
             else if(type == 6) t = juce::dsp::LadderFilterMode::HPF12;
             else if(type == 7) t = juce::dsp::LadderFilterMode::HPF24;
             for (int i=0; i<NUM_VOICES; i++) {
                 ladders[i]->setMode(t);
             }
         }
     }
     if(cutoff != curCutoff) {
         curCutoff = cutoff;
         for (int i=0; i<NUM_VOICES; i++) {
             TPTs[i].setCutoffFrequency(cutoff);
             ladders[i]->setCutoffFrequencyHz(cutoff);
         }
     }
     if(res != curRes) {
         curRes = res;
         for (int i=0; i<NUM_VOICES; i++) {
             TPTs[i].setResonance(res);
             ladders[i]->setResonance(res);
         }
     }
 }
 
 void Sampler::processFilters(std::vector<juce::AudioBuffer<float>*>& buffers) {
     for (int i=0; i<NUM_VOICES; i++) {
         if(curPitch[i] != -1) {
             float cutoff = curCutoff;
             if(keytrack) {
                 float newcutoff = midiToFreq(curPitch[i]) * keytrackAmount + curCutoff;
                 cutoff = newcutoff < 20000 ? newcutoff : 20000;
                 ladders[i]->setCutoffFrequencyHz(cutoff);
                 TPTs[i].setCutoffFrequency(cutoff);
             }
             if(envToFilter) {
                 float envSample = envs[i].getNextSample();
                 ladders[i]->setCutoffFrequencyHz((cutoff * (1-ADSRDepth)) + (cutoff * envSample * ADSRDepth));
                 TPTs[i].setCutoffFrequency((cutoff * (1-ADSRDepth)) + (cutoff * envSample * ADSRDepth));
                 if(!envToVol) {
                     for(int j=0; j<buffers[i]->getNumSamples() - 1; j++) {
                         envSample = envs[i].getNextSample();
                     }
                 }
             }
             juce::dsp::AudioBlock<float> block(*buffers[i]);
             auto pc = juce::dsp::ProcessContextReplacing<float>(block);
             if(type <= 3) TPTs[i].process(pc);
             else ladders[i]->process(pc);
         }
     }
 }

 void Sampler::processFilters(juce::AudioBuffer<float>* buffer, int i) {
     if(curPitch[i] != -1) {
         float cutoff = curCutoff;
         if(keytrack) {
             float newcutoff = midiToFreq(curPitch[i]) * keytrackAmount + curCutoff;
             cutoff = newcutoff < 20000 ? newcutoff : 20000;
             ladders[i]->setCutoffFrequencyHz(cutoff);
             TPTs[i].setCutoffFrequency(cutoff);
         }
         if(envToFilter) {
             float envSample = envs[i].getNextSample();
             ladders[i]->setCutoffFrequencyHz((cutoff * (1-ADSRDepth)) + (cutoff * envSampleStart * ADSRDepth));
             TPTs[i].setCutoffFrequency((cutoff * (1-ADSRDepth)) + (cutoff * envSampleStart * ADSRDepth));
         }
         juce::dsp::AudioBlock<float> block(*buffer);
         auto pc = juce::dsp::ProcessContextReplacing<float>(block);
         if(type <= 3) TPTs[i].process(pc);
         else ladders[i]->process(pc);
     }
 }

 void Sampler::processVol(std::vector<juce::AudioBuffer<float>*>& buffers) {
     for (int i=0; i<NUM_VOICES; i++) {
         if(curPitch[i] != -1) {
             if(envToVol) {
                 float envSample = envs[i].getNextSample();
                 
                 for(int channel=0; channel<buffers[i]->getNumChannels(); channel++) {
                     auto writePointer = buffers[i]->getWritePointer(channel);
                     auto readPointer = buffers[i]->getReadPointer(channel);
                     for(int sample=0; sample<buffers[i]->getNumSamples(); sample++) {
                         writePointer[sample] = readPointer[sample] * envSample;
                     }
                 }
                 
                 for(int j=0; j<buffers[0]->getNumSamples()-1; j++) {
                     envSample = envs[i].getNextSample();
                 }
             }
         }
     }
 }
 
 void Sampler::getEnvSamples(int numSamples, int i) {
     if(!cycleEnv[i]) {
         envSampleStart = envs[i].getNextSample();
         for(int j=0; j<numSamples-2; j++) {
             envs[i].getNextSample();
         }
         envSampleEnd = envs[i].getNextSample();
         cycleEnv[i] = true;
     }
 }

 void Sampler::processDist(std::vector<juce::AudioBuffer<float>*>& buffers) {
     if(distType == 1) return;
     for (int i=0; i<NUM_VOICES; i++) {
         if(curPitch[i] != -1) {
             if(envToDist) {
                 float envSample = envs[i].getNextSample();
                 dist.setEnv(envSample, ADSRDepth);
                 if(!envToVol && !envToFilter) {
                     for(int j=0; j<buffers[i]->getNumSamples() - 2; j++) {
                         envSample = envs[i].getNextSample();
                     }
                 }
             }
             if(distType == 2) dist.processBufferWaveshaper(*buffers[i], bezier);
             else dist.processBuffer(*buffers[i]);
         }
     }
 }
 
 void Sampler::processBuffers(std::vector<juce::AudioBuffer<float>*>& buffers, juce::MidiBuffer& midiMessages) {
     if(nextVoice < 0) nextVoice *= -1;
     if(!repitch || pitch != 0) {
         //juce::MidiBuffer processedMidi = repitchMessages(midiMessages, repitch);
         for(int i=0; i<NUM_VOICES; i++) {
             auto processedMidi = sortMessages(midiMessages, i);
             const juce::MidiBuffer repitchedMidi = repitchMessages(processedMidi);
             if(curPitch[i] != -1) {
                 jucesamplers[i]->renderNextBlock(*buffers[i], repitchedMidi, 0, buffers[i]->getNumSamples());
                 curSample[i] += buffers[i]->getNumSamples();
             }
         }
     }
     else {
         for(int i=0; i<NUM_VOICES; i++) {
             auto processedMidi = sortMessages(midiMessages, i);
             if(curPitch[i] != -1) {
                 jucesamplers[i]->renderNextBlock(*buffers[i], processedMidi, 0, buffers[i]->getNumSamples());
                 curSample[i] += buffers[i]->getNumSamples();
             }
         }
     }
     processVol(buffers);
     processDist(buffers);
     //processFilters(buffers);
 }

*/

}
