#ifndef COLIN_CDAA_H
#define COLIN_CDAA_H

/*
  ==============================================================================

    CDAA.h
    Created: 13 Mar 2023 4:36:40pm
    Author:  Colin Raab

  ==============================================================================
*/

#include <math.h>
#include <JuceHeader.h>
#include <vector>

namespace Colin {

class Bezier {
public:
    Bezier() = default;
    Bezier(uint32_t points);
    ~Bezier() = default;
    Bezier(const Bezier& bezier) = default;
    void setNumberOfPoints(const uint32_t& points);
    void calcPoints(std::vector<juce::Point<float>> points);
    juce::Point<float>* getPoint(const uint32_t& index);
private:
    std::vector<juce::Point<float>> bezierVector;
};

class CDAA {
public:
    CDAA() = default;
    ~CDAA() = default;
    void setBezier(Bezier bezier);
    
private:
    
};

}

#endif
