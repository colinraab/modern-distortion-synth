/*
  ==============================================================================

    The PluginEditor manages the user interface elements of the plugin
    and synchronizes with the PluginProcessor to send/recieve parameter data

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "../TextSlider.h"
#include "../UI/TabPages.h"
#include "../GainMeter.h"
#include "../Presets/PresetPanel.h"

//==============================================================================

/// Set global font for text

class newLookAndFeel : public juce::LookAndFeel_V4 {
public:
    newLookAndFeel() {
        juce::Typeface::Ptr tface = juce::Typeface::createSystemTypefaceFor(BinaryData::Avenir_Regular_ttf, BinaryData::Avenir_Regular_ttfSize);
        setDefaultSansSerifTypeface (tface);
    }
};

/// Custom slider component definitions

class blackKnob : public juce::LookAndFeel_V4 {
public:
    blackKnob();
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;
private:
    juce::Image img1;
};

class silverKnob : public juce::LookAndFeel_V4 {
public:
    silverKnob();
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;
private:
    juce::Image img1;
};

class CapstoneAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    CapstoneAudioProcessorEditor (CapstoneAudioProcessor&);
    ~CapstoneAudioProcessorEditor() override;

    //==============================================================================
    
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    
    Colin::Osc1Page* osc1Page;
    Colin::Osc2Page* osc2Page;
    Colin::NoisePage* noisePage;
    Colin::SamplerPage* samplerPage;
    Colin::MainPage* mainPage;
    
    void updateParamUI();
    Gui::PresetPanel presetPanel;


private:
    CapstoneAudioProcessor& audioProcessor;
    
    juce::ImageButton randomize {"RAND"};
    juce::ImageButton init {"INIT"};
    
    juce::Slider osc1V;
    juce::Slider osc2V;
    juce::Slider noiseV;
    juce::Slider samplerV;
    juce::Slider mainV;
    juce::Slider mainC;
    juce::Slider mainD;
    
    juce::AudioParameterFloat* osc1Vol;
    juce::AudioParameterFloat* osc2Vol;
    juce::AudioParameterFloat* noiseVol;
    juce::AudioParameterFloat* samplerVol;
    juce::AudioParameterFloat* mainVol;
    juce::AudioParameterFloat* mainDAmt;
    juce::AudioParameterFloat* mainCutoff;
        
    juce::TabbedComponent mainWindow;
    
    newLookAndFeel newLook;
    blackKnob bk;
    silverKnob sk;
        
    Colin::VerticalDiscreteMeter verticalDiscreteMeterL, verticalDiscreteMeterR;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CapstoneAudioProcessorEditor)
};
