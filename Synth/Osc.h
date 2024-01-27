#ifndef Colin_OSC_H
#define Colin_OSC_H

#include <cmath>

namespace Colin
{

typedef unsigned int uint32;
typedef int int32;
typedef unsigned long long int uint64;
typedef long long int int64;

class Triangle
{
public:
    Triangle() = default;
    ~Triangle() = default;
    Triangle(const Triangle& oscillator) = default;
    void setSampleRate(const uint32& sampleRate)
    {
        if (this->sampleRate != sampleRate)
        {
            this->sampleRate = sampleRate;
            stop();
        }
    }
    
    void setFrequency(float frequency)
    {
        indexIncrement = frequency / this->sampleRate;
    }
    
    float getSample()
    {
        if (isPlaying())
        {
            if (mod >= 1.0)
                mod = mod - 1.0;
            sample = 2.0 * fabs(2.0 * mod - 1.0) - 1.0;
            mod += indexIncrement;
            return sample;
        }
        return 0;
    }
    
    void stop()
    {
        indexIncrement = 0;
        mod = 0;
    }
    
    bool isPlaying()
    {
        return indexIncrement != 0;
    }
    
private:
    float indexIncrement = 0;
    float mod = 0;
    float sample = 0;
    uint32 sampleRate = 44100;
    bool playing = false;
};

class Sawtooth
{
public:
    Sawtooth() = default;
    ~Sawtooth() = default;
    Sawtooth(const Sawtooth& oscillator) = default;
    void setSampleRate(const uint32& sampleRate)
    {
        if (this->sampleRate != sampleRate)
        {
            this->sampleRate = sampleRate;
            lfo.setSampleRate(sampleRate);
            stop();
        }
    }
    
    void setFrequency(float frequency)
    {
        indexIncrement = frequency / this->sampleRate;
        freq = frequency;
    }
    
    void setVibrato(float depth, float speed) {
        lfo.setFrequency(speed);
        lfoDepth = depth;
    }
    
    void setVibratoDepth(float depth) {
        lfoDepth = depth;
    }
    
    float getFrequency() {
        //return indexIncrement * sampleRate;
        return freq;
    }
    
    inline float doPolyBLEP_2(float modulo, float moduloInc, float height, bool risingEdge)
    {
        //  return value
        double dPolyBLEP = 0.0;
        
        //  LEFT side of discontinuity
        //    -1 < t < 0
        if(modulo > 1.0 - moduloInc)
        {
            // calculate distance
            double t = (modulo - 1.0)/moduloInc;

            // calculate residual
            dPolyBLEP = height*(t*t + 2.0*t + 1.0);
        }
        // RIGHT side of discontinuity
        // 0 <= t < 1
        else if(modulo < moduloInc)
        {
            // --- calculate distance
            double t = modulo/moduloInc;

            // --- calculate residual
            dPolyBLEP = height*(2.0*t - t*t - 1.0);
        }

        // --- subtract for falling, add for rising edge
        if(!risingEdge)
            dPolyBLEP *= -1.0;

        return dPolyBLEP;
    }
    
    float getSample()
    {
        if (isPlaying())
        {
            float lfoSample = lfo.getSample();
            lfoSample *= lfoDepth;
            lfoSample += 1;
            if(lfoSample < 1) lfoSample = lfoSample / 2 + 0.5;
            float newFreq = freq * lfoSample;
            indexIncrement = newFreq / sampleRate;
            
            if (mod >= 1.0f)
            {
                mod = mod - 1.f;
            }
            float trivialSaw = 2.0f * mod - 1.f;
            sample = trivialSaw + doPolyBLEP_2(mod, abs(indexIncrement), 1.f, false);
            mod = mod + indexIncrement;
            /*
            if(mod > 1.0 - indexIncrement) {
                float t = (mod - 1.0) / indexIncrement;
                sample = 1 * (t*t + 2.0*t + 1.0);
            }
            else if(mod < indexIncrement) {
                float t = mod / indexIncrement;
                sample = 1 * (2.0*t - t*t - 1.0);
            }
            else sample = 2.0 * mod - 1.0;
            mod += indexIncrement;
            if(mod > 1.0) mod -= 1.0;
            */
            return sample;
        }
    }
    
    void stop() {
        indexIncrement = 0;
        mod = 0;
    }
    
    bool isPlaying() {
        return indexIncrement != 0;
    }
    
private:
    float indexIncrement = 0;
    float mod = 0;
    float sample = 0;
    float lfoDepth = 0;
    float freq = 0;
    uint32 sampleRate = 44100;
    Triangle lfo;
};

//--------------------------------------------------------------------------------------\\

class polyBlepSaw
{
public:
    polyBlepSaw() = default;
    ~polyBlepSaw() = default;
    void setSampleRate(const int& sampleRate)
    {
        if (this->sampleRate != sampleRate)
        {
            this->sampleRate = sampleRate;
        }
    }
    
