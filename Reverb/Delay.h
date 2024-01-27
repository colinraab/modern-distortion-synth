#ifndef COLIN_DELAY_H
#define COLIN_DELAY_H
#include <math.h>
#include <vector>
#include <cstdlib>
#include "Mix_Matrix.h"
#include "../Synth/LFO.h"

/*
  ==============================================================================

    Delay.h
    Created: 1 Nov 2022 7:45:31pm
    Author:  Colin Raab
 
    very naive implementation at the moment, can definitely be optimized

  ==============================================================================
*/

namespace Colin {

class Buffer {
protected:
    int size;
    int position = 0;
    int chorusPos = 0;
    float shimmerMix = 0; /// value between 0 and 1
    float antiMix = 0; /// value between 0 and 1
    int doublePos = 0;
    Mix_Matrix matrix;
    std::vector<float> buffer;
    
public:
    Buffer() = default;
    Buffer(int size) {
        this->size = size;
        buffer.resize(size, 0);
    }
    
    ~Buffer() = default;
    
    void add(float value) {
        buffer[position] = value;
        position++;
        doublePos++;
        if (position == size) position = 0;
        if (doublePos == size * 2) doublePos = 0;
    }
    
    float get() {
        //if(position < 0 || position >= size) return 0;
        return buffer[position];
    }
    
    float getChorus(float time, int depth) {
        /// time is the scalar, depth is the amount of samples the chorus moves
        /// figure out how to prevent popping
        float offsetF = (time * depth) - (depth/2); /// bipolar modulation
        int offset = (int)offsetF;
        float remainder = offsetF - offset;
        int pos = position - size/2;
        if(pos < 0) pos += size;
        pos += offset;
        if(pos < 0) pos += size;
        if(pos >= size) pos -= size;
        /*
        if (chorusPos > position) pos+=size;
        chorusPos = pos * time;
        pos = chorusPos;
        if (pos >= size) pos -= size;
        if (pos == position) chorusPos = position;
        */
        float sample = cubicInter(pos, remainder);
        return sample;
    }
    
    float getShimmer() {
        int doublePos = position * 2;
        if(doublePos >= size) doublePos -= size;
        float singleCoeff = 0;
        float doubleCoeff = 0;
        matrix.cheapEnergyCrossfade(shimmerMix, doubleCoeff, singleCoeff);
        return buffer[position] * singleCoeff + buffer[doublePos] * doubleCoeff;
    }
    
    float getAntiShimmer() {
        int halfPos = doublePos / 2;
        float remainder = ((float)doublePos / 2.0) - halfPos;
        if(halfPos > size-1) halfPos = 0;
        float singleCoeff = 0;
        float halfCoeff = 0;
        matrix.cheapEnergyCrossfade(antiMix, halfCoeff, singleCoeff);
        return buffer[position] * singleCoeff + cubicInter(halfPos, remainder) * halfCoeff;
    }
    
    void resize(int newSize) {
        buffer.resize(newSize, 0);
        size = newSize;
        if(position >= newSize) {
            position = 0;
            doublePos = 0;
            reset();
        }
    }
    
    void setShimmerMix(float mix) {
        shimmerMix = mix;
    }
    
    void setAntiShimmerMix(float mix) {
        antiMix = mix;
    }
    
    void reset() {
        for(int i=0; i<size; i++) buffer[i] = 0;
    }
   
    float getInter(int pos) {
        float sample1 = buffer[pos];
        float sample2;
        if(pos == 0) sample2 = buffer[size-1];
        else sample2 = buffer[pos-1];
        float difference = sample1 - sample2;
        return sample2 + difference;
    }
    
    float getInter1(int pos) {
        float sample1 = buffer[pos];
        float sample2;
        if(pos==size-1) sample2 = buffer[0];
        else sample2 = buffer[pos+1];
        float difference = sample1 - sample2;
        return sample1 + difference;
    }
    
