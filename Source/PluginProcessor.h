/*
  ==============================================================================

    The PluginProcessor is the core of the plugin, all the audio enters and leaves through here

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Synth/Synth.h"
#include "../Reverb/Reverb.h"
#include "../Distortion.h"
#include "../AuxParam.h"
#include "../GainMeter.h"
#include "../Synth/Sampler.h"
#include "../UI/Oscilloscope.h"
#include "../Presets/PresetManager.h"
#include "../Presets/ParameterHelper.h"


//==============================================================================
/**
*/

class globalVoice {
public:
    globalVoice(double sampleRate, juce::ADSR::Parameters* params, int pitch, int bufChan, int bufSize) {
        osc1Buffer = new juce::AudioBuffer<float>(bufChan,bufSize);
        osc2Buffer = new juce::AudioBuffer<float>(bufChan,bufSize);
        noiseBuffer = new juce::AudioBuffer<float>(bufChan,bufSize);
        samplerBuffer = new juce::AudioBuffer<float>(bufChan,bufSize);
        osc1Buffer->clear();
        osc2Buffer->clear();
        noiseBuffer->clear();
        samplerBuffer->clear();
        adsr.setSampleRate(sampleRate);
        adsr.setParameters(*params);
        adsr.reset();
        this->pitch = pitch;
        adsr.noteOn();
    }
    
    ~globalVoice() {
        delete osc1Buffer;
        delete osc2Buffer;
        delete noiseBuffer;
        delete samplerBuffer;
    }
    
    juce::AudioBuffer<float>* getOsc1() {
        return osc1Buffer;
    }
    
    juce::AudioBuffer<float>* getOsc2() {
        return osc2Buffer;
    }
    
    juce::AudioBuffer<float>* getNoise() {
        return noiseBuffer;
    }
    
    juce::AudioBuffer<float>* getSampler() {
        return samplerBuffer;
    }
    
    void setADSRParameters(juce::ADSR::Parameters params) {
        adsr.setParameters(params);
    }
    
    void noteOn() {
        adsr.noteOn();
    }
    
    void noteOff() {
        adsr.noteOff();
    }
    
    bool isActive() {
        return adsr.isActive();
    }
    
    float getNextSample() {
        return adsr.getNextSample();
    }
    
    void reset() {
        adsr.reset();
    }
    
    void setRelease() {
        release = true;
    }
    
    int getPitch() {
        return pitch;
    }
    
    bool isRelease() {
        return release;
    }
    
    void cycleADSR(int offset) {
        for(int i=offset; i<osc1Buffer->getNumSamples(); i++) {
            adsr.getNextSample();
        }
    }
    
    void applyADSR() {
        float envSampleStart = adsr.getNextSample();
        cycleADSR(2);
        float envSampleEnd = adsr.getNextSample();
        osc1Buffer->applyGainRamp(0, osc1Buffer->getNumSamples(), envSampleStart, envSampleEnd);
        osc2Buffer->applyGainRamp(0, osc1Buffer->getNumSamples(), envSampleStart, envSampleEnd);
        noiseBuffer->applyGainRamp(0, osc1Buffer->getNumSamples(), envSampleStart, envSampleEnd);
        samplerBuffer->applyGainRamp(0, osc1Buffer->getNumSamples(), envSampleStart, envSampleEnd);
    }
    
    float getSample(int channel, int sample) {
        return osc1Buffer->getSample(channel, sample) * osc1Vol + osc2Buffer->getSample(channel, sample) * osc2Vol + noiseBuffer->getSample(channel, sample) * noiseVol + samplerBuffer->getSample(channel, sample) * samplerVol;
    }
    
    void setVolume(float osc1, float osc2, float noise, float sampler) {
        osc1Vol = osc1;
        osc2Vol = osc2;
        noiseVol = noise;
        samplerVol = sampler;
    }
private:
    juce::AudioBuffer<float>* osc1Buffer;
    juce::AudioBuffer<float>* osc2Buffer;
    juce::AudioBuffer<float>* noiseBuffer;
    juce::AudioBuffer<float>* samplerBuffer;
    juce::ADSR adsr;
    int pitch = 0;
    bool release = false;
    
    float osc1Vol;
    float osc2Vol;
    float noiseVol;
    float samplerVol;
};


class CapstoneAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    CapstoneAudioProcessor();
    ~CapstoneAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    AuxPort::ParameterMap parameterMap;
    Colin::Synth* osc1;
    Colin::Synth* osc2;
    Colin::Synth* noise;
    Colin::Sampler* sampler;
    Colin::GainMeter VUMeter;
    float getRmsValue(const int channel) const;
    
    juce::dsp::LadderFilterMode getFilterMode(int type);
    void randomizeParams();
    float getSampleFromBuffers(std::vector<juce::AudioBuffer<float>*>&, int channel, int sample);
    float getSampleFromSampler(std::vector<juce::AudioBuffer<float>*>&, int channel, int sample);
    void applyADSR(std::vector<juce::AudioBuffer<float>*>&);
    void applyADSRSampler(std::vector<juce::AudioBuffer<float>*>&);
    void enableADSR(juce::MidiBuffer&, int bufChan, int bufSize);
    void setADSR(float atk, float dec, float sus, float rel);
    Colin::Distortion* distMain;
    
    Colin::Oscilloscope* oscilloscope;
    
    AuxPort::Bezier* bezier1;
    AuxPort::Bezier* bezier2;
    AuxPort::Bezier* bezierN;
    AuxPort::Bezier* bezierS;
    AuxPort::Bezier* bezierM;
    
    Service::PresetManager presetManager;
    
    Service::PresetManager& getPresetManager() { return presetManager; }
    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CapstoneAudioProcessor)
        
    juce::dsp::ProcessSpec spec;
    
    std::vector<std::unique_ptr<globalVoice>> globalVoices;

    std::vector<juce::AudioBuffer<float>*> osc1Buffers;
    std::vector<juce::AudioBuffer<float>*> osc2Buffers;
    std::vector<juce::AudioBuffer<float>*> noiseBuffers;
    std::vector<juce::AudioBuffer<float>*> samplerBuffers;
    
    void prepareBezier(AuxPort::Bezier* b, juce::AudioParameterFloat* x, juce::AudioParameterFloat* y, juce::AudioParameterFloat* s, std::vector<juce::Point<float>*> points, int tab);
    void setBezier(AuxPort::Bezier* b, juce::AudioParameterFloat* x, juce::AudioParameterFloat* y, juce::AudioParameterFloat* s, std::vector<juce::Point<float>*> points, int tab);
    
    juce::AudioParameterFloat* yParam1;
    juce::AudioParameterFloat* xParam1;
    juce::AudioParameterFloat* slopeParam1;
    std::vector<juce::Point<float>*> points1;
    juce::AudioParameterFloat* yParam2;
    juce::AudioParameterFloat* xParam2;
    juce::AudioParameterFloat* slopeParam2;
    std::vector<juce::Point<float>*> points2;
    juce::AudioParameterFloat* yParamN;
    juce::AudioParameterFloat* xParamN;
    juce::AudioParameterFloat* slopeParamN;
    std::vector<juce::Point<float>*> pointsN;
    juce::AudioParameterFloat* yParamS;
    juce::AudioParameterFloat* xParamS;
    juce::AudioParameterFloat* slopeParamS;
    std::vector<juce::Point<float>*> pointsS;
    juce::AudioParameterFloat* yParamM;
    juce::AudioParameterFloat* xParamM;
    juce::AudioParameterFloat* slopeParamM;
    std::vector<juce::Point<float>*> pointsM;
    std::vector<float> slopeFactors;
    
    juce::ADSR::Parameters* ADSRparams;
    //std::vector<newADSR*> ADSRs;
    //std::vector<int> activeADSRs;
    
    juce::AudioParameterFloat * osc1Atk;
    juce::AudioParameterFloat * osc1Dec;
    juce::AudioParameterFloat * osc1Sus;
    juce::AudioParameterFloat * osc1Rel;
    juce::AudioParameterFloat * osc1Depth;
    juce::AudioParameterFloat * osc2Atk;
    juce::AudioParameterFloat * osc2Dec;
    juce::AudioParameterFloat * osc2Sus;
    juce::AudioParameterFloat * osc2Rel;
    juce::AudioParameterFloat * osc2Depth;
    juce::AudioParameterFloat * noiseAtk;
    juce::AudioParameterFloat * noiseDec;
    juce::AudioParameterFloat * noiseSus;
    juce::AudioParameterFloat * noiseRel;
    juce::AudioParameterFloat * noiseDepth;
    juce::AudioParameterFloat * samplerAtk;
    juce::AudioParameterFloat * samplerDec;
    juce::AudioParameterFloat * samplerSus;
    juce::AudioParameterFloat * samplerRel;
    juce::AudioParameterFloat * samplerDepth;
    juce::AudioParameterFloat * mainAtk;
    juce::AudioParameterFloat * mainDec;
    juce::AudioParameterFloat * mainSus;
    juce::AudioParameterFloat * mainRel;
    
    juce::AudioParameterBool * osc1etV;
    juce::AudioParameterBool * osc1etD;
    juce::AudioParameterBool * osc1etF;
    juce::AudioParameterBool * osc2etV;
    juce::AudioParameterBool * osc2etD;
    juce::AudioParameterBool * osc2etF;
    juce::AudioParameterBool * noiseetV;
    juce::AudioParameterBool * noiseetD;
    juce::AudioParameterBool * noiseetF;
    juce::AudioParameterBool * sampleretV;
    juce::AudioParameterBool * sampleretD;
    juce::AudioParameterBool * sampleretF;
    
    juce::dsp::Compressor<float> limiter;
    
    juce::dsp::LadderFilter<float> ladderM;
        
    juce::AudioParameterInt * osc1DistSel;
    juce::AudioParameterInt * osc2DistSel;
    juce::AudioParameterInt * noiseDistSel;
    juce::AudioParameterInt * samplerDistSel;
    juce::AudioParameterInt * mainDistSel;
    juce::AudioParameterFloat * osc1DAmt;
    juce::AudioParameterFloat * osc2DAmt;
    juce::AudioParameterFloat * noiseDAmt;
    juce::AudioParameterFloat * samplerDAmt;
    juce::AudioParameterFloat * mainDAmt;
    juce::AudioParameterFloat * osc1DCoeff;
    juce::AudioParameterFloat * osc2DCoeff;
    juce::AudioParameterFloat * noiseDCoeff;
    juce::AudioParameterFloat * samplerDCoeff;
    juce::AudioParameterFloat * mainDCoeff;
    juce::AudioParameterFloat * fmAmt1;
    juce::AudioParameterFloat * fmAmt2;
    juce::AudioParameterFloat * osc1Pitch;
    juce::AudioParameterFloat * osc2Pitch;
    juce::AudioParameterFloat * samplerPitch;
    juce::AudioParameterBool * samplerRepitch;
    juce::AudioParameterBool * samplerLoop;
    juce::AudioParameterInt * osc1Wave;
    juce::AudioParameterInt * osc2Wave;
    juce::AudioParameterInt * noiseWave;
    
    juce::AudioParameterFloat * osc1WaveSlider;
    juce::AudioParameterFloat * osc2WaveSlider;
    juce::AudioParameterFloat * noiseWaveSlider;
    juce::AudioParameterFloat * samplerWaveSlider;
    juce::AudioParameterFloat * mainWaveSlider;
    juce::AudioParameterFloat * osc1DistSlider;
    juce::AudioParameterFloat * osc2DistSlider;
    juce::AudioParameterFloat * noiseDistSlider;
    juce::AudioParameterFloat * samplerDistSlider;
    juce::AudioParameterFloat * mainDistSlider;
    
    juce::AudioParameterInt * osc1Filter;
    juce::AudioParameterInt * osc2Filter;
    juce::AudioParameterInt * noiseFilter;
    juce::AudioParameterInt * samplerFilter;
    juce::AudioParameterInt * mainFilter;
    juce::AudioParameterFloat * osc1Cutoff;
    juce::AudioParameterFloat * osc1Res;
    juce::AudioParameterFloat * osc2Cutoff;
    juce::AudioParameterFloat * osc2Res;
    juce::AudioParameterFloat * noiseCutoff;
    juce::AudioParameterFloat * noiseRes;
    juce::AudioParameterFloat * samplerCutoff;
    juce::AudioParameterFloat * samplerRes;
    juce::AudioParameterFloat * mainCutoff;
    juce::AudioParameterFloat * mainRes;
    juce::AudioParameterBool * osc1Keytrack;
    juce::AudioParameterBool * osc2Keytrack;
    juce::AudioParameterBool * noiseKeytrack;
    juce::AudioParameterBool * samplerKeytrack;
    juce::AudioParameterFloat * osc1ktA;
    juce::AudioParameterFloat * osc2ktA;
    juce::AudioParameterFloat * noisektA;
    juce::AudioParameterFloat * samplerktA;
    
    juce::AudioParameterFloat * osc1Vol;
    juce::AudioParameterFloat * osc2Vol;
    juce::AudioParameterFloat * noiseVol;
    juce::AudioParameterFloat * samplerVol;
    juce::AudioParameterFloat * mainVol;
    
    juce::AudioParameterFloat * cThresh;
    juce::AudioParameterFloat * cRatio;
    juce::AudioParameterFloat * cAtk;
    juce::AudioParameterFloat * cRel;
        
    float RMS;
    juce::LinearSmoothedValue<float> rmsLevelLeft, rmsLevelRight;
    
};
