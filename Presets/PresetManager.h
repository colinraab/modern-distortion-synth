/*
  ==============================================================================

    PresetManager.h
    Created: 8 Jun 2023 1:52:58pm
    Author:  Colin Raab
 
     Adapted from Akash Murthy's Preset Manager tutorial
     https://www.youtube.com/watch?v=YwAtWuGA4Cg
 
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../AuxParam.h"

namespace Service
{
    class PresetManager : juce::ValueTree::Listener
    {
    public:
        static const juce::File defaultDirectory;
        static const juce::String extension;
        static const juce::String presetNameProperty;

        PresetManager(AuxPort::ParameterMap*);

        void savePreset(const juce::String& presetName);
        void deletePreset(const juce::String& presetName);
        void loadPreset(const juce::String& presetName);
        int loadNextPreset();
        int loadPreviousPreset();
        juce::StringArray getAllPresets() const;
        juce::String getCurrentPreset() const;
    private:
        void valueTreeRedirected(juce::ValueTree& treeWhichHasBeenChanged) override;

        //juce::AudioProcessorValueTreeState& valueTreeState;
        juce::Value currentPreset;
        
        AuxPort::ParameterMap* parameterMap;
    };
}