    int validPos(int pos) {
        if(pos < 0) return size + pos;
        else if(pos >= size) return pos - size;
        else return pos;
    }
    
    float cubicInter(int pos, float fractional) {
        float a = buffer[validPos(pos-2)];
        float b = buffer[validPos(pos-1)];
        float c = buffer[validPos(pos)];
        float d = buffer[validPos(pos+1)];
        float cbDiff = c-b;
        float k1 = (c-a) * 0.5;
        float k3 = k1 + (d-b) * 0.5 - cbDiff * 2;
        float k2 = cbDiff - k3 - k1;
        return b + fractional * (k1 + fractional * (k2 + fractional * k3));
    }
    
    /// unused functions
    float getAt(int pos) {
        int newpos = position - pos;
        if(newpos < 0) newpos += size;
        return buffer[newpos];
    }
    
    float get_last() {
        return buffer[size-1];
    }
};

class Single_Delay {
protected:
    Buffer buffer;
    int length;
    float decay = 0; /// 1 = no decay, 0 = instant decay
    int fs = 44100;
    
public:
    Single_Delay() = default;
    ~Single_Delay() = default;
    Single_Delay(int fs, float time, float decay) {
        this->fs = fs;
        length = (int)(fs * time);
        buffer.resize(length);
        this->decay = decay;
    }
    
    void prepareToPlay(int fs, float time, float decay) {
        this->fs = fs;
        length = fs * time;
        buffer.resize(length);
        buffer.reset();
        this->decay = decay;
    }
    
    void reset() {
        buffer.reset();
    }
    
    void delay(float sample) {
        buffer.add(sample);
    }
    
    void delayDecay(float sample) {
        buffer.add(sample * decay);
    }
    
    float get() {
        return buffer.get();
    }
    
    float getChorus(float lfo, int depth) {
        return buffer.getChorus(lfo, depth);
    }
    
    float getShimmer() {
        return buffer.getShimmer();
    }
    
    float getAntiShimmer() {
        return buffer.getAntiShimmer();
    }
    
    float getAt(int position) {
        return buffer.getAt(position);
    }
    
    void setTime(float newTime) {
        length = (int)(fs * newTime);
        buffer.resize(length);
    }
    
    void setTimeSamples(int bufferSize) {
        buffer.resize(bufferSize);
    }
    
    void setDecay(float decay) {
        this->decay = decay;
    }
    
    void setShimmerMix(float shimmerMix) {
        buffer.setShimmerMix(shimmerMix);
    }
    
    void setAntiShimmerMix(float antiMix) {
        buffer.setAntiShimmerMix(antiMix);
    }
};

class Multi_Delay {
protected:
    int fs = 44100;
    float decay = 0.85; /// 1 = no decay, 0 = instant decay
    float time = 150; /// in ms
    Mix_Matrix matrix;
    std::vector<Single_Delay> delays;
    int delaySamples[numChannels] = {0};
    LFO lfos[numChannels];
    float depth = 10;
    float shimmerMix = 0;
    float antiMix = 0;
    
public:
    Multi_Delay() = default;
    ~Multi_Delay() = default;
    Multi_Delay(int numDelays, int fs) {
        delays.resize(numChannels);
        for(int i=0; i<numChannels; i++) {
            delays[i].prepareToPlay(fs, 0, 1);
        }
    }
    
    void prepareToPlay(int fs, float time, float decay) {
        delays.resize(numChannels);
        float delaySec = time * 0.001;
        for(int i=0; i<numChannels; i++) {
            float r = i * 1.0 / numChannels;
            //delaySamples[i] = std::pow(2,r) * delaySec;
            float d = std::pow(2,r) * delaySec;
            delays[i].prepareToPlay(fs, d, decay);
            lfos[i].prepareToPlay(fs);
            lfos[i].setType(Colin::LFO_type::Sine);
            float rate = std::pow(2,r) / 2;
            lfos[i].setRate(rate);
        }
        this->time = time;
        this->decay = decay;
    }
    
