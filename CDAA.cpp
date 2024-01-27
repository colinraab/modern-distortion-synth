/*
  ==============================================================================

    CDAA.cpp
    Created: 13 Mar 2023 4:36:40pm
    Author:  Colin Raab

  ==============================================================================
*/

#include "CDAA.h"

Colin::Bezier::Bezier(uint32_t points)
{
    bezierVector.resize(points);
}

void Colin::Bezier::setNumberOfPoints(const uint32_t& points)
{
    bezierVector.resize(points);
}

void Colin::Bezier::calcPoints(std::vector<juce::Point<float> > points)
{
    for (uint32_t i = 0; i < bezierVector.size(); i++)
    {
        float ii = static_cast<float>(i / static_cast<float>(bezierVector.size()));
        if(points.size() == 4) {
            bezierVector[i].x = points[0].x + 3*ii*(-1*points[0].x + points[1].x) + 3*powf(ii,2)*(points[0].x - 2*points[1].x + points[2].x) + powf(ii,3)*(-1*points[0].x + 3*points[1].x + 3*points[2].x + points[3].x);
            bezierVector[i].y = points[0].y + 3*ii*(-1*points[0].y + points[1].y) + 3*powf(ii,2)*(points[0].y - 2*points[1].y + points[2].y) + powf(ii,3)*(-1*points[0].y + 3*points[1].y + 3*points[2].y + points[3].y);
        }
        else if(points.size() == 5) {
            bezierVector[i].x = points[0].x + 4*ii*(points[0].x+points[1].x) + 6*powf(ii,2)*(points[0].x-2*points[1].x+points[2].x) + 4*powf(ii,3)*(-1*points[0].x+3*points[1].x-3*points[2].x+points[3].x) + powf(ii,4)*(points[0].x-4*points[1].x+6*points[2].x-4*points[3].x+points[4].x);
            bezierVector[i].y = points[0].y + 4*ii*(points[0].y+points[1].y) + 6*powf(ii,2)*(points[0].y-2*points[1].y+points[2].y) + 4*powf(ii,3)*(-1*points[0].y+3*points[1].y-3*points[2].y+points[3].y) + powf(ii,4)*(points[0].y-4*points[1].y+6*points[2].y-4*points[3].y+points[4].y);
        }
    }
}

juce::Point<float>* Colin::Bezier::getPoint(const uint32_t& index)
{
    return &bezierVector[index];
}

