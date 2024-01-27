/*
  ==============================================================================

    AuxShaper.h
    Created: 28 Mar 2023 5:13:37pm
    Author:  satya

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../AuxParam.h"
#include "AuxBezier.h"

//==============================================================================
/*
*/
class AuxShaper  : public juce::Component
{
public:
    AuxShaper() = default;
    AuxShaper(AuxPort::ParameterMap* parameterMap);
    AuxShaper(juce::AudioParameterFloat* x1, juce::AudioParameterFloat* y1, juce::AudioParameterFloat* slope);
    ~AuxShaper() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    class AuxPoint : public juce::Component
    {
    public:
        AuxPoint(juce::AudioParameterFloat* x,juce::AudioParameterFloat* y,juce::AudioParameterFloat* slope);
        ~AuxPoint() override;
        void paint(juce::Graphics&) override;
        void resized() override;
        void setColour(const juce::Colour& colour);
        juce::Colour* getColour();
        void mouseDrag(const juce::MouseEvent& e) override;
        void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;
        void mouseUp(const juce::MouseEvent& e) override;
        void setPointBounds(const juce::Rectangle<int> pointBounds);
        juce::Rectangle<int>* getPointBounds();
        void setCoordinate(int x, int y);
        juce::Point<int>* getCoordinate();
        juce::Point<int>* getNegBand();
        juce::Point<int>* getPosBand();
        void setText(const juce::String& text);
    private:
        void limit(juce::Point<int>* point);
        juce::AudioParameterFloat* x;
        juce::AudioParameterFloat* y;
        juce::AudioParameterFloat* slope;
        
        juce::Point<int> xyCoordinate;
        juce::Point<int> negBand;
        juce::Point<int> posBand;
        juce::Point<int> origin;
        juce::Rectangle<int> pointBounds;
        juce::Colour pointColour;
        juce::Path ellipsePath;
        std::mutex pointMutex;
        juce::String text;
        float slopeFactor;
        bool drawingBool;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AuxPoint)
    };
    
    AuxPoint p1;
    AuxPort::Bezier bezier;
    AuxPort::ParameterMap* parameterMap;
    juce::Point<float> normalize;
    juce::Point<int> startPoint;
    juce::Point<int> endPoint;
    bool drawingBool;
    
    juce::AudioParameterFloat* xP;
    juce::AudioParameterFloat* yP;
    juce::AudioParameterFloat* slopeP;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AuxShaper)
};