    void setTime(float time) {
        float delaySec = time * 0.001;
        for(int i=0; i<numChannels; i++) {
            float r = i * 1.0 / numChannels;
            float d = std::pow(2,r) * delaySec;
            delays[i].setTime(d);
        }
    }
    
    void setShimmerMix(float shimmerMix) {
        if(this->shimmerMix == shimmerMix) return;
        this->shimmerMix = shimmerMix;
        if(shimmerMix == 0) return;
        for(int i=0; i<numChannels; i++) {
            float r = (numChannels - i) * 1.0 / numChannels;
            float m = std::pow(2,r) * shimmerMix;
            delays[i].setShimmerMix(m);
        }
    }
    
    void setAntiShimmerMix(float antiMix) {
        if(this->antiMix == antiMix) return;
        this->antiMix = antiMix;
        if(antiMix == 0) return;
        for(int i=0; i<numChannels; i++) {
            float r = (numChannels - i) * 1.0 / numChannels;
            float m = std::pow(2,r) * antiMix;
            delays[i].setAntiShimmerMix(m);
        }
    }
    
    void setLFODepth(float depth) {
        if(depth == this->depth) return;
        for(int i=0; i<numChannels; i++) {
            float r = i * 1.0 / numChannels;
            float d = std::pow(2,r) * depth;
            lfos[i].setDepth(d);
        }
        this->depth = depth;
    }
    
    void setDecay(float decay) {
        this->decay = decay;
    }
    
    void delay(int channel, float sample) {
        delays[channel].delay(sample);
    }
    
    float get(int channel) {
        return delays[channel].get();
    }
    
    data getAll() {
        data d;
        for(int i=0; i<numChannels; i++) {
            if(shimmerMix != 0) d.channels[i] = delays[i].getShimmer();
            else if(antiMix != 0) d.channels[i] = delays[i].getAntiShimmer();
            else d.channels[i] = delays[i].get();
        }
        return d;
    }
    
    void delayAll(data d) {
        for(int i=0; i<numChannels; i++) {
            delays[i].delay(d.channels[i]);
        }
    }
    
    data process(data input) {
        data output = getAll();
        //for(int i=0; i<numChannels; i++) output.channels[i] = delays[i].getAt(delaySamples[i]);
        data mixed = matrix.Householder(output);
        for(int i=0; i<numChannels; i++) {
            float sum = input.channels[i] + (mixed.channels[i] * decay * lfos[i].getValue());
            delay(i, sum);
        }
        return output;
    }
};

class DiffusionStep {
protected:
    bool flipPolarity[numChannels] = {false};
    std::vector<Single_Delay> delays;
    Mix_Matrix matrix;
    int delaySamples[numChannels] = {0};
    
public:
    int delayRange = 150; /// in ms
    
    DiffusionStep() = default;
    ~DiffusionStep() = default;
    
    void configure(int fs) {
        float delayTime = delayRange * 0.001;
        delays.resize(numChannels);
        for(int i=0; i<numChannels; i++) {
            float low = delayTime * i / numChannels;
            float high = delayTime * (i+1) / numChannels;
            //delaySamples[i] = randomInRange(low, high);
            float d = randomInRange(low, high);
            if(d<0.001) d = 0.001;
            delays[i].prepareToPlay(fs, d, 0);
            flipPolarity[i] = rand() % 2;
        }
    }
    
    data getAll() {
        data d;
        for(int i=0; i<numChannels; i++) {
            d.channels[i] = delays[i].get();
        }
        return d;
    }
    
    void delayAll(data r) {
        for(int i=0; i<numChannels; i++) {
            delays[i].delay(r.channels[i]);
        }
    }
    