    void setFrequency(float frequency)
    {
        inc = frequency / sampleRate;
        //freq = frequency;
    }
    
    float getSample() {
        if(!isPlaying()) return 0;
        float sample = 0.0;
        if(mod > 1.0 - inc) {
            float t = (mod - 1.0) / inc;
            sample = height * (t*t + 2.0*t + 1.0);
            mod -= 1.0;
        }
        else if(mod < inc) {
            float t = mod / inc;
            sample = height * (2.0*t - t*t - 1.0);
        }
        else {
            sample = 2.0 * mod - 1.0;
        }
        if(!rising) {
            sample *= -1.0;
        }
        mod += inc;
        return sample;
    }
    
    void stop() {
        inc = 0;
        mod = 0;
    }
    
    bool isPlaying() {
        return inc != 0;
    }
    
private:
    int sampleRate = 44100;
    float inc = 0;
    float mod = 0;
    float height = 1.0;
    bool rising = true;
};


//--------------------------------------------------------------------------------------\\

class ReverseSawtooth
{
public:
    ReverseSawtooth() = default;
    ~ReverseSawtooth() = default;
    ReverseSawtooth(const ReverseSawtooth& oscillator) = default;
    void setSampleRate(const uint32& sampleRate)
    {
        if (this->sampleRate != sampleRate)
        {
            this->sampleRate = sampleRate;
            stop();
        }
        
    }
    
    void setFrequency(float frequency)
    {
        indexIncrement = frequency / this->sampleRate;
    }
    
    float getSample()
    {
        if (isPlaying())
        {
            if (mod <= 0.0)
                mod = mod + 1.0;
            sample = 2.0 * mod - 1.0;
            mod -= indexIncrement;
            return sample;
        }
    }
    
    void start() {
        if(!playing)
            playing = true;
    }
    
    void stop()
    {
        //indexIncrement = 0;
        mod = 0;
        if(playing)
            playing = false;
    }
    
    bool isPlaying()
    {
        return playing;
    }
    
private:
    float indexIncrement = 0;
    float mod = 0;
    float sample = 0;
    uint32 sampleRate = 44100;
    bool playing = false;
};

//--------------------------------------------------------------------------------------\\

class Sine
{
public:
    Sine() = default;
    ~Sine() = default;
    Sine(const Sine& oscillator) = default;
    void setSampleRate(const uint32& sampleRate)
    {
        if (this->sampleRate != sampleRate)
        {
            this->sampleRate = sampleRate;
            stop();
        }
    }
    
    void setFrequency(float frequency)
    {
        indexIncrement = frequency / this->sampleRate;
    }
    
    float getSample()
    {
        if (isPlaying())
        {
            if (mod >= 1.0)
                mod = mod - 1.0;
            sample = sin(mod * 3.14159 * 2);
            mod += indexIncrement;
            return sample;
        }
    }
    
    void setPhase(float phase) {
        mod = phase;
    }
    
    void start() {
        if(!playing)
            playing = true;
    }
    
    void stop()
    {
        //indexIncrement = 0;
        mod = 0;
        if(playing)
            playing = false;
    }
    
    bool isPlaying()
    {
        return playing;
    }
    
private:
    float indexIncrement = 0;
    float mod = 0;
    float sample = 0;
    uint32 sampleRate = 44100;
    bool playing = false;
    
};

//--------------------------------------------------------------------------------------\\

class Noise
{
public:
    Noise() = default;
    ~Noise() = default;
    Noise(const Noise& oscillator) = default;
    void setSampleRate(const uint32& sampleRate)
    {
        if (this->sampleRate != sampleRate)
        {
            this->sampleRate = sampleRate;
            stop();
        }
    }
    
    float getSample()
    {
        if (isPlaying())
        {
            sample = ( (float)rand() / (float)RAND_MAX ) - 0.5; // white noise
            // add random spikes for variation
            if(spike == 0) spikeChance = rand() % 1000;
            if(spikeChance == 1) spike = 100;
            if(spike > 0) {
                sample = sample * (spike + 20 / 20);
                if(sample < -0.95) sample = -0.95;
                else if(sample > 0.95) sample = 0.95;
                spike--;
            }
            return sample;
        }
    }
    
    void stop()
    {
        playing = false;
    }
    
    void start()
    {
        playing = true;
    }
    
    bool isPlaying()
    {
        return playing;
    }
    
private:
    bool playing = false;
    float sample = 0;
    uint32 sampleRate = 44100;
    int spikeChance = 0;
    int spike = 0;
};

}

#endif


