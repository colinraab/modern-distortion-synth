/*
  ==============================================================================

    The PluginProcessor is the core of the plugin, all the audio enters and leaves through here

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CapstoneAudioProcessor::CapstoneAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
parameterMap(this),
presetManager(&parameterMap)

#endif
{
    osc1DistSel = new juce::AudioParameterInt(juce::ParameterID{"0.1", 1}, "osc1DistSel", 1, 6, 1);
    parameterMap.addParameter(osc1DistSel);
    osc2DistSel = new juce::AudioParameterInt(juce::ParameterID{"0.2", 1}, "osc2DistSel", 1, 6, 1);
    parameterMap.addParameter(osc2DistSel);
    noiseDistSel = new juce::AudioParameterInt(juce::ParameterID{"0.3", 1}, "noiseDistSel", 1, 6, 1);
    parameterMap.addParameter(noiseDistSel);
    samplerDistSel = new juce::AudioParameterInt(juce::ParameterID{"0.4", 1}, "samplerDistSel", 1, 6, 1);
    parameterMap.addParameter(samplerDistSel);
    mainDistSel = new juce::AudioParameterInt(juce::ParameterID{"0.5", 1}, "mainDistSel", 1, 6, 1);
    parameterMap.addParameter(mainDistSel);
    osc1DAmt = new juce::AudioParameterFloat(juce::ParameterID{"1", 1}, "osc1Drive", juce::NormalisableRange<float>(0.0f, 99.f), 50.5f);
    parameterMap.addParameter(osc1DAmt);
    osc2DAmt = new juce::AudioParameterFloat(juce::ParameterID{"1.1", 1}, "osc2Drive", juce::NormalisableRange<float>(0.0f, 99.f), 50.5f);
    parameterMap.addParameter(osc2DAmt);
    noiseDAmt = new juce::AudioParameterFloat(juce::ParameterID{"1.2", 1}, "noiseDrive", juce::NormalisableRange<float>(0.0f, 99.f), 50.5f);
    parameterMap.addParameter(noiseDAmt);
    samplerDAmt = new juce::AudioParameterFloat(juce::ParameterID{"1.3", 1}, "samplerDrive", juce::NormalisableRange<float>(0.0f, 99.f), 50.5f);
    parameterMap.addParameter(samplerDAmt);
    mainDAmt = new juce::AudioParameterFloat(juce::ParameterID{"1.4", 1}, "mainDrive", juce::NormalisableRange<float>(0.0f, 99.f), 50.5f);
    parameterMap.addParameter(mainDAmt);
    osc1DCoeff = new juce::AudioParameterFloat(juce::ParameterID{"1.51", 1}, "osc1DCoeff", juce::NormalisableRange<float>(0.0f, 99.f), 0.f);
    parameterMap.addParameter(osc1DCoeff);
    osc2DCoeff = new juce::AudioParameterFloat(juce::ParameterID{"1.52", 1}, "osc2DCoeff", juce::NormalisableRange<float>(0.0f, 99.f), 0.f);
    parameterMap.addParameter(osc2DCoeff);
    noiseDCoeff = new juce::AudioParameterFloat(juce::ParameterID{"1.53", 1}, "noiseDCoeff", juce::NormalisableRange<float>(0.0f, 99.f), 0.f);
    parameterMap.addParameter(noiseDCoeff);
    samplerDCoeff = new juce::AudioParameterFloat(juce::ParameterID{"1.54", 1}, "samplerDCoeff", juce::NormalisableRange<float>(0.0f, 99.f), 0.f);
    parameterMap.addParameter(samplerDCoeff);
    mainDCoeff = new juce::AudioParameterFloat(juce::ParameterID{"1.55", 1}, "mainDCoeff", juce::NormalisableRange<float>(0.0f, 99.f), 0.f);
    parameterMap.addParameter(mainDCoeff);
    
    
    osc1Atk = new juce::AudioParameterFloat(juce::ParameterID{"3.01", 1}, "osc1Atk", juce::NormalisableRange<float>(0.0f, 99.f), 10.f);
    parameterMap.addParameter(osc1Atk);
    osc1Dec = new juce::AudioParameterFloat(juce::ParameterID{"3.02", 1}, "osc1Dec", juce::NormalisableRange<float>(0.0f, 99.f), 10.f);
    parameterMap.addParameter(osc1Dec);
    osc1Sus = new juce::AudioParameterFloat(juce::ParameterID{"3.03", 1}, "osc1Sus", juce::NormalisableRange<float>(0.0f, 99.f), 80.f);
    parameterMap.addParameter(osc1Sus);
    osc1Rel = new juce::AudioParameterFloat(juce::ParameterID{"3.04", 1}, "osc1Rel", juce::NormalisableRange<float>(0.0f, 99.f), 30.f);
    parameterMap.addParameter(osc1Rel);
    osc1Depth = new juce::AudioParameterFloat(juce::ParameterID{"3.05", 1}, "osc1Depth", juce::NormalisableRange<float>(0.0f, 99.f), 0.f);
    parameterMap.addParameter(osc1Depth);
    osc2Atk = new juce::AudioParameterFloat(juce::ParameterID{"3.11", 1}, "osc2Atk", juce::NormalisableRange<float>(0.0f, 99.f), 10.f);
    parameterMap.addParameter(osc2Atk);
    osc2Dec = new juce::AudioParameterFloat(juce::ParameterID{"3.12", 1}, "osc2Dec", juce::NormalisableRange<float>(0.0f, 99.f), 10.f);
    parameterMap.addParameter(osc2Dec);
    osc2Sus = new juce::AudioParameterFloat(juce::ParameterID{"3.13", 1}, "osc2Sus", juce::NormalisableRange<float>(0.0f, 99.f), 80.f);
    parameterMap.addParameter(osc2Sus);
    osc2Rel = new juce::AudioParameterFloat(juce::ParameterID{"3.14", 1}, "osc2Rel", juce::NormalisableRange<float>(0.0f, 99.f), 30.f);
    parameterMap.addParameter(osc2Rel);
    osc2Depth = new juce::AudioParameterFloat(juce::ParameterID{"3.15", 1}, "osc2Depth", juce::NormalisableRange<float>(0.0f, 99.f), 0.f);
    parameterMap.addParameter(osc2Depth);
    noiseAtk = new juce::AudioParameterFloat(juce::ParameterID{"3.21", 1}, "noiseAtk", juce::NormalisableRange<float>(0.0f, 99.f), 10.f);
    parameterMap.addParameter(noiseAtk);
    noiseDec = new juce::AudioParameterFloat(juce::ParameterID{"3.22", 1}, "noiseDec", juce::NormalisableRange<float>(0.0f, 99.f), 10.f);
    parameterMap.addParameter(noiseDec);
    noiseSus = new juce::AudioParameterFloat(juce::ParameterID{"3.23", 1}, "noiseSus", juce::NormalisableRange<float>(0.0f, 99.f), 80.f);
    parameterMap.addParameter(noiseSus);
    noiseRel = new juce::AudioParameterFloat(juce::ParameterID{"3.24", 1}, "noiseRel", juce::NormalisableRange<float>(0.0f, 99.f), 30.f);
    parameterMap.addParameter(noiseRel);
    noiseDepth = new juce::AudioParameterFloat(juce::ParameterID{"3.25", 1}, "noiseDepth", juce::NormalisableRange<float>(0.0f, 99.f), 0.f);
    parameterMap.addParameter(noiseDepth);
    samplerAtk = new juce::AudioParameterFloat(juce::ParameterID{"3.31", 1}, "samplerAtk", juce::NormalisableRange<float>(0.0f, 99.f), 10.f);
    parameterMap.addParameter(samplerAtk);
    samplerDec = new juce::AudioParameterFloat(juce::ParameterID{"3.32", 1}, "samplerDec", juce::NormalisableRange<float>(0.0f, 99.f), 10.f);
    parameterMap.addParameter(samplerDec);
    samplerSus = new juce::AudioParameterFloat(juce::ParameterID{"3.33", 1}, "samplerSus", juce::NormalisableRange<float>(0.0f, 99.f), 80.f);
    parameterMap.addParameter(samplerSus);
    samplerRel = new juce::AudioParameterFloat(juce::ParameterID{"3.34", 1}, "samplerRel", juce::NormalisableRange<float>(0.0f, 99.f), 30.f);
    parameterMap.addParameter(samplerRel);
    samplerDepth = new juce::AudioParameterFloat(juce::ParameterID{"3.35", 1}, "samplerDepth", juce::NormalisableRange<float>(0.0f, 99.f), 0.f);
    parameterMap.addParameter(samplerDepth);
    mainAtk = new juce::AudioParameterFloat(juce::ParameterID{"3.41", 1}, "mainAtk", juce::NormalisableRange<float>(0.0f, 99.f), 10.f);
    parameterMap.addParameter(mainAtk);
    mainDec = new juce::AudioParameterFloat(juce::ParameterID{"3.42", 1}, "mainDec", juce::NormalisableRange<float>(0.0f, 99.f), 10.f);
    parameterMap.addParameter(mainDec);
    mainSus = new juce::AudioParameterFloat(juce::ParameterID{"3.43", 1}, "mainSus", juce::NormalisableRange<float>(0.0f, 99.f), 80.f);
    parameterMap.addParameter(mainSus);
    mainRel = new juce::AudioParameterFloat(juce::ParameterID{"3.44", 1}, "mainRel", juce::NormalisableRange<float>(0.0f, 99.f), 30.f);
    parameterMap.addParameter(mainRel);
    
    osc1etV = new juce::AudioParameterBool(juce::ParameterID{"3.51", 1}, "osc1envToVol", false);
    parameterMap.addParameter(osc1etV);
    osc1etD = new juce::AudioParameterBool(juce::ParameterID{"3.52", 1}, "osc1envToDist", false);
    parameterMap.addParameter(osc1etD);
    osc1etF = new juce::AudioParameterBool(juce::ParameterID{"3.53", 1}, "osc1envToFilt", false);
    parameterMap.addParameter(osc1etF);
    osc2etV = new juce::AudioParameterBool(juce::ParameterID{"3.61", 1}, "osc2envToVol", false);
    parameterMap.addParameter(osc2etV);
    osc2etD = new juce::AudioParameterBool(juce::ParameterID{"3.62", 1}, "osc2envToDist", false);
    parameterMap.addParameter(osc2etD);
    osc2etF = new juce::AudioParameterBool(juce::ParameterID{"3.63", 1}, "osc2envToFilt", false);
    parameterMap.addParameter(osc2etF);
    noiseetV = new juce::AudioParameterBool(juce::ParameterID{"3.71", 1}, "noiseenvToVol", false);
    parameterMap.addParameter(noiseetV);
    noiseetD = new juce::AudioParameterBool(juce::ParameterID{"3.72", 1}, "noiseenvToDist", false);
    parameterMap.addParameter(noiseetD);
    noiseetF = new juce::AudioParameterBool(juce::ParameterID{"3.73", 1}, "noiseenvToFilt", false);
    parameterMap.addParameter(noiseetF);
    sampleretV = new juce::AudioParameterBool(juce::ParameterID{"3.81", 1}, "samplerenvToVol", false);
    parameterMap.addParameter(sampleretV);
    sampleretD = new juce::AudioParameterBool(juce::ParameterID{"3.82", 1}, "samplerenvToDist", false);
    parameterMap.addParameter(sampleretD);
    sampleretF = new juce::AudioParameterBool(juce::ParameterID{"3.83", 1}, "samplerenvToFilt", false);
    parameterMap.addParameter(sampleretF);
    
    
    fmAmt1 = new juce::AudioParameterFloat(juce::ParameterID{"4", 1}, "FMDepth1", juce::NormalisableRange<float>(0.0f, 99.f), 0.f);
    parameterMap.addParameter(fmAmt1);
    fmAmt2 = new juce::AudioParameterFloat(juce::ParameterID{"4.1", 1}, "FMDepth2", juce::NormalisableRange<float>(0.0f, 99.f), 0.f);
    parameterMap.addParameter(fmAmt2);
    
    osc1Pitch = new juce::AudioParameterFloat(juce::ParameterID{"5", 1}, "osc1Pitch", juce::NormalisableRange<float>(-24.0f, 24.f), 0.f);
    parameterMap.addParameter(osc1Pitch);
    osc2Pitch = new juce::AudioParameterFloat(juce::ParameterID{"5.1", 1}, "osc2Pitch", juce::NormalisableRange<float>(-24.0f, 24.f), 0.f);
    parameterMap.addParameter(osc2Pitch);
    samplerPitch = new juce::AudioParameterFloat(juce::ParameterID{"5.2", 1}, "samplerPitch", juce::NormalisableRange<float>(-24.0f, 24.f), 0.f);
    parameterMap.addParameter(samplerPitch);
    samplerRepitch = new juce::AudioParameterBool(juce::ParameterID{"5.3", 1}, "samplerRepitch", true);
    parameterMap.addParameter(samplerRepitch);
    samplerLoop = new juce::AudioParameterBool(juce::ParameterID{"5.35", 1}, "samplerLoop", false);
    parameterMap.addParameter(samplerLoop);
    osc1Wave = new juce::AudioParameterInt(juce::ParameterID{"5.4", 1}, "osc1Wave", 1, 9, 3);
    parameterMap.addParameter(osc1Wave);
    osc2Wave = new juce::AudioParameterInt(juce::ParameterID{"5.5", 1}, "osc2Wave", 1, 9, 3);
    parameterMap.addParameter(osc2Wave);
    noiseWave = new juce::AudioParameterInt(juce::ParameterID{"5.6", 1}, "noiseWave", 1, 4, 1);
    parameterMap.addParameter(noiseWave);
    osc1WaveSlider = new juce::AudioParameterFloat(juce::ParameterID{"5.71", 1}, "osc1WaveSlider", juce::NormalisableRange<float>(1.f, 80.f), 20.f);
    parameterMap.addParameter(osc1WaveSlider);
    osc2WaveSlider = new juce::AudioParameterFloat(juce::ParameterID{"5.72", 1}, "osc2WaveSlider", juce::NormalisableRange<float>(1.f, 80.f), 20.f);
    parameterMap.addParameter(osc2WaveSlider);
    noiseWaveSlider = new juce::AudioParameterFloat(juce::ParameterID{"5.73", 1}, "noiseWaveSlider", juce::NormalisableRange<float>(1.f, 80.f), 20.f);
    parameterMap.addParameter(noiseWaveSlider);
    samplerWaveSlider = new juce::AudioParameterFloat(juce::ParameterID{"5.74", 1}, "samplerWaveSlider", juce::NormalisableRange<float>(2.0f, 100.f), 100.f);
    parameterMap.addParameter(samplerWaveSlider);
    mainWaveSlider = new juce::AudioParameterFloat(juce::ParameterID{"5.75", 1}, "mainWaveSlider", juce::NormalisableRange<float>(0.0f, 99.f), 0.f);
    parameterMap.addParameter(mainWaveSlider);
    osc1DistSlider = new juce::AudioParameterFloat(juce::ParameterID{"5.81", 1}, "osc1DistSlider", juce::NormalisableRange<float>(0.0f, 100.f), 100.f);
    parameterMap.addParameter(osc1DistSlider);
    osc2DistSlider = new juce::AudioParameterFloat(juce::ParameterID{"5.82", 1}, "osc2DistSlider", juce::NormalisableRange<float>(0.0f, 100.f), 100.f);
    parameterMap.addParameter(osc2DistSlider);
    noiseDistSlider = new juce::AudioParameterFloat(juce::ParameterID{"5.83", 1}, "noiseDistSlider", juce::NormalisableRange<float>(0.0f, 100.f), 100.f);
    parameterMap.addParameter(noiseDistSlider);
    samplerDistSlider = new juce::AudioParameterFloat(juce::ParameterID{"5.84", 1}, "samplerDistSlider", juce::NormalisableRange<float>(0.0f, 100.f), 100.f);
    parameterMap.addParameter(samplerDistSlider);
    mainDistSlider = new juce::AudioParameterFloat(juce::ParameterID{"5.85", 1}, "mainDistSlider", juce::NormalisableRange<float>(0.0f, 100.f), 100.f);
    parameterMap.addParameter(mainDistSlider);
    
    osc1Filter = new juce::AudioParameterInt(juce::ParameterID{"2", 1}, "osc1Filter", 1, 7, 1);
    parameterMap.addParameter(osc1Filter);
    osc2Filter = new juce::AudioParameterInt(juce::ParameterID{"2.1", 1}, "osc2Filter", 1, 7, 1);
    parameterMap.addParameter(osc2Filter);
    noiseFilter = new juce::AudioParameterInt(juce::ParameterID{"2.2", 1}, "noiseFilter", 1, 7, 1);
    parameterMap.addParameter(noiseFilter);
    samplerFilter = new juce::AudioParameterInt(juce::ParameterID{"2.3", 1}, "samplerFilter", 1, 7, 1);
    parameterMap.addParameter(samplerFilter);
    mainFilter = new juce::AudioParameterInt(juce::ParameterID{"2.4", 1}, "mainFilter", 1, 4, 1);
    parameterMap.addParameter(mainFilter);
    osc1Cutoff = new juce::AudioParameterFloat(juce::ParameterID{"6", 1}, "osc1Cutoff", juce::NormalisableRange<float>(20.f, 20000.f, 0.f, 0.4), 20000.f);
    parameterMap.addParameter(osc1Cutoff);
    osc2Cutoff = new juce::AudioParameterFloat(juce::ParameterID{"6.1", 1}, "osc2Cutoff", juce::NormalisableRange<float>(20.f, 20000.f, 0.f, 0.4), 20000.f);
    parameterMap.addParameter(osc2Cutoff);
    noiseCutoff = new juce::AudioParameterFloat(juce::ParameterID{"6.2", 1}, "noiseCutoff", juce::NormalisableRange<float>(0.f, 20000.f, 0.f, 0.4), 20000.f);
    parameterMap.addParameter(noiseCutoff);
    samplerCutoff = new juce::AudioParameterFloat(juce::ParameterID{"6.3", 1}, "samplerCutoff", juce::NormalisableRange<float>(20.0f, 20000.f, 0.f, 0.4), 20000.f);
    parameterMap.addParameter(samplerCutoff);
    mainCutoff = new juce::AudioParameterFloat(juce::ParameterID{"6.4", 1}, "mainCutoff", juce::NormalisableRange<float>(20.0f, 20000.f, 0.f, 0.4), 20000.f);
    parameterMap.addParameter(mainCutoff);
    osc1Res = new juce::AudioParameterFloat(juce::ParameterID{"7", 1}, "osc1Res", juce::NormalisableRange<float>(0.0f, 99.f), 10.f);
    parameterMap.addParameter(osc1Res);
    osc2Res = new juce::AudioParameterFloat(juce::ParameterID{"7.1", 1}, "osc2Res", juce::NormalisableRange<float>(0.0f, 99.f), 10.f);
    parameterMap.addParameter(osc2Res);
    noiseRes = new juce::AudioParameterFloat(juce::ParameterID{"7.2", 1}, "noiseRes", juce::NormalisableRange<float>(0.0f, 99.f), 10.f);
    parameterMap.addParameter(noiseRes);
    samplerRes = new juce::AudioParameterFloat(juce::ParameterID{"7.3", 1}, "samplerRes", juce::NormalisableRange<float>(0.0f, 99.f), 10.f);
    parameterMap.addParameter(samplerRes);
    mainRes = new juce::AudioParameterFloat(juce::ParameterID{"7.4", 1}, "mainRes", juce::NormalisableRange<float>(0.0f, 99.f), 10.f);
    parameterMap.addParameter(mainRes);
    osc1Keytrack = new juce::AudioParameterBool(juce::ParameterID{"7.51", 1}, "osc1Keytrack", false);
    parameterMap.addParameter(osc1Keytrack);
    osc2Keytrack = new juce::AudioParameterBool(juce::ParameterID{"7.52", 1}, "osc2Keytrack", false);
    parameterMap.addParameter(osc2Keytrack);
    noiseKeytrack = new juce::AudioParameterBool(juce::ParameterID{"7.53", 1}, "noiseKeytrack", false);
    parameterMap.addParameter(noiseKeytrack);
    samplerKeytrack = new juce::AudioParameterBool(juce::ParameterID{"7.54", 1}, "samplerKeytrack", false);
    parameterMap.addParameter(samplerKeytrack);
    osc1ktA = new juce::AudioParameterFloat(juce::ParameterID{"8.1", 1}, "osc1ktA", juce::NormalisableRange<float>(0.0f, 99.f), 99.f);
    parameterMap.addParameter(osc1ktA);
    osc2ktA = new juce::AudioParameterFloat(juce::ParameterID{"8.2", 1}, "osc2ktA", juce::NormalisableRange<float>(0.0f, 99.f), 99.f);
    parameterMap.addParameter(osc2ktA);
    noisektA = new juce::AudioParameterFloat(juce::ParameterID{"8.3", 1}, "noisektA", juce::NormalisableRange<float>(0.0f, 99.f), 99.f);
    parameterMap.addParameter(noisektA);
    samplerktA = new juce::AudioParameterFloat(juce::ParameterID{"8.4", 1}, "samplerktA", juce::NormalisableRange<float>(0.0f, 99.f), 99.f);
    parameterMap.addParameter(samplerktA);
    
    osc1Vol = new juce::AudioParameterFloat(juce::ParameterID{"10", 1}, "osc1Vol", juce::NormalisableRange<float>(0.0f, 99.f, 0.f, 1.4), 80.f);
    parameterMap.addParameter(osc1Vol);
    osc2Vol = new juce::AudioParameterFloat(juce::ParameterID{"10.1", 1}, "osc2Vol", juce::NormalisableRange<float>(0.0f, 99.f, 0.f, 1.4), 0.f);
    parameterMap.addParameter(osc2Vol);
    noiseVol = new juce::AudioParameterFloat(juce::ParameterID{"10.2", 1}, "noiseVol", juce::NormalisableRange<float>(0.0f, 99.f, 0.f, 1.4), 0.f);
    parameterMap.addParameter(noiseVol);
    samplerVol = new juce::AudioParameterFloat(juce::ParameterID{"10.3", 1}, "samplerVol", juce::NormalisableRange<float>(0.0f, 99.f, 0.f, 1.4), 0.f);
    parameterMap.addParameter(samplerVol);
    mainVol = new juce::AudioParameterFloat(juce::ParameterID{"10.4", 1}, "mainVol", juce::NormalisableRange<float>(0.0f, 99.f, 0.f, 1.4), 80.f);
    parameterMap.addParameter(mainVol);
    
    cThresh = new juce::AudioParameterFloat(juce::ParameterID{"11.1", 1}, "cThresh", juce::NormalisableRange<float>(-40.f, 0.f), -3.f);
    parameterMap.addParameter(cThresh);
    cRatio = new juce::AudioParameterFloat(juce::ParameterID{"11.2", 1}, "cRatio", juce::NormalisableRange<float>(1.f, 9.f), 9.f);
    parameterMap.addParameter(cRatio);
    cAtk = new juce::AudioParameterFloat(juce::ParameterID{"11.3", 1}, "cAtk", juce::NormalisableRange<float>(5.f, 200.f), 5.f);
    parameterMap.addParameter(cAtk);
    cRel = new juce::AudioParameterFloat(juce::ParameterID{"11.4", 1}, "cRel", juce::NormalisableRange<float>(5.f, 200.f), 40.f);
    parameterMap.addParameter(cRel);
    
    xParam1 = new juce::AudioParameterFloat(juce::ParameterID{"12.11", 1}, "xParam1", juce::NormalisableRange<float>(0.f, 1.f), 0.5);
    yParam1 = new juce::AudioParameterFloat(juce::ParameterID{"12.12", 1}, "yParam1", juce::NormalisableRange<float>(0.f, 1.f), 0.5);
    slopeParam1 = new juce::AudioParameterFloat(juce::ParameterID{"12.13", 1}, "slopeParam1", juce::NormalisableRange<float>(0.f, 1.f), 0);
    parameterMap.addParameter(xParam1);
    parameterMap.addParameter(yParam1);
    parameterMap.addParameter(slopeParam1);
    
    xParam2 = new juce::AudioParameterFloat(juce::ParameterID{"12.21", 1}, "xParam2", juce::NormalisableRange<float>(0.f, 1.f), 0.5);
    yParam2 = new juce::AudioParameterFloat(juce::ParameterID{"12.22", 1}, "yParam2", juce::NormalisableRange<float>(0.f, 1.f), 0.5);
    slopeParam2 = new juce::AudioParameterFloat(juce::ParameterID{"12.23", 1}, "slopeParam2", juce::NormalisableRange<float>(0.f, 1.f), 0);
    parameterMap.addParameter(xParam2);
    parameterMap.addParameter(yParam2);
    parameterMap.addParameter(slopeParam2);
    
    xParamN = new juce::AudioParameterFloat(juce::ParameterID{"12.31", 1}, "xParamN", juce::NormalisableRange<float>(0.f, 1.f), 0.5);
    yParamN = new juce::AudioParameterFloat(juce::ParameterID{"12.32", 1}, "yParamN", juce::NormalisableRange<float>(0.f, 1.f), 0.5);
    slopeParamN = new juce::AudioParameterFloat(juce::ParameterID{"12.33", 1}, "slopeParamN", juce::NormalisableRange<float>(0.f, 1.f), 0);
    parameterMap.addParameter(xParamN);
    parameterMap.addParameter(yParamN);
    parameterMap.addParameter(slopeParamN);
    
    xParamS = new juce::AudioParameterFloat(juce::ParameterID{"12.41", 1}, "xParamS", juce::NormalisableRange<float>(0.f, 1.f), 0.5);
    yParamS = new juce::AudioParameterFloat(juce::ParameterID{"12.42", 1}, "yParamS", juce::NormalisableRange<float>(0.f, 1.f), 0.5);
    slopeParamS = new juce::AudioParameterFloat(juce::ParameterID{"12.43", 1}, "slopeParamS", juce::NormalisableRange<float>(0.f, 1.f), 0);
    parameterMap.addParameter(xParamS);
    parameterMap.addParameter(yParamS);
    parameterMap.addParameter(slopeParamS);
    
    xParamM = new juce::AudioParameterFloat(juce::ParameterID{"12.51", 1}, "xParamM", juce::NormalisableRange<float>(0.f, 1.f), 0.5);
    yParamM = new juce::AudioParameterFloat(juce::ParameterID{"12.52", 1}, "yParamM", juce::NormalisableRange<float>(0.f, 1.f), 0.5);
    slopeParamM = new juce::AudioParameterFloat(juce::ParameterID{"12.53", 1}, "slopeParamM", juce::NormalisableRange<float>(0.f, 1.f), 0);
    parameterMap.addParameter(xParamM);
    parameterMap.addParameter(yParamM);
    parameterMap.addParameter(slopeParamM);
    
    parameterMap.save();
    
    juce::AudioProcessorValueTreeState valueTreeState(*this, nullptr, ProjectInfo::projectName, Utility::ParameterHelper::createParameterLayout());
    
    osc1 = new Colin::Synth();
    osc2 = new Colin::Synth();
    noise = new Colin::Synth();
    sampler = new Colin::Sampler();
    distMain = new Colin::Distortion();
    
    osc1Buffers.resize(8);
    osc2Buffers.resize(8);
    noiseBuffers.resize(8);
    samplerBuffers.resize(sampler->NUM_VOICES);
    
    for(int i=0; i<8; i++) {
        osc1Buffers[i] = new juce::AudioBuffer<float>(2, 512);
        osc2Buffers[i] = new juce::AudioBuffer<float>(2, 512);
        noiseBuffers[i] = new juce::AudioBuffer<float>(2, 512);
    }
    for(int i=0; i<sampler->NUM_VOICES; i++) {
        samplerBuffers[i] = new juce::AudioBuffer<float>(2, 512);
    }
    
    oscilloscope = new Colin::Oscilloscope();
    
    slopeFactors.resize(5);
    
    bezier1 = new AuxPort::Bezier(4096, AuxPort::Bezier::FourthOrder);
    bezier2 = new AuxPort::Bezier(4096, AuxPort::Bezier::FourthOrder);
    bezierN = new AuxPort::Bezier(4096, AuxPort::Bezier::FourthOrder);
    bezierS = new AuxPort::Bezier(4096, AuxPort::Bezier::FourthOrder);
    bezierM = new AuxPort::Bezier(4096, AuxPort::Bezier::FourthOrder);
    
    points1.resize(5);
    points2.resize(5);
    pointsN.resize(5);
    pointsS.resize(5);
    pointsM.resize(5);
    for(int i=0; i<5; i++) {
        points1[i] = new juce::Point<float>();
        points2[i] = new juce::Point<float>();
        pointsN[i] = new juce::Point<float>();
        pointsS[i] = new juce::Point<float>();
        pointsM[i] = new juce::Point<float>();
    }
}

CapstoneAudioProcessor::~CapstoneAudioProcessor()
{
    delete osc1;
    delete osc2;
    delete noise;
    delete distMain;
    for(int i=0; i<sampler->NUM_VOICES; i++) {
        delete samplerBuffers[i];
    }
    delete sampler;
    for (int i=0; i<8; i++) {
        delete osc1Buffers[i];
        delete osc2Buffers[i];
        delete noiseBuffers[i];
    }
    globalVoices.clear();

    delete oscilloscope;
    delete bezier1;
    delete bezier2;
    delete bezierN;
    delete bezierS;
    delete bezierM;
}

//==============================================================================
const juce::String CapstoneAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CapstoneAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CapstoneAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CapstoneAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CapstoneAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CapstoneAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CapstoneAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CapstoneAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CapstoneAudioProcessor::getProgramName (int index)
{
    return {};
}

void CapstoneAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CapstoneAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    osc1->prepareToPlay(spec);
    osc1->setOscillator(3);
    osc2->prepareToPlay(spec);
    osc2->setOscillator(1);
    noise->prepareToPlay(spec);
    noise->setNoise(true);
    noise->setOscillator(1);
    sampler->prepareToPlay(spec);

    distMain->setType(1);
    distMain->setInputGain(3.f);
    distMain->setOutputGain(0.f);
    
    limiter.reset();
    limiter.prepare(spec);
    limiter.setRatio(10);
    limiter.setAttack(10);
    limiter.setRelease(100);
    limiter.setThreshold(-1);
        
    ladderM.reset();
    ladderM.prepare(spec);
    ladderM.setMode(juce::dsp::LadderFilterMode::LPF24);
    ladderM.setCutoffFrequencyHz(20000);
    ladderM.setResonance(.1); // value between 0 and 1
    
    rmsLevelLeft.reset(sampleRate, 0.5);
    rmsLevelRight.reset(sampleRate, 0.5);

    rmsLevelLeft.setCurrentAndTargetValue(-100.f);
    rmsLevelRight.setCurrentAndTargetValue(-100.f);
    
    oscilloscope->clear();
    
    prepareBezier(bezier1, xParam1, yParam1, slopeParam1, points1, 0);
    prepareBezier(bezier2, xParam2, yParam2, slopeParam2, points2, 1);
    prepareBezier(bezierN, xParamN, yParamN, slopeParamN, pointsN, 2);
    prepareBezier(bezierS, xParamS, yParamS, slopeParamS, pointsS, 3);
    prepareBezier(bezierM, xParamM, yParamM, slopeParamM, pointsM, 4);
    
    ADSRparams = new juce::ADSR::Parameters(0.55, 0.5, 0.8, 0.9);
}

void CapstoneAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CapstoneAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void CapstoneAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();

    /// Prepare AudioBuffers for storing new audio data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear (i, 0, numSamples);
        
        /// Resize buffers if necessary
        if(osc1Buffers[0]->getNumSamples() != numSamples) {
            for(int j=0; j<8; j++) {
                osc1Buffers[j]->setSize(numChannels, numSamples);
                osc2Buffers[j]->setSize(numChannels, numSamples);
                noiseBuffers[j]->setSize(numChannels, numSamples);
            }
            for(int j=0; j<sampler->NUM_VOICES; j++) {
                samplerBuffers[j]->setSize(numChannels, numSamples);
            }
        }
        
        /// Clear buffers that currently contain audio data
        for(int j=0; j<8; j++) {
            osc1Buffers[j]->clear(i, 0, numSamples);
            osc2Buffers[j]->clear(i, 0, numSamples);
            noiseBuffers[j]->clear(i, 0, numSamples);
        }
        for(int j=0; j<sampler->NUM_VOICES; j++) {
            samplerBuffers[j]->clear(i, 0, numSamples);
        }
    }
    
    /// Update global ADSR and check for new MIDI events to create new global voices
    enableADSR(midiMessages, buffer.getNumChannels(), buffer.getNumSamples());
    setADSR(*mainAtk / 20.f + 0.05f, *mainDec / 20.f, *mainSus / 100.f, std::powf(*mainRel, 1.4f) / 100.f);
    
    /// SAMPLER
    if(*samplerDistSel == 2) setBezier(bezierS, xParamS, yParamS, slopeParamS, pointsS, 3);
    sampler->setDistortion(*samplerDistSel, *samplerDAmt / 10.f, *samplerDAmt / -15.f - 3.f, *samplerDCoeff / 100.f, *samplerDistSlider / 100.f, bezierS);
    sampler->setSampleLength(*samplerWaveSlider / 100.f);
    if(sampler->isSampleLoaded() && *samplerVol != 0) {
        sampler->setLoop(*samplerLoop);
        sampler->setPitch(*samplerPitch, *samplerRepitch);
        sampler->setADSR(*samplerAtk / 30 + 0.05, *samplerDec / 30, *samplerSus / 100, std::powf(*samplerRel, 1.2) / 100, *samplerDepth / 100);
        sampler->setFilter(*samplerFilter, *samplerCutoff, (*samplerRes + 1) / 101, *samplerKeytrack, *samplerktA / 100 + 1);
        sampler->setEnvRouting(*sampleretV, *sampleretD, *sampleretF);
        sampler->processBuffers(samplerBuffers, midiMessages);
    }
    
    /// NOISE
    noise->setOscillator(*noiseWave);
    if(*noiseDistSel == 2) setBezier(bezierN, xParamN, yParamN, slopeParamN, pointsN, 2);
    noise->setDistortion(*noiseDistSel, *noiseDAmt / 10.f, *noiseDAmt / -15.f - 3.f, *noiseDCoeff / 100, *noiseDistSlider / 100, bezierN);
    //if(*noiseVol != 0 || *fmAmt2 != 0) {
    noise->setOscVol(*noiseWaveSlider/100);
    noise->setADSR(*noiseAtk / 30 + 0.05, *noiseDec / 30, *noiseSus / 100, std::powf(*noiseRel, 1.2) / 100, *noiseDepth / 100);
    noise->setFilter(*noiseFilter, *noiseCutoff, (*noiseRes + 1) / 101, *noiseKeytrack, *noisektA / 100 + 1);
    noise->setEnvRouting(*noiseetV, *noiseetD, *noiseetF);
    //noise->processBuffers(noiseBuffers, midiMessages);
    for(int i=0; i<globalVoices.size(); i++) {
        noise->processBuffer(globalVoices[i]->getNoise(), midiMessages, i);
    }
    
    /// OSC 2
    osc2->setOscillator(*osc2Wave);
    if(*osc2DistSel == 2) setBezier(bezier2, xParam2, yParam2, slopeParam2, points2, 1);
    osc2->setDistortion(*osc2DistSel, *osc2DAmt / 10.f, *osc2DAmt / -15.f - 3.f, *osc2DCoeff / 100, *osc2DistSlider / 100, bezier2);
    //if(*osc2Vol != 0 || *fmAmt1 != 0) {
    osc2->setOscVol(*osc2WaveSlider/100);
    osc2->setPitch(*osc2Pitch);
    osc2->setADSR(*osc2Atk / 30 + 0.05, *osc2Dec / 30, *osc2Sus / 100, std::powf(*osc2Rel, 1.2) / 100, *osc2Depth / 100);
    osc2->setFilter(*osc2Filter, *osc2Cutoff, (*osc2Res + 1) / 101, *osc2Keytrack, *osc2ktA / 100 + 1);
    osc2->setEnvRouting(*osc2etV, *osc2etD / 10, *osc2etF);
    if(*fmAmt2 != 0) {
        osc2->setFMDepth(*fmAmt2 / 25);
        for(int i=0; i<globalVoices.size(); i++) {
            osc2->processBufferFM(globalVoices[i]->getOsc2(), globalVoices[i]->getNoise(), midiMessages, i);
        }
    }
    else {
        for(int i=0; i<globalVoices.size(); i++) {
            osc2->processBuffer(globalVoices[i]->getOsc2(), midiMessages, i);
        }
    }
    
    /// OSC 1
    osc1->setOscillator(*osc1Wave);
    osc1->setOscVol(*osc1WaveSlider/100);
    osc1->setPitch(*osc1Pitch);
    osc1->setADSR(*osc1Atk / 30.f + 0.05, *osc1Dec / 30.f, *osc1Sus / 100.f, std::powf(*osc1Rel, 1.2) / 100.f, *osc1Depth / 100.f);
    osc1->setFilter(*osc1Filter, *osc1Cutoff, (*osc1Res + 1) / 101, *osc1Keytrack, *osc1ktA / 100 + 1);
    if(*osc1DistSel == 2) setBezier(bezier1, xParam1, yParam1, slopeParam1, points1, 0);
    osc1->setDistortion(*osc1DistSel, *osc1DAmt / 10.f, *osc1DAmt / -15.f - 3.f, *osc1DCoeff / 100.f, *osc1DistSlider / 100.f, bezier1);
    osc1->setEnvRouting(*osc1etV, *osc1etD, *osc1etF);
    if(*fmAmt1 != 0) {
        osc1->setFMDepth(*fmAmt1 / 25);
        for(int i=0; i<globalVoices.size(); i++) {
            osc1->processBufferFM(globalVoices[i]->getOsc1(), globalVoices[i]->getOsc2(), midiMessages, i);
        }
    }
    else {
        for(int i=0; i<globalVoices.size(); i++) {
            osc1->processBuffer(globalVoices[i]->getOsc1(), midiMessages, i);
        }
    }
    
    /// Apply volume envelope (set in main tab) to each of the sources
    for(int i=0; i<globalVoices.size(); i++) {
        globalVoices[i]->applyADSR();
    }
    
    for(int i=0; i<globalVoices.size(); i++) {
        globalVoices[i]->setVolume(*osc1Vol/90.f, *osc2Vol/90.f, *noiseVol/90.f, *samplerVol/90.f);
    }
    
    /// SUM
    for(int channel = 0; channel < numChannels; channel++) {
        for(int sample = 0; sample < numSamples; sample++) {
            float output = 0.f;
            for(int i=0; i<globalVoices.size(); i++) {
                output = output + globalVoices[i]->getSample(channel, sample);
            }
            buffer.getWritePointer(channel)[sample] = output * *mainVol/100;
        }
    }
    
    distMain->setType(*mainDistSel);
    distMain->setInputGain(*mainDAmt / 10.f);
    distMain->setCoeff(*mainDCoeff/100);
    distMain->setMix(*mainDistSlider/100);
    distMain->setOutputGain(0);
    if(*mainDistSel == 2) {
        setBezier(bezierM, xParamM, yParamM, slopeParamM, pointsM, 4);
        distMain->processBufferWaveshaper(buffer, bezierM);
    }
    else distMain->processBuffer(buffer);
    
    juce::dsp::AudioBlock<float> blockMain(buffer);
    auto pcM = juce::dsp::ProcessContextReplacing<float>(blockMain);
    ladderM.setMode(getFilterMode(*mainFilter));
    ladderM.setCutoffFrequencyHz(*mainCutoff);
    ladderM.setResonance((*mainRes + 1) / 101);
    ladderM.process(pcM);
    
    limiter.setThreshold(*cThresh);
    limiter.setRatio(*cRatio);
    limiter.setAttack(*cAtk);
    limiter.setRatio(*cRel);
    limiter.process(pcM);
        
    rmsLevelLeft.skip(numSamples);
    rmsLevelRight.skip(numSamples);
    {
        const auto value = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, numSamples));
        if (value < rmsLevelLeft.getCurrentValue())
            rmsLevelLeft.setTargetValue(value);
        else
            rmsLevelLeft.setCurrentAndTargetValue(value);
        }
    {
        const auto value = juce::Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, numSamples));
        if (value < rmsLevelRight.getCurrentValue())
            rmsLevelRight.setTargetValue(value);
        else
            rmsLevelRight.setCurrentAndTargetValue(value);
    }
    
    oscilloscope->setBuffer(*mainWaveSlider * 2 + 32);
    oscilloscope->pushBuffer(buffer);
}

//==============================================================================
bool CapstoneAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CapstoneAudioProcessor::createEditor()
{
    return new CapstoneAudioProcessorEditor (*this);
}

//==============================================================================
void CapstoneAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    //DBG("sending audio parameters to ableton");
        
    parameterMap.writeParams(destData);
    /*
#if TARGET_CPU_X86_64
    //destData.insert(<#const void *dataToInsert#>, <#size_t numBytesToInsert#>, <#size_t insertPosition#>);
    
#else
    //destData.insert(<#const void *dataToInsert#>, <#size_t numBytesToInsert#>, <#size_t insertPosition#>);
#endif
    */
}

void CapstoneAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    //DBG("getting audio parameters from ableton");
    parameterMap.readParams(data, sizeInBytes);
    
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CapstoneAudioProcessor();
}


//==============================================================================
/// Custom functions not from the JUCE template

juce::dsp::LadderFilterMode CapstoneAudioProcessor::getFilterMode(int type) {
    if(type == 1) return juce::dsp::LadderFilterMode::LPF12;
    else if(type == 2) return juce::dsp::LadderFilterMode::LPF24;
    else if(type == 3) return juce::dsp::LadderFilterMode::HPF12;
    else if(type == 4) return juce::dsp::LadderFilterMode::HPF24;
}

float CapstoneAudioProcessor::getRmsValue(const int channel) const
{
    jassert(channel == 0 || channel == 1);
    if (channel == 0)
        return rmsLevelLeft.getCurrentValue();
    if (channel == 1)
        return rmsLevelRight.getCurrentValue();
    return 0.f;
}

void CapstoneAudioProcessor::randomizeParams() {
    std::vector<juce::String> paramsToSkip;
    paramsToSkip.push_back(juce::String("mainVol"));
    paramsToSkip.push_back(juce::String("mainWaveSlider"));
    //paramsToSkip.push_back(juce::String(""));
    parameterMap.randomize(paramsToSkip);
    setBezier(bezier1, xParam1, yParam1, slopeParam1, points1, 0);
}

