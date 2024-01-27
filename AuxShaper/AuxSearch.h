#ifndef AUXPORT_SEARCH_H
#define AUXPORT_SEARCH_H
/*
  ==============================================================================

    AuxSearch.h
    Created: 1 Apr 2023 6:44:47pm
    Author:  Satyarth

  ==============================================================================
*/
#include <vector>
#include "JuceHeader.h"

namespace AuxPort
{
    class AuxSearch
    {
    public:
        AuxSearch() = default;;
        ~AuxSearch() = default;
        AuxSearch(const AuxSearch& auxSearch) = default;
        uint32_t search(const std::vector<juce::Point<float>>& vector, const float& value,const bool& nearest);       
    };
}


#endif


