/*
  ==============================================================================

    WavetableOsc.cpp
    Created: 5 Mar 2023 12:32:55pm
    Author:  Colin Raab

  ==============================================================================
*/

#include "WavetableOsc.h"

namespace Colin {

WavetableOscillator::WavetableOscillator(std::vector<float> wavetable, double sampleRate, int midi)
: wavetable{std::move(wavetable)}, sampleRate{sampleRate}, midi{midi}
{}

void WavetableOscillator::setFrequency(float frequency) {
    freq = frequency;
    indexIncrement = frequency * static_cast<float>(wavetable.size()) / static_cast<float>(sampleRate);
}

void WavetableOscillator::isNoise() {
    indexIncrement = 1;
}

void WavetableOscillator::setPitch(int midi) {
    this->midi = midi;
    setFrequency(midiToFreq(midi));
}

float WavetableOscillator::getFrequency() {
    return freq;
}

float WavetableOscillator::midiToFreq(int midiNote)
{
    if(midiNote < 0) midiNote = 0;
    if(midiNote > 128) midiNote = 128;
    constexpr float A4_FREQ = 440;
    constexpr float A4_MIDINOTE = 69;
    constexpr float NOTES_IN_OCTAVE = 12.f;
    return A4_FREQ * std::powf(2, (static_cast<float>(midiNote) - A4_MIDINOTE) / NOTES_IN_OCTAVE);
}

void WavetableOscillator::setFM(float modSample) {
    float newFreq = freq + modSample;
    if (newFreq < 0) newFreq = std::abs(newFreq);
    if (newFreq > (sampleRate / 2)) newFreq = newFreq - (sampleRate/2);
    indexIncrement = newFreq * static_cast<float>(wavetable.size()) / static_cast<float>(sampleRate);
}

void WavetableOscillator::setPM(float modSample) {
    index += modSample * indexIncrement;
    while(index < 0) index = wavetable.size() + index;
}

float WavetableOscillator::getSample() {
    auto sample = interpolateLinearly();
    index += indexIncrement;
    index = std::fmod(index, static_cast<float>(wavetable.size()));
    return sample;
}

float WavetableOscillator::getSampleRaw() {
    auto sample = wavetable[index];
    index += indexIncrement;
    index = std::fmod(index, static_cast<float>(wavetable.size()));
    return sample;
}

float WavetableOscillator::interpolateLinearly() {
    const auto truncatedIndex = static_cast<int>(index) % static_cast<int>(wavetable.size());
    const auto nextIndex = (truncatedIndex + 1) % static_cast<int>(wavetable.size());
    const auto nextIndexWeight = index - static_cast<float>(truncatedIndex);
    const auto truncatedIndexWeight = 1.f - nextIndexWeight;
    
    return truncatedIndexWeight * wavetable[truncatedIndex] + nextIndexWeight * wavetable[nextIndex];
}

void WavetableOscillator::stop() {
    index = 0.f;
    indexIncrement = 0.f;
}

bool WavetableOscillator::isPlaying() {
    return indexIncrement != 0.f;
}

}
