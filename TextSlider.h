#ifndef Colin_TEXTSLIDER_H
#define Colin_TEXTSLIDER_H

/*
  ==============================================================================

    TextSlider.h
    Created: 10 Mar 2023 5:44:27pm
    Author:  Colin Raab
 
    Custom component that functions like a slider but only displays numbers
    2, 3, or 4 digit sliders can be made
    Drag up or down on the numbers to adjust the slider, double-click to reset
    Mostly designed for this particular use case, might make more broadly applicable in the future

  ==============================================================================
*/

#include <JuceHeader.h>

class TextSlider : public juce::Component
{
  public:
    TextSlider(juce::AudioParameterFloat* param)
    {
        addAndMakeVisible (textBox);
        textBox.setReadOnly(true);
        // makes mouse events pass through to TextSlider:
        textBox.setInterceptsMouseClicks (false, false);
        this->param = param;
        juce::Typeface::Ptr tface = juce::Typeface::createSystemTypefaceFor(BinaryData::EHSMB_TTF, BinaryData::EHSMB_TTFSize);
        juce::Font led = juce::Font(tface);
        led.setHeight(15);
        textBox.setFont(led);
        textBox.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        textBox.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        textBox.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
    }
    
    TextSlider() {
        addAndMakeVisible (textBox);
        textBox.setReadOnly(true);
        // makes mouse events pass through to TextSlider:
        textBox.setInterceptsMouseClicks (false, false);
    }
    
    void setParam(juce::AudioParameterFloat* param) {
        this->param = param;
    }
    
    void setRange(float start, float end, float current, float interval, float skew=1);

    void resized() override { textBox.setBounds (getLocalBounds()); }

    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseDoubleClick (const juce::MouseEvent& e) override;
    
    void setRepaint(bool y) { shouldRepaint = y; }
    
    void paint(juce::Graphics& g) override;
    
    void setText(std::string text);
    void update(const juce::MouseEvent& e);
    void updateText();
    float getValue();
    void setValue();
    void setThreeChar(int isThree);
    void setDistSlider(bool y) { isDistSlider = y; shouldRepaint = true; }
    void setMainSlider(bool y) { isMainSlider = y; shouldRepaint = true; }
    void setPitchSlider(bool y) { isPitchSlider = y; }
    
  private:
    juce::TextEditor textBox;
    juce::NormalisableRange<float> range;
    int defaultVal;
    int startVal;
    int currentVal;
    int oldDiff = 0;
    int interval = 2;
    std::mutex sliderMutex;
    juce::AudioParameterFloat* param;
    int threechar = 0; // 0 = 2 char, 1 = three char +/-, 2 = three char
    bool shouldRepaint = false;
    bool isDistSlider = false;
    bool isMainSlider = false;
    bool isPitchSlider = false;
};

#endif
