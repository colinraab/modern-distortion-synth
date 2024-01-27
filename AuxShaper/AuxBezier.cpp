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

#include "AuxBezier.h"


AuxPort::Bezier::Bezier(uint32_t points, Type curveType)
{
    bezierVector.resize(points);
    waveshaperVector.resize(2 * points);
    this->curveType = curveType;
    bezierPoints.resize(static_cast<uint32_t>(this->curveType));
    noOfPoints = points;
    for (uint32_t i = 0; i < bezierPoints.size(); i++)
        bezierPoints[i] = { 0,0 };
    

}

void AuxPort::Bezier::calcPoints()
{
    for (uint32_t i = 0; i < bezierVector.size(); i++)
    {
        float ii = static_cast<float>(i / static_cast<float>(bezierVector.size()));
        if (curveType == Type::Quadratic)
        {
            bezierVector[i].x = powf(ii, 2) * bezierPoints[0].x + (2 * ii * (1 - ii)) * bezierPoints[1].x + powf(1 - ii, 2) * bezierPoints[2].x;
            bezierVector[i].y = powf(ii, 2) * bezierPoints[0].y + (2 * ii * (1 - ii)) * bezierPoints[1].y + powf(1 - ii, 2) * bezierPoints[2].y;
        }
        if (curveType == Type::Cubic)
        {
            bezierVector[i].x = powf(ii, 3) * bezierPoints[0].x + (3 * powf(ii, 2) * (1 - ii)) * bezierPoints[1].x + ((3 * ii * powf(1 - ii, 2))) * bezierPoints[2].x + powf(1 - ii, 3) * bezierPoints[3].x;
            bezierVector[i].y = powf(ii, 3) * bezierPoints[0].y + (3 * powf(ii, 2) * (1 - ii)) * bezierPoints[1].y + ((3 * ii * powf(1 - ii, 2))) * bezierPoints[2].y + powf(1 - ii, 3) * bezierPoints[3].y;
        }
        if (curveType == Type::FourthOrder)
        {
            bezierVector[i].x = powf(ii, 4) * bezierPoints[0].x + (4 * powf(ii, 3) * (1 - ii)) * bezierPoints[1].x + ((6 * powf(ii, 2) * powf(1 - ii, 2))) * bezierPoints[2].x + (4 * ii * powf(1 - ii, 3)) * bezierPoints[3].x + powf(1 - ii, 4) * bezierPoints[4].x;
            bezierVector[i].y = powf(ii, 4) * bezierPoints[0].y + (4 * powf(ii, 3) * (1 - ii)) * bezierPoints[1].y + ((6 * powf(ii, 2) * powf(1 - ii, 2))) * bezierPoints[2].y + (4 * ii * powf(1 - ii, 3)) * bezierPoints[3].y + powf(1 - ii, 4) * bezierPoints[4].y;
        }
        if (curveType == Type::FifthOrder)
        {
            bezierVector[i].x = powf(ii, 5) * bezierPoints[0].x + (5 * powf(ii, 4) * (1 - ii)) * bezierPoints[1].x + (10 * powf(ii, 3) * powf(1 - ii, 2)) * bezierPoints[2].x + (10 * powf(ii, 2) * powf(1 - ii, 3)) * bezierPoints[3].x + (5 * powf(ii, 1) * powf(1 - ii, 4)) * bezierPoints[4].x + (powf(1 - ii, 5)) * bezierPoints[5].x;
            bezierVector[i].y = powf(ii, 5) * bezierPoints[0].y + (5 * powf(ii, 4) * (1 - ii)) * bezierPoints[1].y + (10 * powf(ii, 3) * powf(1 - ii, 2)) * bezierPoints[2].y + (10 * powf(ii, 2) * powf(1 - ii, 3)) * bezierPoints[3].y + (5 * powf(ii, 1) * powf(1 - ii, 4)) * bezierPoints[4].y + (powf(1 - ii, 5)) * bezierPoints[5].y;
        }
     
    }
}


void AuxPort::Bezier::setPoint(const juce::Point<int>& point, uint32_t index,const juce::Point<float> normalize)
{
    jassert(index < bezierPoints.size());
    juce::Point<float> p;
    p.x = static_cast<float>(point.x) / normalize.x;
    p.y = static_cast<float>(point.y) / normalize.y;
    bezierPoints[index] = p;
}

void AuxPort::Bezier::setPoint(const juce::Point<float>& point,uint32_t index)
{
    jassert(index < bezierPoints.size());
    bezierPoints[index] = point;
}



juce::Point<float>* AuxPort::Bezier::getPoint(const uint32_t& index,const PointType& pointType)
{
    if (pointType == PointType::Vector)
        return &bezierVector[index];
    else if (pointType == PointType::Parameter)
        return &bezierPoints[index];
    else if (pointType == PointType::WaveshapeVector)
        return &waveshaperVector[index];
}

uint32_t AuxPort::Bezier::getSize()
{
    return noOfPoints;
}

void AuxPort::Bezier::setSize(uint32_t size)
{
    noOfPoints = size;
    bezierVector.resize(noOfPoints);
    waveshaperVector.resize(2 * noOfPoints);
}

void AuxPort::Bezier::drawWaveshaper()
{
    for (uint32_t i = 0; i < bezierVector.size(); i++)
    {
        waveshaperVector[i] = phaseInvert * bezierVector[i];
    }
    uint32_t j = noOfPoints-1;
    for (uint32_t i = noOfPoints; i < waveshaperVector.size(); i++,j--)
    {
        waveshaperVector[i] = bezierVector[j];
    }
}

uint32_t AuxPort::Bezier::search(const float& sample)
{
    return auxSearch.search(waveshaperVector, sample, true);
}