/// These functions are for implementing and applying a global volume envelope to each of the sources and voices

void CapstoneAudioProcessor::enableADSR(juce::MidiBuffer& midiMessages, int bufChan, int bufSize) {
    for (const auto midiMessage : midiMessages) {
        const auto midiEvent = midiMessage.getMessage();
        int note = midiEvent.getNoteNumber();
        if(midiEvent.isNoteOn()) {
            std::unique_ptr<globalVoice> v = std::make_unique<globalVoice>(spec.sampleRate, ADSRparams, note, bufChan, bufSize);
            globalVoices.push_back(std::move(v));
            if(globalVoices.size() > 8) {
                globalVoices.erase(globalVoices.begin());
            }
        }
        if(midiEvent.isNoteOff()) {
            for(int i=0; i<globalVoices.size(); i++) {
                if(globalVoices[i]->getPitch() == note && !globalVoices[i]->isRelease()) {
                    globalVoices[i]->noteOff();
                    globalVoices[i]->setRelease();
                }
            }
        }
    }
    for(int i=0; i<globalVoices.size(); i++) {
        if(!globalVoices[i]->isActive()) {
            globalVoices.erase(globalVoices.begin()+i);
            osc1->deleteVoice(i);
            osc2->deleteVoice(i);
            noise->deleteVoice(i);
            //sampler->deleteVoice(i);
        }
    }
}