    data process(data input) {
        delayAll(input);
        data output = getAll();
        //for(int i=0; i<numChannels; i++) output.channels[i] = delays[i].getAt(delaySamples[i]);
        data mixed = matrix.Hadamard(output);
        for(int i=0; i<numChannels; i++) {
            if(flipPolarity[i]) mixed.channels[i] *= -1;
        }
        return mixed;
    }
    
    float randomInRange(float a, float b) {
        float random = ((float) rand()) / (float) RAND_MAX;
        float diff = b - a;
        float r = random * diff;
        return a + r;
    }
};

class Diffuser {
protected:
    const static int numSteps = 4;
    std::vector<DiffusionStep> steps;
    
public:
    Diffuser() = default;
    ~Diffuser() = default;
    
    void prepareToPlay(float diffusionMS, int fs) {
        steps.resize(numSteps);
        for(int i=0; i<numSteps; i++) {
            steps[i].delayRange = diffusionMS;
            steps[i].configure(fs);
            diffusionMS *= 0.5;
        }
    }
    
    data process(data input) {
        data o = input;
        for(int i=0; i<numSteps; i++) {
            o = steps[i].process(o);
        }
        return o;
    }
};

class Chorus {
protected:
    int fs = 44100;
    static const int numTaps = 2; /// stereo currently, expand in the future
    LFO lfos[numTaps];
    Single_Delay left;
    Single_Delay right;
    float depth; /// value between 1 and 30 ms
    int sampleDepth; /// corresponding depth value in samples
    float mix = 0; /// value between 0 and 1
    float dry = 0;
    float wet = 0;
    float rate; /// value best between 0.3 and 1Hz 
    Mix_Matrix matrix;
    
    
public:
    Chorus() = default;
    ~Chorus() = default;
    void prepareToPlay(int fs) {
        this->fs = fs;
        left.prepareToPlay(fs, .1, 0);
        right.prepareToPlay(fs, .1, 0);
        for(int i=0; i<numTaps; i++) {
            lfos[i].prepareToPlay(fs);
            lfos[i].setRate(.4 + i/10);
            lfos[i].setDepth(100);
            lfos[i].setType(LFO_type::Triangle);
            lfos[i].start();
        }
    }
    
    void setDepth(float depth) {
        if(this->depth == depth) return;
        this->depth = depth;
        sampleDepth = fs * depth * .001;
    }
    
    void setRate(float rate) {
        if(this->rate == rate) return;
        this->rate = rate;
        for(int i=0; i<numTaps; i++) {
            double increment = double(i) / 10;
            lfos[i].setRate(rate + increment);
        }
    }
    
    void setMix(float mix) {
        this->mix = mix;
        matrix.cheapEnergyCrossfade(mix, wet, dry);
    }
    
    void processStereo(juce::AudioBuffer<float>& buffer) {
        if(buffer.getMagnitude(0, 0, buffer.getNumSamples()) == 0) {
            left.reset();
            right.reset();
            return;
        }
        auto* firstChannel = buffer.getWritePointer(0);
        auto* secondChannel = buffer.getWritePointer(1);
        auto* readL = buffer.getReadPointer(0);
        auto* readR = buffer.getReadPointer(1);
        
        for (auto i = 0; i < buffer.getNumSamples(); i++)
        {
            float sampleL = readL[i];
            float sampleR = readR[i];
            left.delay(sampleL);
            right.delay(sampleR);
            firstChannel[i] = sampleL * dry + left.getChorus(lfos[0].getValue(), sampleDepth) * wet;
            secondChannel[i] = sampleR * dry + right.getChorus(lfos[1].getValue(), sampleDepth) * wet;
        }
    }
    
    float processSample(float sample, int channel) {
        if(channel == 0){
            left.delay(sample);
            return sample * dry + left.getChorus(lfos[0].getValue(), sampleDepth) * wet;
        }
        else if(channel == 1){
            right.delay(sample);
            return sample * dry + right.getChorus(lfos[0].getValue(), sampleDepth) * wet;
        }
    }
};

}

#endif
