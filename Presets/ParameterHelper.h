#pragma once

#include <JuceHeader.h>

namespace Utility
{
    class ParameterHelper
    {
    public:
        ParameterHelper() = delete;

        static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
        {
            return juce::AudioProcessorValueTreeState::ParameterLayout{
                
            };
        }
    };
}
