#ifndef COLIN_GAINMETER_H
#define COLIN_GAINMETER_H
#include <JuceHeader.h>
#include <vector>

/*
  ==============================================================================

    GainMeter.h
    Created: 10 Nov 2022 10:30:49am
    Author:  Colin Raab
 
    very WIP effect, at proof of concept stage

  ==============================================================================
*/

namespace Colin {

class GainMeter {
protected:
    float magnitude;
    float RMS;
    float gain;
    float release;
    int imageIdx;
    int imageCache;
    int imagesMax;
    float imagesMaxInv;
    bool isReady;
    juce::Image img1;
    std::vector<juce::Image> images;
    std::vector<juce::Colour> colors;
    juce::ColourGradient gradient;
    
    float distortion = 0;
    
    int x, y, width, height;
    
public:
    GainMeter() {
        magnitude = 0;
        gain = 0;
        release = .2f;
        imageIdx = 0;
        imageCache = 0;
        imagesMax = 0;
        imagesMaxInv = 0.f;
        isReady = false;
    }
    
    ~GainMeter() = default;
    
    void setDistortion(float distortion) {
        this->distortion = distortion;
    }
    
    void setGradient(juce::ColourGradient gradient) {
        for(int i=0; i<colors.size(); i++) {
            double pos = double(i);
            pos /= colors.size();
            juce::Colour color = gradient.getColourAtPosition(pos);
            colors[i] = color;
        }
        this->gradient = gradient;
        isReady = true;
    }
    
    void setBuffer(juce::AudioBuffer<float>& buffer, int skipSamples = 0) {
        magnitude = 0.f;
        /// alternative code to get magnitude of the buffer
        skipSamples++;
        for (auto i = 0; i < buffer.getNumSamples(); i+=skipSamples)
        {
            auto sampleL = buffer.getReadPointer(0,i);
            auto sampleR = buffer.getReadPointer(1,i);
            float absSample = std::abs(*sampleL + *sampleR) / 2;
            if(magnitude < absSample) magnitude = absSample;
        }
        //magnitude = buffer.getMagnitude(0, 0, buffer.getNumSamples());
        if(gain < magnitude) gain = magnitude;
        else gain = gain + release * (magnitude - gain);
        if(magnitude < 0.05) imageIdx = 0;
        else imageIdx = int(gain*(imagesMax-1) + 1);
    }
    
    void setBufferRMS(juce::AudioBuffer<float>& buffer) {
        float newRMS = (buffer.getRMSLevel(0, 0, buffer.getNumSamples()) + buffer.getRMSLevel(1, 0, buffer.getNumSamples())) / 2;
        RMS = (newRMS + RMS) / 2;
    }
    
    void draw(juce::Graphics& g) {
        const int frames = img1.getHeight() / img1.getWidth() - 80;
        //const float dB = linearToDB(RMS);
        const int frameId = (int)ceil(RMS * ((double)frames - 1.0)) + 50;
        const float radius = juce::jmin(width / 2.0f, height / 2.0f);
        const float centerX = x + width * 0.5f;
        const float centerY = y + height * 0.5f;
        const float rx = centerX - radius - 1.0f;
        const float ry = centerY - radius;

        g.drawImage(img1,
            (int)rx,
            (int)ry,
            3 * (int)radius,
            2 * (int)radius,
            0,
            frameId*img1.getWidth(),
            img1.getWidth(),
            img1.getWidth());
    }
    
    bool shouldRepaint() {
        return true;
        /*
        if(imageCache == imageIdx) {
            return false;
        }
        else {
            imageCache = imageIdx;
            return true;
        }
         */
    }
    
    void setImages(int x, int y, int width, int height) {
        img1 = juce::ImageFileFormat::loadFrom(BinaryData::VU_png, BinaryData::VU_pngSize);
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        
        isReady = true;
        /*
        for(int i=0; i<images.size(); i++) {
            //auto iF = float(i);
            //auto iRel = iF * imagesMaxInv;
            
            
            for(int y=0; y<images[i].getHeight(); y++) {
                for(int x=0; x<images[i].getWidth(); x++) {
                    images[i] = image;
                    //images[i].setPixelAt(x, y, image.getPixelAt(x, y));
                }
            }
        }
         */
    }
};

class Bulb : public juce::Component
    {
    public:
        Bulb(const juce::Colour& c) : colour(c) {}

        void paint(juce::Graphics& g) override
        {
            const auto delta = 4.f;
            const auto bounds = getLocalBounds().toFloat().reduced(delta);
            //const auto side = juce::jmin(bounds.getWidth(), bounds.getHeight());
            const auto height = 10;
            const auto width = 8;
            const auto bulbFillBounds = juce::Rectangle<float>{ bounds.getX(), bounds.getY(), width, height };
            if (isOn)
                g.setColour(colour);
            else
                g.setColour(juce::Colours::black);
            g.fillRect(bulbFillBounds);
            g.setColour(juce::Colours::black);
            g.drawRect(bulbFillBounds, 1.f);
            if (isOn)
            {
                g.setGradientFill(
                        juce::ColourGradient{
                        colour.withAlpha(0.3f),
                        bulbFillBounds.getCentre(),
                        colour.withLightness(1.5f).withAlpha(0.f),
                        {},
                        true
                    });
                g.fillEllipse(bulbFillBounds.expanded(delta));
            }
        }
        void setState(const bool state) { isOn = state; }
        private:
            bool isOn = false;
            juce::Colour colour{};
        };

class VerticalDiscreteMeter : public juce::Component, public juce::Timer
    {
    public:
        VerticalDiscreteMeter(std::function<float()>&& valueFunction) : valueSupplier(std::move(valueFunction))
        {
            startTimerHz(24);
        }

        void paint(juce::Graphics& g) override
        {
            const auto level = juce::jmap(valueSupplier(), -60.f, 6.f, 0.f, 1.f);
            for (auto i = 0 ; i < totalNumberOfBulbs; i++)
            {
                if (level >= static_cast<float>(i + 1) / totalNumberOfBulbs)
                    bulbs[i]->setState(true);
                else
                    bulbs[i]->setState(false);
            }
        }

        void resized() override
        {
            const auto bounds = getLocalBounds().toFloat();
            gradient = juce::ColourGradient{
                    juce::Colours::green,
                    bounds.getBottomLeft(),
                    juce::Colours::red,
                    bounds.getTopLeft(),
                    false
            };
            gradient.addColour(0.5, juce::Colours::yellow);

            auto bulbBounds = getLocalBounds();
            const auto bulbWidth = bulbBounds.getWidth() / totalNumberOfBulbs;
            bulbs.clear();
            for (auto i = 0; i < totalNumberOfBulbs; i++)
            {
                auto bulb = std::make_unique<Bulb>(gradient.getColourAtPosition(static_cast<double>(i) / totalNumberOfBulbs));
                addAndMakeVisible(bulb.get());
                bulb->setBounds(bulbBounds.removeFromLeft(bulbWidth));
                bulbs.push_back(std::move(bulb));
            }
        }

        void timerCallback() override
        {
            repaint();
        }
    private:
        std::function<float()> valueSupplier;
        std::vector<std::unique_ptr<Bulb>> bulbs;
        juce::ColourGradient gradient{};
        const int totalNumberOfBulbs = 20;
    };

}

#endif
