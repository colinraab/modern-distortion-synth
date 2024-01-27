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
    envs.resize(256);
    TPTs.resize(256);
    ladders.resize(256);
    noiseIsPlaying.resize(256);
    for(int i=0; i<OSCILLATORS_COUNT; i++) {
        ladders[i] = new juce::dsp::LadderFilter<float>();
        noiseIsPlaying[i] = false;
    }
    
}

Synth::~Synth() {
    for(int i=0; i<OSCILLATORS_COUNT; i++) {
        delete ladders[i];
    }
}

void Synth::prepareToPlay(juce::dsp::ProcessSpec spec)
{
    this->sampleRate = spec.sampleRate;
    initializeOscillators();
    dist.setType(Distortion_Type::none);
    dist.setOutputGain(-3.f);
    for(int i=0; i<oscillators.size(); i++) {
        envs[i].setSampleRate(sampleRate);
        TPTs[i].reset();
        TPTs[i].prepare(spec);
        TPTs[i].setType(juce::dsp::StateVariableTPTFilterType::lowpass);
        TPTs[i].setCutoffFrequency(curCutoff);
        TPTs[i].setCutoffFrequency(curRes);
        ladders[i]->reset();
        ladders[i]->prepare(spec);
        ladders[i]->setMode(juce::dsp::LadderFilterMode::LPF12);
        ladders[i]->setCutoffFrequencyHz(curCutoff);
        ladders[i]->setResonance(curRes);
        ladders[i]->setDrive(3.f);
    }
}