void CapstoneAudioProcessor::setADSR(float atk, float dec, float sus, float rel) {
    if(atk == ADSRparams->attack && dec == ADSRparams->decay && sus == ADSRparams->sustain && rel == ADSRparams->release) return;
    ADSRparams = new juce::ADSR::Parameters(atk, dec, sus, rel);
    for(int i=0; i<globalVoices.size(); i++) {
        //ADSRs[i].reset();
        // JUCE documentation recommends calling reset() when changing ADSR parameters, but that's lame
        globalVoices[i]->setADSRParameters(*ADSRparams);
    }
}

/// The next two functions are for the bezier used for the arbitrary waveshaping

void CapstoneAudioProcessor::prepareBezier(AuxPort::Bezier* b, juce::AudioParameterFloat* x, juce::AudioParameterFloat* y, juce::AudioParameterFloat* s, std::vector<juce::Point<float>*> points, int tab) {
    if ((*x) != points[2]->x)
        points[2]->setX(*x);
    if ((*y) != points[2]->y)
        points[2]->setY(juce::jmap<float>(*y, -10, 10));
    if (slopeFactors[tab] != *s)
        slopeFactors[tab] = *s;
    points[0]->setXY(0, -0);
    points[4]->setXY(1, 1);
    points[1]->setXY((1 - slopeFactors[tab]) * points[2]->getX(), (1 - slopeFactors[tab]) * points[2]->getY());
}

