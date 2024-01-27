/*
  ==============================================================================

    AuxFilter.cpp
    Created: 11 Mar 2023 4:25:58pm
    Author:  satya

  ==============================================================================
*/

#include "AuxFilter.h"


namespace AuxPort
{
    void FIR::setImpulseResponse(const std::vector<float>& impulseResponse)
    {
        if (this->impulseResponse != impulseResponse)
            this->impulseResponse = impulseResponse;
    }
#ifdef JUCE_SHARED_CODE
    void FIR::process(juce::AudioBuffer<float>& buffer)
    {
        if (buffer.getNumChannels() != filterBuffer.size())
        {
            filterBuffer.resize(buffer.getNumChannels());
            outputBuffer.resize(buffer.getNumChannels());
        }


        if (filterBuffer[0].size() != impulseResponse.size())
            for (uint32_t i = 0; i < filterBuffer.size(); i++)
                filterBuffer[i].resize(impulseResponse.size());

        for (uint32_t i = 0; i < buffer.getNumSamples(); i++)
        {
            for (uint32_t j = 0; j < buffer.getNumChannels(); j++)
            {
                filterBuffer[j][writeIndex] = buffer.getSample(j, i);
                outputBuffer[j] = 0;
                readIndex = writeIndex;
                for (uint32_t k = 0; k < impulseResponse.size(); k++)
                {
                    outputBuffer[j] += filterBuffer[j][readIndex] * impulseResponse[k];
                    readIndex--;
                    if (readIndex < 0)
                        readIndex = impulseResponse.size() - 1;
                }
                buffer.setSample(j, i, outputBuffer[j]);
            }
            writeIndex++;
            writeIndex %= impulseResponse.size();
        }

    }
#endif
    float FIR::process(const float& inputSample)
    {
        if (sampleFilterBuffer.size() != impulseResponse.size())
            sampleFilterBuffer.resize(impulseResponse.size());
        sampleFilterBuffer[writeIndex] = inputSample;
        output = 0;
        readIndex = writeIndex;
        for (uint32_t k = 0; k < impulseResponse.size(); k++)
        {
            output += sampleFilterBuffer[readIndex] * impulseResponse[k];
            readIndex--;
            if (readIndex < 0)
                readIndex = impulseResponse.size() - 1;
        }
        writeIndex++;
        writeIndex %= impulseResponse.size();
        return output;
    }
    Hamming::Hamming(const Type& type)
    {
        this->type = type;
        Wc = 0;
        Wp = 0;
        Ws = 0;
        hammingWindow.resize(200);
        hLowpass.resize(200);
        hHighpass.resize(200);
        impulseResponse.resize(200);

    }

    void Hamming::compute(float passBand,float stopBand,uint32_t samplingRate)
    {
        Wp = (2 * AuxPI * passBand) / samplingRate;
        Ws = (2 * AuxPI * stopBand) / samplingRate;
        uint32_t M = (8 * AuxPI) / (Ws - Wp);
        if (M+1 >= hammingWindow.size())
        {
            hammingWindow.resize(M + 1);
            hLowpass.resize(M + 1);
            hHighpass.resize(M + 1);
            impulseResponse.resize(M + 1);
        }
        
        int32_t v = 0;
        for (uint32_t i = 0; i < M+1; i++)
        {
            v = i - (M / 2);
            hammingWindow[i] = 0.54 - 0.46 * cosf((2 * v * AuxPI) / M);

        }
            
        Wc = (Wp + Ws) / 2;
        for (uint32_t i = 0; i < M+1; i++)
        {
            v = i - (M / 2);
            hLowpass[i] = (Wc / AuxPI) * Utility::Sinc((Wc * v) / AuxPI);
        }
        
        if (type == AuxPort::Hamming::LowPass)
        {
            for (uint32_t i = 0; i < M+1; i++)
                impulseResponse[i] = hammingWindow[i] * hLowpass[i];
        }

        if (type == AuxPort::Hamming::HighPass)
        {
            for (uint32_t i = 0; i < M + 1; i++)
            {
                v = i - (M / 2);
                hHighpass[i] = Utility::Sinc(v) - hLowpass[i];
            }

            for (uint32_t i = 0; i < M + 1; i++)
                impulseResponse[i] = hammingWindow[i] * hHighpass[i];
        }
    }
    std::vector<float>* Hamming::getImpulseResponse()
    {
        return &impulseResponse;
    }
    std::vector<float> CDFilters::computeJ1(uint32_t order)
    {
        std::vector<float> ir;
        ir.resize(2 * order+1);
        int32_t n;
        for (int32_t i = 0; i < ir.size(); i++)
        {
            n = i - (ir.size() / 2.0f);
            ir[i] = 0.5 * AuxPort::Utility::Sinc(n) + 0.25 * AuxPort::Utility::Sinc(pow((n / 2), 2));
        }
        return ir;
    }

    std::vector<float> CDFilters::computeJ2(uint32_t order)
    {
        std::vector<float> ir;
        ir.resize(2 * order + 1);
        int32_t n;
        for (int32_t i = 0; i < ir.size(); i++)
        {
            n = i - (ir.size() / 2);
            ir[i] = 0.25 * n * AuxPort::Utility::Sinc(pow(n / 2, 2));
        }
        return ir;

    }

    
    void Derivative::reset()
    {
        prevSample = 0;
    }

}