void Synth::initializeOscillators() {
    oscillators.clear();
    const auto wavetable = getWavetable();
    for(auto i = 0; i < OSCILLATORS_COUNT; i++) {
        oscillators.emplace_back(wavetable, sampleRate, i);
    }
}
/*
void Synth::initializeNoise() {
    oscillators.clear();
    const auto wavetable = getNoise();
    for(auto i = 0; i < OSCILLATORS_COUNT; i++) {
        oscillators.emplace_back(wavetable, sampleRate, i);
        oscillators[i].isNoise();
    }
}
*/
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
    envToFilter = f;
    envToDist = d;
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
        if(type != static_cast<int>(oscType)) {
            oscType = static_cast<Oscillator_Type>(type);
            initializeOscillators();
        }
    }
    else {
        if(type != static_cast<int>(noiseType)) {
            noiseType = static_cast<Noise_Type>(type);
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
    filterType = type;
    keytrack = key;
    keytrackAmount = ktA;
    if(type <= 3) {
        if(type != curTPTMode) {
            curTPTMode = type;
            auto t = juce::dsp::StateVariableTPTFilterType::lowpass; // type == 1
            if(type == 2) t = juce::dsp::StateVariableTPTFilterType::highpass;
            else if(type == 3) t = juce::dsp::StateVariableTPTFilterType::bandpass;
            for (int i=0; i<oscillators.size(); i++) {
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
            for (int i=0; i<oscillators.size(); i++) {
                ladders[i]->setMode(t);
            }
        }
    }
    if(cutoff != curCutoff) {
        curCutoff = cutoff;
        for (int i=0; i<oscillators.size(); i++) {
            TPTs[i].setCutoffFrequency(cutoff);
            ladders[i]->setCutoffFrequencyHz(cutoff);
        }
    }
    if(res != curRes) {
        curRes = res;
        for (int i=0; i<oscillators.size(); i++) {
            TPTs[i].setResonance(res);
            ladders[i]->setResonance(res);
        }
    }
}

void Synth::renderBuffers(std::vector<juce::AudioBuffer<float>*>& buffers, int startSample, int endSample) {
    for (int i=0; i<oscillators.size(); i++) {
        if(oscillators[i].isPlaying()) {
            if(!envs[i].isActive()) {
                if(envToVol) {
                    oscillators[i].stop();
                    curVoices.remove(i);
                    continue; 
                }
            }
            auto* firstChannel = buffers[i]->getWritePointer(0);
            for (auto sample = startSample; sample < endSample; sample++) {
                if(envToVol) {
                    float envSample = envs[i].getNextSample();
                    firstChannel[sample] += oscillators[i].getSample() * envSample * oscVol;
                }
                else firstChannel[sample] += oscillators[i].getSample() * oscVol;
            }
            for (auto channel = 1; channel < buffers[i]->getNumChannels(); channel++) {
                std::copy(firstChannel + startSample, firstChannel + endSample, buffers[i]->getWritePointer(channel) + startSample);
            }
        }
        
        if(isNoise && noiseIsPlaying[i]) {
            if(!envs[i].isActive()) {
                if(envToVol) {
                    noiseIsPlaying[i] = false;
                    //curVoices.remove(i);
                    continue;
                }
            }
            auto* firstChannel = buffers[i]->getWritePointer(0);
            for (auto sample = startSample; sample < endSample; sample++) {
                if(envToVol) {
                    float envSample = envs[i].getNextSample();
                    firstChannel[sample] += renderNoise() * envSample * oscVol;
                    //firstChannel[sample] += oscillators[i].getSampleRaw() * envSample * oscVol;
                }
                else firstChannel[sample] += renderNoise() * oscVol;
                //else firstChannel[sample] += oscillators[i].getSampleRaw() * oscVol;
            }
            for (auto channel = 1; channel < buffers[i]->getNumChannels(); channel++) {
                std::copy(firstChannel + startSample, firstChannel + endSample, buffers[i]->getWritePointer(channel) + startSample);
            }
        }
    }
}

float Synth::renderNoise() {
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
    processFilters(buffers);
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
    processFilters(carrierBuffers);
}

void Synth::processDist(std::vector<juce::AudioBuffer<float>*>& buffers) {
    if(distType == 1) return;
    for (int i=0; i<oscillators.size() / 2; i++) {
        if(oscillators[i].isPlaying() || noiseIsPlaying[i]) {
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
        if(oscillators[i+128].isPlaying() || noiseIsPlaying[i+128]) {
            if(envToDist) {
                float envSample = envs[i+128].getNextSample();
                dist.setEnv(envSample, ADSRDepth);
                if(!envToVol && !envToFilter) {
                    for(int j=0; j<buffers[i+128]->getNumSamples() - 2; j++) {
                        envSample = envs[i+128].getNextSample();
                    }
                }
            }
            if(distType == 2) dist.processBufferWaveshaper(*buffers[i+128], bezier);
            else dist.processBuffer(*buffers[i+128]);
        }
    }
}

void Synth::processFilters(std::vector<juce::AudioBuffer<float>*>& buffers) {
    for (int i=0; i<oscillators.size() / 2; i++) {
        if(oscillators[i].isPlaying() || noiseIsPlaying[i]) {
            float cutoff = curCutoff;
            if(keytrack) {
                float newcutoff = midiToFreq(i) * keytrackAmount + curCutoff;
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
            if(filterType <= 3) TPTs[i].process(pc);
            else ladders[i]->process(pc);
        }
        if(oscillators[i+128].isPlaying() || noiseIsPlaying[i+128]) {
            float cutoff = curCutoff;
            if(keytrack) {
                float newcutoff = midiToFreq(i) * keytrackAmount + curCutoff;
                cutoff = newcutoff < 20000 ? newcutoff : 20000;
                ladders[i+128]->setCutoffFrequencyHz(cutoff);
                TPTs[i+128].setCutoffFrequency(cutoff);
            }
            if(envToFilter) {
                float envSample = envs[i+128].getNextSample();
                ladders[i+128]->setCutoffFrequencyHz((cutoff * (1-ADSRDepth)) + (cutoff * envSample * ADSRDepth));
                TPTs[i+128].setCutoffFrequency((cutoff * (1-ADSRDepth)) + (cutoff * envSample * ADSRDepth));
                if(!envToVol) {
                    for(int j=0; j<buffers[i+128]->getNumSamples() - 1; j++) {
                        envSample = envs[i+128].getNextSample();
                    }
                }
            }
            juce::dsp::AudioBlock<float> block(*buffers[i+128]);
            auto pc = juce::dsp::ProcessContextReplacing<float>(block);
            if(filterType <= 3) TPTs[i+128].process(pc);
            else ladders[i+128]->process(pc);
        }
    }
    
}

float Synth::midiToFreq(int midiNote)
{
    constexpr float A4_FREQ = 440;
    constexpr float A4_MIDINOTE = 69;
    constexpr float NOTES_IN_OCTAVE = 12.f;
    return A4_FREQ * std::powf(2, (static_cast<float>(midiNote) - A4_MIDINOTE + pitch) / NOTES_IN_OCTAVE);
}

void Synth::updateFreqs() {
    for (int i=0; i<oscillators.size() / 2; i++) {
        if(oscillators[i].isPlaying()) oscillators[i].setPitch(i + pitch);
        if(oscillators[i+128].isPlaying()) oscillators[i+128].setPitch(i + pitch);
    }
}

void Synth::handleMidiEvent(const juce::MidiMessage& midiEvent) {
    if(midiEvent.isNoteOn()) {
        const auto note = midiEvent.getNoteNumber();
        if(isNoise) noiseIsPlaying[note] = true;
        else {
            oscillators[note].setPitch(note + pitch);
            oscillators[note+128].setPitch(note + pitch);
            if(envs[note].isActive()) {
                envs[note].noteOff();
                envs[note+128].noteOn();
                curVoices.push_back(note+128);
            }
            else if(envs[note+128].isActive()) {
                envs[note+128].noteOff();
                envs[note].noteOn();
                curVoices.push_back(note);
            }
            envs[note].noteOn();
            if(curVoices.size() >= 8) {
                oscillators[curVoices.front()].stop();
                curVoices.pop_front();
            }
            //curVoices.push_back(note);
        }
    }
    if(midiEvent.isNoteOff()) {
        const auto note = midiEvent.getNoteNumber();
        if(envs[note].isActive()) {
            envs[note].noteOff();
        }
        else if(envs[note+128].isActive()){
            envs[note+128].noteOff();
        }
        //else oscillators[note].stop();
    }
    if(midiEvent.isAllNotesOff()) {
        for (int i=0; i<oscillators.size(); i++) {
            oscillators[i].stop();
            envs[i].noteOff();
            noiseIsPlaying[i] = false;
        }
        curVoices.clear();
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

void Synth::setADSR(float atk, float dec, float sus, float rel, float depth) {
    ADSRDepth = depth;
    if(atk == envParams.attack && dec == envParams.decay && sus == envParams.sustain && rel == envParams.release)
        return;
    envParams.attack = atk;
    envParams.decay = dec;
    envParams.sustain = sus;
    envParams.release = rel;
    for(int i=0; i<envs.size(); i++) {
        //envs[i].reset();
        envs[i].setParameters(envParams);
    }
}







//-------------------------------------------------
/*
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
