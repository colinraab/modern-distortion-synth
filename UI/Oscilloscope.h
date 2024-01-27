#ifndef Colin_OSCILLOSCOPE_H
#define Colin_OSCILLOSCOPE_H

/*
  ==============================================================================

    Oscilloscope.h
    Created: 8 Apr 2023 6:52:04pm
    Author:  Colin Raab

  ==============================================================================
*/

#include "JuceHeader.h"

namespace Colin {

class Oscilloscope : public juce::AudioVisualiserComponent {
private:
    juce::Colour newgold = juce::Colour(juce::uint8(74), juce::uint8(64), juce::uint8(15));
    
public:
    Oscilloscope() : juce::AudioVisualiserComponent(1) {
        setBufferSize(128);
        setSamplesPerBlock(16);
        setColours(newgold, juce::Colours::lightblue);
    }
    
    void setBuffer(int bufSize) {
        setBufferSize(bufSize);
    }
};

}


#endif
