#ifndef AUXPORT_PARAM_H
#define AUXPORT_PARAM_H


/*
*			AuxParam : Extension for JUCE
			"I tried to make it easier" - inpinseptipin
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

/// Thank you to Satyarth for the ParameterMap, an alternative to JUCE's AudioParameterTree
/// I adapted his code to better suit this particular project, in addition to including preset XML functionality


#include "JuceHeader.h"
#include <random>
#include <fstream>
#include <iostream>

namespace AuxPort
{
	
	class ParameterMap
	{
	public:
		ParameterMap(juce::AudioProcessor* audioProcessor)
		{
			this->audioProcessor = audioProcessor;
		}
		~ParameterMap() = default;
		ParameterMap(const ParameterMap& params) = default;
		enum type
		{
			aBool,aFloat,aInt
		};
/***********************************************************************************/
/*
	[Function] Use this to add a juce::AudioParameterBool pointer to the AuxParam Map
*/
/***********************************************************************************/
		void addParameter(juce::AudioParameterBool* boolParam)
		{
			for (uint32_t i = 0; i < parameterMap.size(); i++)
			{
				jassert(parameterMap[i].getParameterName() != boolParam->name);
				//DBG("Try using a Different Parameter Name");
			}
			boolParameters.push_back(boolParam);
            boolInit.push_back(*boolParam);
			parameterMap.push_back({ boolParameters.size() - 1,AuxPort::ParameterMap::type::aBool,boolParam->name });
		}
/***********************************************************************************/
/*
	[Function] Use this to add a juce::AudioParameterFloat pointer to the AuxParam Map
*/
/***********************************************************************************/
		void addParameter(juce::AudioParameterFloat* floatParam)
		{
			for (uint32_t i = 0; i < parameterMap.size(); i++)
			{
				jassert(parameterMap[i].getParameterName() != floatParam->name);
				//DBG("Try using a Different Parameter Name");
			}
            floatParameters.push_back(floatParam);
			floatInit.push_back(*floatParam);
			parameterMap.push_back({ floatParameters.size() - 1,AuxPort::ParameterMap::type::aFloat,floatParam->name });
		}

/***********************************************************************************/
/*
	[Function] Use this to add a juce::AudioParameterInt pointer to the AuxParam Map
*/
/***********************************************************************************/

		void addParameter(juce::AudioParameterInt* intParam)
		{
			for (uint32_t i = 0; i < parameterMap.size(); i++)
			{
				jassert(parameterMap[i].getParameterName() != intParam->name);
				//DBG("Try using a Different Parameter Name");
			}
            intParameters.push_back(intParam);
			intInit.push_back(*intParam);
			parameterMap.push_back({ intParameters.size() - 1,AuxPort::ParameterMap::type::aInt,intParam->name });
		}

/***********************************************************************************/
/*
	[Function] Pass the parameter name to this function, and get the juce::AudioParameterFloat pointer.
*/
/***********************************************************************************/
		juce::AudioParameterFloat* getFloatParameter(const juce::String& parameterName)
		{
			for (uint32_t i = 0; i < parameterMap.size(); i++)
			{
				AuxPort::ParameterMap::parameter* param = &parameterMap[i];
				if (param->getParameterType() == AuxPort::ParameterMap::aFloat)
					if (param->getParameterName() == parameterName)
						return floatParameters[param->getParameterPosition()];
			}
			jassert(true);
			DBG("There is no AudioProcessorFloat parameter with that name");
		}
/***********************************************************************************/
/*
	[Function] Pass the parameter name to this function, and get the juce::AudioParameterBool pointer.
*/
/***********************************************************************************/

		juce::AudioParameterBool* getBoolParameter(const juce::String& parameterName)
		{
			for (uint32_t i = 0; i < parameterMap.size(); i++)
			{
				AuxPort::ParameterMap::parameter* param = &parameterMap[i];
				if (param->getParameterType() == AuxPort::ParameterMap::aBool)
					if (param->getParameterName() == parameterName)
						return boolParameters[param->getParameterPosition()];
			}
			jassert(true);
			DBG("There is no AudioProcessorBool parameter with that name");
		}

