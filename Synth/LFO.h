#ifndef Colin_LFO_H
#define Colin_LFO_H
#include "Osc.h"

namespace Colin
{

enum class LFO_type {
    Sine, Saw, RevSaw, Triangle
};

class LFO {
private:
    uint32 sampleRate = 44100;
    Colin::Sine sine;
    Colin::Sawtooth saw;
    Colin::ReverseSawtooth revsaw;
    Colin::Triangle triangle;
    Colin::LFO_type type = Colin::LFO_type::Sine;
    bool playing = false;
    
    float depth = 50.f; // 1 to 100 value
    float rate; // 0.1 to 10 hz
    
public:
    LFO() = default;
    ~LFO() = default;
    LFO(const LFO& lfo) = default;
    
    void prepareToPlay(const uint32& sampleRate) {
        if (this->sampleRate != sampleRate)
        {
            sine.setSampleRate(sampleRate);
            saw.setSampleRate(sampleRate);
            revsaw.setSampleRate(sampleRate);
            triangle.setSampleRate(sampleRate);
            this->sampleRate = sampleRate;
        }
    }
    
    void setType(Colin::LFO_type type) {
        this->type = type;
    }
    
    void setDepth(float depth) {
        stop();
        this->depth = depth;
        start();
    }
    
    void setRate(float rate) {
        if(this->rate != rate) {
            sine.setFrequency(rate);
            saw.setFrequency(rate);
            revsaw.setFrequency(rate);
            triangle.setFrequency(rate);
            this->rate = rate;
        }
    }
    
    void start() {
        if(type == Colin::LFO_type::Sine) {
            sine.start();
        }
        else if(type == Colin::LFO_type::RevSaw) {
            revsaw.start();
        }
        playing = true;
    }
    
    void stop() {
        if(type == Colin::LFO_type::Sine) {
            sine.stop();
        }
        else if(type == Colin::LFO_type::Saw) {
            saw.stop();
        }
        else if(type == Colin::LFO_type::RevSaw) {
            revsaw.stop();
        }
        else if(type == Colin::LFO_type::Triangle) {
            triangle.stop();
        }
        playing = false;
    }
    
    float getValue() {
        // scale oscillator ouputs to be values between 0 and 1, then adjust based on depth
        float sample = 0;
        if(type == Colin::LFO_type::Sine) {
            sample = ((sine.getSample() + 1) / 2) * (depth / 100) + (1 - depth / 100);
        }
        else if(type == Colin::LFO_type::Saw) {
            sample = ((saw.getSample() + 1) / 2) * (depth / 100) + (1 - depth / 100);
        }
        else if(type == Colin::LFO_type::RevSaw) {
            sample = ((revsaw.getSample() + 1) / 2) * (depth / 100) + (1 - depth / 100);
        }
        else if(type == Colin::LFO_type::Triangle) {
            sample = ((triangle.getSample() + 1) / 2) * (depth / 100) + (1 - depth / 100);
        }
        return sample;
    }
    
    float getBipolarValue() {
        /// TBD FIXXXXX
        /// scale oscillator outputs to be values between 0.5 and 2, then adjust based on depth
        float sample = 0;
        if(type == Colin::LFO_type::Sine) {
            sample = sine.getSample();
        }
        else if(type == Colin::LFO_type::Saw) {
            sample = saw.getSample();
        }
        else if(type == Colin::LFO_type::RevSaw) {
            sample = revsaw.getSample();
        }
        else if(type == Colin::LFO_type::Triangle) {
            sample = triangle.getSample();
        }
        sample += depth;
        sample += 1;
        if(sample < 1) sample = sample / 2 + 0.5;
        return sample;
    }
    
    bool isPlaying() {
        return playing;
    }
    
};

}
#endif
