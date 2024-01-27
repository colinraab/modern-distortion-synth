#ifndef COLIN_REVERB_H
#define COLIN_REVERB_H
#include <math.h>
#include "Delay.h"
#include "../Biquad.h"

/*
  ==============================================================================

    Reverb.h
    Created: 1 Nov 2022 7:46:14pm
    Author:  Colin Raab

  ==============================================================================
*/

namespace Colin {

class Reverb {
protected:
    float roomsize = 150; /// in ms
    float rt60 = 3000; /// in ms
    int fs = 44100;
    float drywet = 1; /// wet = 1, dry = 0
    float decay;
    bool sidechain = false;
    float prevRMSL = 0;
    float prevRMSR = 0;
    float chorusMix = 0; /// wet = 1, dry = 0
        
    Diffuser diffusion;
    Multi_Delay feedback;
    Mix_Matrix matrix;
    Chorus chorus;
    
    Biquad_Filter LPFL;
    Biquad_Filter LPFR;
    Biquad_Filter HPFL;
    Biquad_Filter HPFR;
    Biquad_Filter ShelfL;
    Biquad_Filter ShelfR;
    
public:
    Reverb() = default;
    ~Reverb() = default;
    Reverb(int fs, int sizeMS, float rt60) {
        this->fs = fs;
        setSize(sizeMS, rt60);
    }
    
    void prepareToPlay(int fs, int roomsize, float rt60) {
        this->fs = fs;
        this->roomsize = roomsize;
        diffusion.prepareToPlay(150, fs);
        feedback.prepareToPlay(fs, 150, 0.85);
        setSize(roomsize, rt60);
        
        ShelfL.setSampleRate(fs);
        ShelfR.setSampleRate(fs);
        LPFL.setSampleRate(fs);
        LPFR.setSampleRate(fs);
        HPFL.setSampleRate(fs);
        HPFR.setSampleRate(fs);
        
        ShelfL.setBiquad(Colin::Biquad_Type::HighShelf, 4500, 1, -5);
        ShelfR.setBiquad(Colin::Biquad_Type::HighShelf, 4500, 1, -5);
        LPFL.setBiquad(Colin::Biquad_Type::LPF, 6000, 2, 0);
        LPFR.setBiquad(Colin::Biquad_Type::LPF, 6000, 2, 0);
        HPFL.setBiquad(Colin::Biquad_Type::HPF, 439, 2, 0);
        HPFR.setBiquad(Colin::Biquad_Type::HPF, 439, 2, 0);
        
        chorus.prepareToPlay(fs);
        chorus.setRate(.1); /// value in Hz
        chorus.setDepth(15); /// value between 0 and 100
    }
    
    void setSize(float newsize, float rt) {
        if(newsize != roomsize) {
            roomsize = newsize;
            feedback.setTime(roomsize);
            diffusion.prepareToPlay(roomsize, fs);
        }
        if(rt != rt60) {
            rt60 = rt;
            float loop = roomsize * 1.5;
            float loopPerRT60 = rt60 / loop;
            float dbPerLoop = -60/loopPerRT60;
            decay = powf(10,dbPerLoop/20);
            feedback.setDecay(decay);
        }
    }
    
    void setShimmerMix(float shimmerMix) {
        feedback.setShimmerMix(shimmerMix);
    }
    
    void setAntiShimmerMix(float antiMix) {
        feedback.setAntiShimmerMix(antiMix);
    }
    
    void infReverb() {
        feedback.setDecay(1);
    }
    
    void setDryWet(float drywet) {
        this->drywet = drywet;
    }
    
    void setLFODepth(float depth) {
        feedback.setLFODepth(depth);
    }
    
    void setSidechain(bool sidechainOn) {
        sidechain = sidechainOn;
    }
    
    void setChorus(float mix) {
        if(chorusMix == mix) return;
        chorusMix = mix;
        chorus.setMix(chorusMix);
    }
    
    void setFilterParameters(float characterParameter, float sizeParameter) {
        float fc = (((characterParameter > 0) ? characterParameter * 30 : 0) + 3000) / fs;
        float q = ((characterParameter > 0) ? characterParameter : 0) / 100 + 0.5;
        float gain = characterParameter / 10;
        float LPFfc = (sizeParameter * -50 + 7000) / fs;
        float HPFfc = (std::powf(((characterParameter > 0) ? characterParameter : 1), 1.4) + 200) / fs;
        ShelfL.setBiquad(Colin::Biquad_Type::HighShelf, fc, q, gain);
        ShelfR.setBiquad(Colin::Biquad_Type::HighShelf, fc, q, gain);
        LPFL.setBiquad(Colin::Biquad_Type::LPF, LPFfc, 2, 0);
        LPFR.setBiquad(Colin::Biquad_Type::LPF, LPFfc, 2, 0);
        HPFL.setBiquad(Colin::Biquad_Type::HPF, HPFfc, 2, 0);
        HPFR.setBiquad(Colin::Biquad_Type::HPF, HPFfc, 2, 0);
    }
    
    void processMono(juce::AudioBuffer<float>& buffer) {
        /// tbd add?
    }
    
    void processStereo(juce::AudioBuffer<float>& buffer) {
        float rmsL = buffer.getRMSLevel(0, 0, buffer.getNumSamples());
        float rmsR = buffer.getRMSLevel(1, 0, buffer.getNumSamples());
        float newRMSL = prevRMSL + (rmsL - prevRMSL);
        float newRMSR = prevRMSR + (rmsR - prevRMSR);
        prevRMSL = newRMSL;
        prevRMSR = newRMSR;
        auto* firstChannel = buffer.getWritePointer(0);
        auto* secondChannel = buffer.getWritePointer(1);
        auto* readL = buffer.getReadPointer(0);
        auto* readR = buffer.getReadPointer(1);
        if(drywet == 0) return;
        for (auto i = 0; i < buffer.getNumSamples(); i++)
        {
            /// get input
            float sampleL = readL[i];
            float sampleR = readR[i];
            /// distribute across 8 channels
            data input = matrix.stereoToMulti(sampleL, sampleR);
            /// perform reverb on the 8 channels
            data dout = diffusion.process(input);
            data fout = feedback.process(dout);
            float outsampleL = 0;
            float outsampleR = 0;
            /// down mix back to stereo
            matrix.multiToStereo(fout, outsampleL, outsampleR);
            /// apply chorus
            if(chorusMix > 0) {
                outsampleL = chorus.processSample(outsampleL, 0);
                outsampleR = chorus.processSample(outsampleR, 0);
            }
            /// apply filters
            outsampleL = ShelfL.processAudioSample(outsampleL);
            outsampleL = LPFL.processAudioSample(outsampleL);
            outsampleL = HPFL.processAudioSample(outsampleL);
            outsampleR = ShelfR.processAudioSample(outsampleR);
            outsampleR = LPFR.processAudioSample(outsampleR);
            outsampleR = HPFR.processAudioSample(outsampleR);
            /// if sidechain enabled, duck reverb signal by incoming signal
            if(sidechain) {
                outsampleL *= (1-newRMSL);
                outsampleR *= (1-newRMSR);
            }
            /// equal power coefficients for dry/wet mixing
            float inCoeff = 0;
            float outCoeff = 0;
            matrix.cheapEnergyCrossfade(drywet, outCoeff, inCoeff);
            /// dry/wet mix, then send to output
            float left = (outsampleL * outCoeff) + (sampleL * inCoeff);
            float right = (outsampleR * outCoeff) + (sampleR * inCoeff);
            firstChannel[i] = left;
            secondChannel[i] = right;
        }
    }
};

}

#endif