/***********************************************************************************/
/*
	[Function] Pass the parameter name to this function, and get the juce::AudioParameterInt pointer.
*/
/***********************************************************************************/
		juce::AudioParameterInt* getIntParameter(const juce::String& parameterName)
		{
			for (uint32_t i = 0; i < parameterMap.size(); i++)
			{
				AuxPort::ParameterMap::parameter* param = &parameterMap[i];
				if (param->getParameterType() == AuxPort::ParameterMap::aInt)
					if (param->getParameterName() == parameterName)
						return intParameters[param->getParameterPosition()];
			}
			jassert(true);
			DBG("There is no AudioProcessorInt parameter with that Name");
		}
/***********************************************************************************/	
/*
	[Function] Call this function to save your parameters so that they can be seen by the DAW
*/
/***********************************************************************************/

		void save()
		{
			for (uint32_t i = 0; i < parameterMap.size(); i++)
			{
				AuxPort::ParameterMap::parameter* param = &parameterMap[i];
				if (param->getParameterType() == AuxPort::ParameterMap::aBool)
					audioProcessor->addParameter(boolParameters[param->getParameterPosition()]);
				if (param->getParameterType() == AuxPort::ParameterMap::aFloat)
					audioProcessor->addParameter(floatParameters[param->getParameterPosition()]);
				if (param->getParameterType() == AuxPort::ParameterMap::aInt)
					audioProcessor->addParameter(intParameters[param->getParameterPosition()]);
			}
		}
        
        void randomize(std::vector<juce::String> skip) {
            std::random_device rd;     // Only used once to initialise (seed) engine
            std::mt19937 rng(rd());    // Random-number engine used (Mersenne-Twister in this case)
            for (uint32_t i = 0; i < intParameters.size(); i++)
            {
                juce::AudioParameterInt* p = intParameters[i];
                for(int j=0; j<skip.size(); j++) {
                    if(p->getName(20) == skip[j]) continue;
                }
                auto range = p->getRange();
                int min = static_cast<int>(range.getStart());
                int max = static_cast<int>(range.getEnd());
                std::uniform_int_distribution<int> uni(min,max); // Guaranteed unbiased
                int ran = uni(rng);
                *p = ran;
            }
            for (uint32_t i = 0; i < floatParameters.size(); i++)
            {
                juce::AudioParameterFloat* p = floatParameters[i];
                bool skipThis = false;
                auto name = p->getName(20);
                for(int j=0; j<skip.size(); j++) {
                    if(p->getName(20) == skip[j])
                        skipThis = true;
                }
                if(!skipThis) {
                    auto range = p->getNormalisableRange();
                    float min = static_cast<float>(range.start);
                    float max = static_cast<float>(range.end);
                    float ran = min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max-min)));
                    *p = ran;
                }
            }
            for (uint32_t i = 0; i < boolParameters.size(); i++)
            {
                juce::AudioParameterBool* p = boolParameters[i];
                for(int j=0; j<skip.size(); j++) {
                    if(p->getName(20) == skip[j]) continue;
                }
                //auto gen = std::bind(std::uniform_int_distribution<>(0,1),std::default_random_engine());
                //bool b = gen();
                bool b = std::rand() % 2;
                *p = b;
            }
        }
        
        void writeParams(juce::MemoryBlock& destData) {
            auto outStream = juce::MemoryOutputStream(destData, true);
            for (uint32_t i = 0; i < intParameters.size(); i++)
            {
                auto* p = intParameters[i];
                outStream.writeInt(*p);
            }
            for (uint32_t i = 0; i < floatParameters.size(); i++)
            {
                auto* p = floatParameters[i];
                outStream.writeFloat(*p);
            }
            for (uint32_t i = 0; i < boolParameters.size(); i++)
            {
                auto* p = boolParameters[i];
                outStream.writeBool(*p);
            }
        }
        
        void readParams(const void* data, int sizeInBytes) {
            auto inStream = juce::MemoryInputStream(data, static_cast<size_t>(sizeInBytes), false);
            for (uint32_t i = 0; i < intParameters.size(); i++)
            {
                auto* p = intParameters[i];
                *p = inStream.readInt();
            }
            for (uint32_t i = 0; i < floatParameters.size(); i++)
            {
                auto* p = floatParameters[i];
                *p = inStream.readFloat();
            }
            for (uint32_t i = 0; i < boolParameters.size(); i++)
            {
                auto* p = boolParameters[i];
                *p = inStream.readBool();
            }
        }
        
        void init() {
            for (uint32_t i = 0; i < intParameters.size(); i++)
            {
                auto* p = intParameters[i];
                *p = intInit[i];
            }
            for (uint32_t i = 0; i < floatParameters.size(); i++)
            {
                auto* p = floatParameters[i];
                *p = floatInit[i];
            }
            for (uint32_t i = 0; i < boolParameters.size(); i++)
            {
                auto* p = boolParameters[i];
                *p = boolInit[i];
            }
        }
        
        juce::XmlElement createXML() {
            juce::XmlElement paramList ("PARAMETERS");
            for (uint32_t i = 0; i < intParameters.size(); i++)
            {
                auto* p = intParameters[i];
                juce::XmlElement* pElement = new juce::XmlElement ("int");
                pElement->setAttribute("name", p->getName(20));
                pElement->setAttribute("value", p->get());
                paramList.addChildElement(pElement);
            }
            for (uint32_t i = 0; i < floatParameters.size(); i++)
            {
                auto* p = floatParameters[i];
                juce::XmlElement* pElement = new juce::XmlElement ("float");
                pElement->setAttribute("name", p->getName(20));
                pElement->setAttribute("value", static_cast<double>(p->get()));
                paramList.addChildElement(pElement);
            }
            for (uint32_t i = 0; i < boolParameters.size(); i++)
            {
                auto* p = boolParameters[i];
                juce::XmlElement* pElement = new juce::XmlElement ("bool");
                pElement->setAttribute("name", p->getName(20));
                pElement->setAttribute("value", p->get());
                paramList.addChildElement(pElement);
            }
            
            //auto xmlString = paramList.toString();
            return paramList;
        }
        
        void loadXML(juce::XmlElement paramList) {
            std::lock_guard<std::mutex> lock(idk);
            
            if (paramList.hasTagName ("PARAMETERS"))
            {
                for (auto* e : paramList.getChildIterator())
                {
                    if (e->hasTagName ("int"))
                    {
                        juce::String name = e->getStringAttribute ("name");
                        int value = e->getIntAttribute ("value");
                        auto *p = getIntParameter(name);
                        *p = value;
                    }
                    else if (e->hasTagName ("float"))
                    {
                        juce::String name = e->getStringAttribute ("name");
                        float value = static_cast<float>(e->getDoubleAttribute ("value"));
                        auto *p = getFloatParameter(name);
                        *p = value;
                    }
                    else if (e->hasTagName ("bool"))
                    {
                        juce::String name = e->getStringAttribute ("name");
                        bool value = e->getBoolAttribute ("value");
                        auto *p = getBoolParameter(name);
                        *p = value;
                    }
                }
            }
        }
        
		
	private:
		class parameter
		{
		public:
			parameter() = default;
			~parameter() = default;
			parameter(const parameter& param) = default;
			parameter(const size_t& index, const AuxPort::ParameterMap::type& paramType,const juce::String& paramName)
			{
				p_pos = index;
				p_type = paramType;
				p_name = paramName;
			}
			juce::String getParameterName()
			{
				return p_name;
			}
			uint32_t getParameterPosition()
			{
				return p_pos;
			}
			AuxPort::ParameterMap::type getParameterType()
			{
				return p_type;
			}
		private:
			uint32_t p_pos;
			AuxPort::ParameterMap::type p_type;
			juce::String p_name;
		};
        
        class binary_parameter {
        public:
            binary_parameter(float f, bool b, int i, AuxPort::ParameterMap::type t, juce::String n) {
                if(t==AuxPort::ParameterMap::type::aInt) {
                    ival = i;
                }
                else if(t==AuxPort::ParameterMap::type::aBool) {
                    bval = b;
                }
                else if(t==AuxPort::ParameterMap::type::aFloat) {
                    fval = i;
                }
                type = t;
                name = n;
            }
        private:
            float fval;
            bool bval;
            int ival;
            AuxPort::ParameterMap::type type;
            juce::String name;
        };
        
        std::mutex idk;
        
		juce::AudioProcessor* audioProcessor;
		std::vector<juce::AudioParameterBool*> boolParameters;
		std::vector<juce::AudioParameterFloat*> floatParameters;
		std::vector<juce::AudioParameterInt*> intParameters;
		std::vector<AuxPort::ParameterMap::parameter> parameterMap;
        std::vector<bool> boolInit;
        std::vector<float> floatInit;
        std::vector<int> intInit;
	};
}
#endif


