#ifndef AUXPORT_BEZIER_H
#define AUXPORT_BEZIER_H

/*
*			AuxBezier : Quadratic Bezier Curve Implementation for JUCE
            "Shape it, play with it" - inpinseptipin
            BSD 3-Clause License
            Copyright (c) 2023, Satyarth Arora
            All rights reserved.
            Redistribution and use in source and binary forms, with or without
            modification, are permitted provided that the following conditions are met:
            1. Redistributions of source code must retain the above copyright notice, this
               list of conditions and the following disclaimer.
            2. Redistributions in binary form must reproduce the above copyright notice,
               this list of conditions and the following disclaimer in the documentation
               and/or other materials provided with the distribution.
            3. Neither the name of the copyright holder nor the names of its
               contributors may be used to endorse or promote products derived from
               this software without specific prior written permission.
            THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
            AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
            IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
            DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
            FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
            DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
            SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
            CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
            OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
            OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <vector>
#include "JuceHeader.h"
#include "AuxSearch.h"
namespace AuxPort
{
    class Bezier
    {
    public:
        enum Type
        {
            Quadratic = 3, Cubic = 4, FourthOrder = 5, FifthOrder = 6, SixthOrder = 7
        };
        enum PointType
        {
            Vector, Parameter,WaveshapeVector
        };
        Bezier() = default;
        Bezier(uint32_t points, Type curveType);
        ~Bezier() = default;
        Bezier(const Bezier& bezier) = default;
        void calcPoints();
        void setPoint(const juce::Point<int>& point, uint32_t index, const juce::Point<float> normalize = { 1,1 });
        void setPoint(const juce::Point<float>& point, uint32_t index);
        juce::Point<float>* getPoint(const uint32_t& index, const PointType& pointType = PointType::Vector);
        uint32_t getSize();
        void setSize(uint32_t size);
        void drawWaveshaper();
        uint32_t search(const float& sample);
    private:
        std::vector<juce::Point<float>> bezierVector;
        std::vector <juce::Point<float>> waveshaperVector;
        std::vector<juce::Point<float>> bezierPoints;
        uint32_t noOfPoints;
        juce::Point<float> phaseInvert = { -1,-1 };
        Type curveType = { Type::Quadratic };
        AuxSearch auxSearch;
        
    };
}

#endif