void CapstoneAudioProcessor::setBezier(AuxPort::Bezier* b, juce::AudioParameterFloat* x, juce::AudioParameterFloat* y, juce::AudioParameterFloat* s, std::vector<juce::Point<float>*> points, int tab) {
    
    if ((*x) != points[2]->x)
        points[2]->setX(*x);
    if ((*y) != points[2]->y)
        points[2]->setY(*y);
    if (slopeFactors[tab] != *s)
        slopeFactors[tab] = *s;
    
    points[1]->setXY((1 - slopeFactors[tab]) * points[2]->getX(), (1 - slopeFactors[tab]) * points[2]->getY());
    points[3]->setXY((1 + slopeFactors[tab]) * points[2]->getX(), (1 + slopeFactors[tab]) * points[2]->getY());

    b->setPoint(*points[0], 0);
    b->setPoint(*points[1], 1);
    b->setPoint(*points[2], 2);
    b->setPoint(*points[3], 3);
    b->setPoint(*points[4], 4);

    b->calcPoints();
    b->drawWaveshaper();
}

/*
void CapstoneAudioProcessor::applyADSR(std::vector<juce::AudioBuffer<float>*>& buffers) {
    float envSample = 0;
    for(int i=0; i<ADSRs.size(); i++) {
        envSample = ADSRs[i]->getNextSample();
        for(int channel=0; channel<buffers[i]->getNumChannels(); channel++) {
            auto writePointer = buffers[activeADSRs[i]]->getWritePointer(channel);
            auto readPointer = buffers[activeADSRs[i]]->getReadPointer(channel);
            for(int sample=0; sample<buffers[i]->getNumSamples(); sample++) {
                writePointer[sample] = readPointer[sample] * envSample;
            }
        }
    }
}

void CapstoneAudioProcessor::applyADSRSampler(std::vector<juce::AudioBuffer<float>*>& buffers) {
    float envSample = 0;
    for(int i=0; i<buffers.size(); i++) {
        if(ADSRs[sampler->curPitch[i]].isActive()) {
            envSample = ADSRs[sampler->curPitch[i]].getNextSample();
            for(int channel=0; channel<buffers[i]->getNumChannels(); channel++) {
                auto writePointer = buffers[i]->getWritePointer(channel);
                auto readPointer = buffers[i]->getReadPointer(channel);
                for(int sample=0; sample<buffers[i]->getNumSamples(); sample++) {
                    writePointer[sample] = readPointer[sample] * envSample;
                }
            }
        }
    }
}
 
 float CapstoneAudioProcessor::getSampleFromBuffers(std::vector<juce::AudioBuffer<float>*>& buffers, int channel, int sample) {
     float output = 0.f;
     for(int i=0; i<activeADSRs.size(); i++) {
         output += buffers[activeADSRs[i]]->getReadPointer(channel)[sample];
     }
     return output;
 }
  

 float CapstoneAudioProcessor::getSampleFromSampler(std::vector<juce::AudioBuffer<float>*>& buffers, int channel, int sample) {
     float output = 0.f;
     for(int i=0; i<buffers.size(); i++) {
         output += buffers[i]->getReadPointer(channel)[sample];
     }
     return output;
 }
*/
