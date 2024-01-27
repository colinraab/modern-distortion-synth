/*
  ==============================================================================

    AuxSearch.cpp
    Created: 1 Apr 2023 6:44:47pm
    Author:  Satyarth

  ==============================================================================
*/

#include "AuxSearch.h"

uint32_t AuxPort::AuxSearch::search(const std::vector<juce::Point<float>>& vector, const float& value, const bool& nearest)
{
    if (value < vector[0].getY())
        return 0;
    if (value > vector[vector.size() - 1].getY())
        return vector.size() - 1;
    uint32_t end = vector.size() - 1;
    uint32_t start = 0;
    uint32_t mid = 0;
    while (end >=start)
    {
        mid = (start + end) / 2;
        if (value < vector[mid].getX())
            end = mid - 1;
        else if (value > vector[mid].getX())
            start = mid + 1;
        else
            return mid;
    }
    return nearest == true ? (abs(vector[start].getX() - value) < abs(value - vector[end].getX()) ? start : end):-1;
     

}
