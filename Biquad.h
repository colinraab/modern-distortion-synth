#ifndef COLIN_BIQUAD_H
#define COLIN_BIQUAD_H
#include <math.h>

/*
  ==============================================================================

    Biquad.h
    Created: 20 Sep 2022 2:44:09pm
    Updated: 30 Nov 2022
        added sampleRate variable to save effort
    Author:  Colin Raab

  ==============================================================================
*/

namespace Colin
{

enum class Biquad_Type {
    LPF, HPF, BPF, Notch, Peak, LowShelf, HighShelf
};

class Biquad_Filter {
protected:
    Biquad_Type type;
    double a0, a1, a2, b1, b2;
    float Fc, Q, peakGain;
    double z1, z2;
    int sampleRate;
    
public:
    Biquad_Filter()
    {
        type = Biquad_Type::Peak;
        a0 = 1.0;
        a1 = a2 = b1 = b2 = 0.0;
        Fc = 0.50;
        Q = 0.707;
        peakGain = 0.0;
        z1 = z2 = 0.0;
        sampleRate = 44100;
    }
    ~Biquad_Filter() = default;
    Biquad_Filter(Biquad_Type type, float Fc, float Q, float peakGainDB)
    {
        setBiquad(type, Fc, Q, peakGainDB);
        z1 = z2 = 0.0;
    }
    void setSampleRate(int fs) {
        if(this->sampleRate != fs)
            this->sampleRate = fs;
    }
    void setType(Biquad_Type type)
    {
        this->type = type;
        calcBiquad();
    }
    void setQ(float Q)
    {
        this->Q = Q;
        calcBiquad();
    }
    void setFc(float Fc)
    {
        this->Fc = Fc / (float)sampleRate;
        calcBiquad();
    }
    void setPeakGain(float peakGainDB)
    {
        this->peakGain = peakGainDB;
        calcBiquad();
    }
    void setBiquad(Biquad_Type type, float Fc, float Q, float peakGainDB)
    {
        this->type = type;
        this->Q = Q;
        this->Fc = Fc;
        this->peakGain = peakGainDB;
        setFc(Fc);
    }
    float processAudioSample(float sample) {
        float out = sample * a0 + z1;
        z1 = sample * a1 + z2 - b1 * out;
        z2 = sample * a2 - b2 * out;
        return out;
    }
    void calcBiquad()
    {
        double norm;
        double V = pow(10, fabs(peakGain) / 20.0);
        double K = tan(M_PI * Fc);
        switch (this->type) {
            case Biquad_Type::LPF:
                norm = 1 / (1 + K / Q + K * K);
                a0 = K * K * norm;
                a1 = 2 * a0;
                a2 = a0;
                b1 = 2 * (K * K - 1) * norm;
                b2 = (1 - K / Q + K * K) * norm;
                break;
                
            case Biquad_Type::HPF:
                norm = 1 / (1 + K / Q + K * K);
                a0 = 1 * norm;
                a1 = -2 * a0;
                a2 = a0;
                b1 = 2 * (K * K - 1) * norm;
                b2 = (1 - K / Q + K * K) * norm;
                break;
                
            case Biquad_Type::BPF:
                norm = 1 / (1 + K / Q + K * K);
                a0 = K / Q * norm;
                a1 = 0;
                a2 = -a0;
                b1 = 2 * (K * K - 1) * norm;
                b2 = (1 - K / Q + K * K) * norm;
                break;
                
            case Biquad_Type::Notch:
                norm = 1 / (1 + K / Q + K * K);
                a0 = (1 + K * K) * norm;
                a1 = 2 * (K * K - 1) * norm;
                a2 = a0;
                b1 = a1;
                b2 = (1 - K / Q + K * K) * norm;
                break;
                
            case Biquad_Type::Peak:
                if (peakGain >= 0) {    // boost
                    norm = 1 / (1 + 1/Q * K + K * K);
                    a0 = (1 + V/Q * K + K * K) * norm;
                    a1 = 2 * (K * K - 1) * norm;
                    a2 = (1 - V/Q * K + K * K) * norm;
                    b1 = a1;
                    b2 = (1 - 1/Q * K + K * K) * norm;
                }
                else {    // cut
                    norm = 1 / (1 + V/Q * K + K * K);
                    a0 = (1 + 1/Q * K + K * K) * norm;
                    a1 = 2 * (K * K - 1) * norm;
                    a2 = (1 - 1/Q * K + K * K) * norm;
                    b1 = a1;
                    b2 = (1 - V/Q * K + K * K) * norm;
                }
                break;
            case Biquad_Type::LowShelf:
                if (peakGain >= 0) {    // boost
                    norm = 1 / (1 + sqrt(2) * K + K * K);
                    a0 = (1 + sqrt(2*V) * K + V * K * K) * norm;
                    a1 = 2 * (V * K * K - 1) * norm;
                    a2 = (1 - sqrt(2*V) * K + V * K * K) * norm;
                    b1 = 2 * (K * K - 1) * norm;
                    b2 = (1 - sqrt(2) * K + K * K) * norm;
                }
                else {    // cut
                    norm = 1 / (1 + sqrt(2*V) * K + V * K * K);
                    a0 = (1 + sqrt(2) * K + K * K) * norm;
                    a1 = 2 * (K * K - 1) * norm;
                    a2 = (1 - sqrt(2) * K + K * K) * norm;
                    b1 = 2 * (V * K * K - 1) * norm;
                    b2 = (1 - sqrt(2*V) * K + V * K * K) * norm;
                }
                break;
            case Biquad_Type::HighShelf:
                if (peakGain >= 0) {    // boost
                    norm = 1 / (1 + sqrt(2) * K + K * K);
                    a0 = (V + sqrt(2*V) * K + K * K) * norm;
                    a1 = 2 * (K * K - V) * norm;
                    a2 = (V - sqrt(2*V) * K + K * K) * norm;
                    b1 = 2 * (K * K - 1) * norm;
                    b2 = (1 - sqrt(2) * K + K * K) * norm;
                }
                else {    // cut
                    norm = 1 / (V + sqrt(2*V) * K + K * K);
                    a0 = (1 + sqrt(2) * K + K * K) * norm;
                    a1 = 2 * (K * K - 1) * norm;
                    a2 = (1 - sqrt(2) * K + K * K) * norm;
                    b1 = 2 * (K * K - V) * norm;
                    b2 = (V - sqrt(2*V) * K + K * K) * norm;
                }
                break;
        }
        return;
    }
};


}

#endif
