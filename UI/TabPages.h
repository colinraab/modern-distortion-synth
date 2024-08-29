/*
  ==============================================================================

    TabPages.h
    Created: 19 Mar 2023 3:55:37pm
    Author:  Colin Raab
 
    Code for the five pages nested within the TabbedComponent
    Each page needs to initialize UI components, grab parameters, and map them to the components
    Fair amount of redudancy between each page declaration

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../TextSlider.h"
#include "../Synth/Sampler.h"
#include "../AuxShaper/AuxBezier.h"
#include "../AuxShaper/AuxShaper.h"

namespace Colin {

/// Custom slider component, adapted from remberg's juceCustomSliderSample (https://github.com/remberg/juceCustomSliderSample)
/// png files created using KnobMan (https://www.kvraudio.com/product/knobman-by-g200kg)

class customSlider : public juce::LookAndFeel_V4 {
private:
    juce::Image img1;
    
public:
    customSlider() {
        img1 = juce::ImageCache::getFromMemory(BinaryData::slider_png, BinaryData::slider_pngSize);
    }
    ~customSlider() = default;
    
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                          float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        if (img1.isValid())
        {
            const double rotation = (slider.getValue()
                - slider.getMinimum())
                / (slider.getMaximum()
                    - slider.getMinimum());

            const int frames = img1.getHeight() / img1.getWidth();
            const int frameId = (int)ceil(rotation * ((double)frames - 1.0));

            g.drawImage(img1, x, y, 15, 100, 51, frameId*120, 18, 120);
        }
        else
        {
            static const float textPpercent = 0.35f;
            juce::Rectangle<float> text_bounds(1.0f + width * (1.0f - textPpercent) / 2.0f,
                0.5f * height, width * textPpercent, 0.5f * height);

            g.setColour(juce::Colours::white);

            g.drawFittedText(juce::String("No Image"), text_bounds.getSmallestIntegerContainer(),
                juce::Justification::horizontallyCentred | juce::Justification::centred, 1);
        }
    }
};


struct Osc1Page : public juce::Component
{
    Osc1Page(Colin::Synth* o, juce::AudioParameterInt* distSel, juce::AudioParameterFloat* drive, juce::AudioParameterFloat* coeff, juce::AudioParameterFloat* fm, juce::AudioParameterFloat* pitch, juce::AudioParameterInt* waveSel, juce::AudioParameterInt* filter, juce::AudioParameterFloat* cutoff, juce::AudioParameterFloat* res, juce::AudioParameterBool* key, juce::AudioParameterFloat* ktA, juce::AudioParameterFloat* atk, juce::AudioParameterFloat* dec, juce::AudioParameterFloat* sus, juce::AudioParameterFloat* rel, juce::AudioParameterFloat* depth, juce::AudioParameterBool* etV, juce::AudioParameterBool* etD, juce::AudioParameterBool* etF, juce::AudioParameterFloat* wSlider, juce::AudioParameterFloat* dSlider, AuxPort::Bezier* bezier, juce::AudioParameterFloat* xP, juce::AudioParameterFloat* yP, juce::AudioParameterFloat* slopeP)
    {
        osc1 = o;
        
        juce::Typeface::Ptr tface = juce::Typeface::createSystemTypefaceFor(BinaryData::EHSMB_TTF, BinaryData::EHSMB_TTFSize);
        juce::Font led = juce::Font(tface);
        led.setHeight(15);
        
        addAndMakeVisible(name);
        name.setReadOnly(true);
        name.setInterceptsMouseClicks (false, false);
        name.setFont(led);
        name.setBounds(20, 10, 60, 40);
        name.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        name.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        name.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        juce::String text = "OSC 1";
        name.setText(text);
        
        addAndMakeVisible(name2);
        name2.setReadOnly(true);
        name2.setInterceptsMouseClicks (false, false);
        name2.setFont(led);
        name2.setBounds(152, 10, 60, 40);
        name2.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        name2.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        name2.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "DIST";
        name2.setText(text);
        
        addAndMakeVisible(name3);
        name3.setReadOnly(true);
        name3.setInterceptsMouseClicks (false, false);
        name3.setFont(led);
        name3.setBounds(285, 10, 100, 40);
        name3.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        name3.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        name3.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "FILT MOD";
        name3.setText(text);
        
        osc1Drive = new TextSlider(drive);
        osc1Drive->setRange(0, 99, 50, 1);
        osc1Drive->setBounds(160, 155, 30, 50);
        osc1Drive->setDistSlider(true);
        addAndMakeVisible(osc1Drive);
        
        osc1DCoeff = new TextSlider(coeff);
        osc1DCoeff->setRange(0, 99, 50, 1);
        osc1DCoeff->setBounds(200, 155, 30, 50);
        osc1DCoeff->setDistSlider(true);
        addAndMakeVisible(osc1DCoeff);
        
        osc1FM = new TextSlider(fm);
        osc1FM->setRange(0, 99, 0, 1);
        osc1FM->setBounds(70, 155, 30, 50);
        addAndMakeVisible(osc1FM);
        
        osc1Pitch = new TextSlider(pitch);
        osc1Pitch->setThreeChar(1);
        osc1Pitch->setRange(-24, 24, 0, 1);
        osc1Pitch->setBounds(25, 155, 50, 50);
        addAndMakeVisible(osc1Pitch);
        
        osc1Cutoff = new TextSlider(cutoff);
        osc1Cutoff->setThreeChar(2);
        osc1Cutoff->setRange(20, 20000, 20000, 1, 15);
        osc1Cutoff->setBounds(290, 40, 50, 50);
        addAndMakeVisible(osc1Cutoff);
        
        osc1Res = new TextSlider(res);
        osc1Res->setRange(0, 99, 10, 1);
        osc1Res->setBounds(330, 40, 30, 50);
        addAndMakeVisible(osc1Res);
        
        this->kt = key;
        keytrack.setToggleState(kt->get(), juce::dontSendNotification);
        addAndMakeVisible(keytrack);
        keytrack.setBounds(365, 37, 100, 30);
        keytrack.onClick = [this] {
            if(*kt == false) {
                *kt = true;
                keytrack.setToggleState(true, juce::NotificationType::dontSendNotification);
            }
            else {
                *kt = false;
                keytrack.setToggleState(false, juce::NotificationType::dontSendNotification);
            }
        };
        
        ktAmt = new TextSlider(ktA);
        ktAmt->setRange(0, 99, 99, 1);
        ktAmt->setBounds(435, 40, 30, 50);
        addAndMakeVisible(ktAmt);
        
        juce::Typeface::Ptr maintface = juce::Typeface::createSystemTypefaceFor(BinaryData::Avenir_Regular_ttf, BinaryData::Avenir_Regular_ttfSize);
        juce::Font mainFont = juce::Font(maintface);
        mainFont.setHeight(15);
        
        addAndMakeVisible(ADSR);
        ADSR.setReadOnly(true);
        ADSR.setInterceptsMouseClicks (false, false);
        ADSR.setFont(mainFont);
        ADSR.setBounds(295, 125, 155, 40);
        ADSR.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        ADSR.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        ADSR.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "A       D       S       R    Depth";
        ADSR.setText(text);
        
        addAndMakeVisible(infoText);
        infoText.setReadOnly(true);
        infoText.setInterceptsMouseClicks (false, false);
        infoText.setFont(mainFont);
        infoText.setBounds(25, 167, 250, 20);
        infoText.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        infoText.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        infoText.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "Pitch         FM from B                Drive      Coeff";
        infoText.setText(text);
        
        addAndMakeVisible(sliderText);
        sliderText.setReadOnly(true);
        sliderText.setInterceptsMouseClicks (false, false);
        sliderText.setFont(mainFont);
        sliderText.setBounds(126, 132, 250, 20);
        sliderText.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        sliderText.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        sliderText.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "dB                                            Wet";
        sliderText.setText(text);
        
        addAndMakeVisible(filterText);
        filterText.setReadOnly(true);
        filterText.setInterceptsMouseClicks (false, false);
        filterText.setFont(mainFont);
        filterText.setBounds(290, 56, 150, 20);
        filterText.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        filterText.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        filterText.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "Cutoff      Res";
        filterText.setText(text);
        
        osc1Attack = new TextSlider(atk);
        osc1Attack->setRange(0, 99, 10, 1);
        osc1Attack->setBounds(290, 110, 30, 50);
        addAndMakeVisible(osc1Attack);
        osc1Decay = new TextSlider(dec);
        osc1Decay->setRange(0, 99, 10, 1);
        osc1Decay->setBounds(315, 110, 30, 50);
        addAndMakeVisible(osc1Decay);
        osc1Sustain = new TextSlider(sus);
        osc1Sustain->setRange(0, 99, 80, 1);
        osc1Sustain->setBounds(340, 110, 30, 50);
        addAndMakeVisible(osc1Sustain);
        osc1Release = new TextSlider(rel);
        osc1Release->setRange(0, 99, 30, 1);
        osc1Release->setBounds(365, 110, 30, 50);
        addAndMakeVisible(osc1Release);
        ADSRDepth = new TextSlider(depth);
        ADSRDepth->setRange(0, 99, 0, 1);
        ADSRDepth->setBounds(390, 110, 30, 50);
        addAndMakeVisible(ADSRDepth);
        
        this->envV = etV;
        envToVol.setToggleState(envV->get(), juce::dontSendNotification);
        addAndMakeVisible(envToVol);
        envToVol.setBounds(440, 80, 100, 30);
        envToVol.onClick = [this] {
            if(*envV == false) {
                *envV = true;
                envToVol.setToggleState(true, juce::NotificationType::dontSendNotification);
            }
            else {
                *envV = false;
                envToVol.setToggleState(false, juce::NotificationType::dontSendNotification);
            }
        };
        
        this->envD = etD;
        envToDist.setToggleState(envD->get(), juce::dontSendNotification);
        addAndMakeVisible(envToDist);
        envToDist.setBounds(440, 100, 100, 30);
        envToDist.onClick = [this] {
            if(*envD == false) {
                *envD = true;
                envToDist.setToggleState(true, juce::NotificationType::dontSendNotification);
            }
            else {
                *envD = false;
                envToDist.setToggleState(false, juce::NotificationType::dontSendNotification);
            }
        };
        
        this->envF = etF;
        envToFilter.setToggleState(envF->get(), juce::dontSendNotification);
        addAndMakeVisible(envToFilter);
        envToFilter.setBounds(440, 120, 100, 30);
        envToFilter.onClick = [this] {
            if(*envF == false) {
                *envF = true;
                envToFilter.setToggleState(true, juce::NotificationType::dontSendNotification);
            }
            else {
                *envF = false;
                envToFilter.setToggleState(false, juce::NotificationType::dontSendNotification);
            }
        };
        
        dist = distSel;
        distortionChoice.addItem("None", 1);
        distortionChoice.addItem("Arbitrary", 2);
        distortionChoice.addItem("Exponential", 3);
        distortionChoice.addItem("Soft-Clip", 4);
        distortionChoice.addItem("Fuzz", 5);
        distortionChoice.addItem("Bitcrush", 6);
        distortionChoice.onChange = [&]() {
            *dist = distortionChoice.getSelectedId();
            if(distortionChoice.getSelectedId() == 2) auxShaper->setVisible(true);
            else auxShaper->setVisible(false);
            repaint();
        };
        distortionChoice.setSelectedId(*dist);
        distortionChoice.setBounds(157, 135, 100, 20);
        distortionChoice.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        addAndMakeVisible(distortionChoice);
        
        wave = waveSel;
        wavetableChoice.addSectionHeading("Basic");
        wavetableChoice.addItem("Sine", 1);
        wavetableChoice.addItem("Triangle", 2);
        wavetableChoice.addItem("Saw", 3);
        wavetableChoice.addItem("Square", 4);
        wavetableChoice.addSectionHeading("Serum");
        wavetableChoice.addItem("FourtyFour", 5);
        wavetableChoice.addItem("Echo", 6);
        wavetableChoice.addItem("Juliett", 7);
        wavetableChoice.addItem("Crayon", 8);
        wavetableChoice.addItem("Gritty", 9);
        wavetableChoice.onChange = [&]() {
            *wave = wavetableChoice.getSelectedId();
            repaint();
        };
        wavetableChoice.setSelectedId(*wave);
        wavetableChoice.setBounds(25, 135, 100, 20);
        wavetableChoice.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        addAndMakeVisible(wavetableChoice);
        
        filt = filter;
        filterChoice.addSectionHeading("TPT");
        filterChoice.addItem("LPF 12", 1);
        filterChoice.addItem("HPF 12", 2);
        filterChoice.addItem("BPF 12", 3);
        filterChoice.addSectionHeading("Ladder");
        filterChoice.addItem("LPF 12", 4);
        filterChoice.addItem("LPF 24", 5);
        filterChoice.addItem("HPF 12", 6);
        filterChoice.addItem("HPF 24", 7);
        filterChoice.setSelectedId(*filt);
        filterChoice.onChange = [this] {
            *filt = filterChoice.getSelectedId();
        };
        filterChoice.setBounds(290, 80, 80, 20);
        filterChoice.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        addAndMakeVisible(filterChoice);
        
        waveS = wSlider;
        waveSlider.setLookAndFeel(&cs);
        //waveSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 20, 10);
        waveSlider.setScrollWheelEnabled(true);
        waveSlider.setSliderStyle(juce::Slider::LinearVertical);
        waveSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 60, 20);
        waveSlider.setRange(waveS->range.start, waveS->range.end,1);
        waveSlider.setValue(*waveS);
        waveSlider.onValueChange = [this]
        {
            *waveS = waveSlider.getValue();
            waveSlider.setValue(*waveS);
        };
        waveSlider.setBounds(130, 25, 18, 120);
        addAndMakeVisible(waveSlider);
        
        distS = dSlider;
        distSlider.setLookAndFeel(&cs);
        distSlider.setScrollWheelEnabled(true);
        distSlider.setSliderStyle(juce::Slider::LinearVertical);
        distSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 60, 20);
        distSlider.setRange(distS->range.start, distS->range.end,1);
        distSlider.setValue(*distS);
        distSlider.onValueChange = [this]
        {
            *distS = distSlider.getValue();
            distSlider.setValue(*distS);
            repaint();
        };
        distSlider.setBounds(262, 25, 18, 120);
        addAndMakeVisible(distSlider);
        
        auxShaper = new AuxShaper(xP, yP, slopeP);
        auxShaper->setBounds(157, 32, 100, 100);
        addAndMakeVisible(auxShaper);
        
        updateParams();
    }
    
    void paint (juce::Graphics& g) override {
        g.setColour(juce::Colours::gold);
        g.setOpacity(0.3);
        g.fillRect(25, 32, 100, 100);
        g.fillRect(157, 32, 100, 100);
        if(draw) {
            juce::Path p;
            g.setColour(juce::Colours::lightblue);
            int start_x = 26;
            int start_y = 82;
            auto wavetable = osc1->getWavetable();
            p.startNewSubPath(start_x, start_y);
            auto ratio = osc1->WAVETABLE_LENGTH / 64;
            for (int sample = 0; sample < 64; sample++) {
                auto point = juce::jmap<float> (wavetable[sample*ratio], -1.f, 1.f, -50, 50);
                p.lineTo(start_x + sample * 1.56, start_y + point);
            }
            g.strokePath(p, juce::PathStrokeType(2));
            
            if(distortionChoice.getSelectedId() != 2) {
                juce::Path d;
                start_x = 157;
                start_y = 132;
                auto curve = osc1->dist.getCurve();
                p.startNewSubPath(start_x, start_y);
                for (int sample = 0; sample < 64; sample++) {
                    auto point = juce::jmap<float> (curve[sample], 0.f, 1.f, 0, 100);
                    p.lineTo(start_x + sample * 1.56, start_y - point);
                }
                g.strokePath(p, juce::PathStrokeType(2));
            }
            //draw = false;
        }
    }
    
    void updateParams() {
        osc1Drive->setValue();
        osc1DCoeff->setValue();
        osc1FM->setValue();
        osc1Pitch->setValue();
        osc1Cutoff->setValue();
        osc1Res->setValue();
        osc1Attack->setValue();
        osc1Decay->setValue();
        osc1Sustain->setValue();
        osc1Release->setValue();
        ADSRDepth->setValue();
        ktAmt->setValue();
        
        envToVol.setToggleState(*envV, juce::dontSendNotification);
        envToDist.setToggleState(*envD, juce::dontSendNotification);
        envToFilter.setToggleState(*envF, juce::dontSendNotification);
        keytrack.setToggleState(*kt, juce::dontSendNotification);
        distortionChoice.setSelectedId(*dist);
        wavetableChoice.setSelectedId(*wave);
        filterChoice.setSelectedId(*filt);
        waveSlider.setValue(*waveS);
        distSlider.setValue(*distS);
    }
    
    ~Osc1Page() {
        delete osc1Drive;
        delete osc1DCoeff;
        delete osc1FM;
        delete osc1Pitch;
        delete osc1Cutoff;
        delete osc1Res;
        delete osc1Attack;
        delete osc1Decay;
        delete osc1Sustain;
        delete osc1Release;
        delete ADSRDepth;
        delete ktAmt;
        delete auxShaper;
    }
    
private:
    Colin::Synth* osc1;
    juce::TextEditor name;
    juce::TextEditor name2;
    juce::TextEditor name3;
    juce::TextEditor ADSR;
    juce::TextEditor infoText;
    juce::TextEditor sliderText;
    juce::TextEditor filterText;
    customSlider cs;
    juce::Slider waveSlider;
    juce::Slider distSlider;
    juce::AudioParameterFloat * waveS;
    juce::AudioParameterFloat * distS;
    TextSlider* osc1Drive;
    TextSlider* osc1DCoeff;
    TextSlider* osc1FM;
    TextSlider* osc1Pitch;
    TextSlider* osc1Cutoff;
    TextSlider* osc1Res;
    TextSlider* osc1Attack;
    TextSlider* osc1Decay;
    TextSlider* osc1Sustain;
    TextSlider* osc1Release;
    TextSlider* ADSRDepth;
    TextSlider* ktAmt;
    juce::ToggleButton envToVol {"Vol"};
    juce::ToggleButton envToDist {"Dist"};
    juce::ToggleButton envToFilter {"Filt"};
    juce::ToggleButton keytrack {"Keytrack"};
    juce::ComboBox wavetableChoice;
    juce::ComboBox distortionChoice;
    juce::ComboBox filterChoice;
    juce::AudioParameterInt * filt;
    juce::AudioParameterInt * wave;
    juce::AudioParameterInt * dist;
    juce::AudioParameterBool * envV;
    juce::AudioParameterBool * envD;
    juce::AudioParameterBool * envF;
    juce::AudioParameterBool * kt;
    //std::vector<float> audioPoints;
    bool draw = true;
    AuxShaper* auxShaper;
};

struct Osc2Page : public juce::Component
{
    Osc2Page(Colin::Synth* o, juce::AudioParameterInt* distSel, juce::AudioParameterFloat* drive, juce::AudioParameterFloat* coeff, juce::AudioParameterFloat* fm, juce::AudioParameterFloat* pitch, juce::AudioParameterInt* waveSel, juce::AudioParameterInt* filter, juce::AudioParameterFloat* cutoff, juce::AudioParameterFloat* res, juce::AudioParameterBool* key, juce::AudioParameterFloat* ktA, juce::AudioParameterFloat* atk, juce::AudioParameterFloat* dec, juce::AudioParameterFloat* sus, juce::AudioParameterFloat* rel, juce::AudioParameterFloat* depth, juce::AudioParameterBool* etV, juce::AudioParameterBool* etD, juce::AudioParameterBool* etF, juce::AudioParameterFloat* wSlider, juce::AudioParameterFloat* dSlider, AuxPort::Bezier* bezier, juce::AudioParameterFloat* xP, juce::AudioParameterFloat* yP, juce::AudioParameterFloat* slopeP)
    {
        osc2 = o;
        
        juce::Typeface::Ptr tface = juce::Typeface::createSystemTypefaceFor(BinaryData::EHSMB_TTF, BinaryData::EHSMB_TTFSize);
        juce::Font led = juce::Font(tface);
        led.setHeight(15);
        
        addAndMakeVisible(name);
        name.setReadOnly(true);
        name.setInterceptsMouseClicks (false, false);
        name.setFont(led);
        name.setBounds(20, 10, 60, 40);
        name.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        name.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        name.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        juce::String text = "OSC 2";
        name.setText(text);
        
        addAndMakeVisible(name2);
        name2.setReadOnly(true);
        name2.setInterceptsMouseClicks (false, false);
        name2.setFont(led);
        name2.setBounds(152, 10, 60, 40);
        name2.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        name2.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        name2.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "DIST";
        name2.setText(text);
        
        addAndMakeVisible(name3);
        name3.setReadOnly(true);
        name3.setInterceptsMouseClicks (false, false);
        name3.setFont(led);
        name3.setBounds(285, 10, 100, 40);
        name3.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        name3.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        name3.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "FILT MOD";
        name3.setText(text);
        
        osc2Drive = new TextSlider(drive);
        osc2Drive->setRange(0, 99, 50, 1);
        osc2Drive->setBounds(160, 155, 30, 50);
        osc2Drive->setDistSlider(true);
        addAndMakeVisible(osc2Drive);
        
        osc2DCoeff = new TextSlider(coeff);
        osc2DCoeff->setRange(0, 99, 50, 1);
        osc2DCoeff->setBounds(200, 155, 30, 50);
        osc2DCoeff->setDistSlider(true);
        addAndMakeVisible(osc2DCoeff);
        
        osc2FM = new TextSlider(fm);
        osc2FM->setRange(0, 99, 0, 1);
        osc2FM->setBounds(70, 155, 30, 50);
        addAndMakeVisible(osc2FM);
        
        osc2Pitch = new TextSlider(pitch);
        osc2Pitch->setThreeChar(1);
        osc2Pitch->setRange(-24, 24, 0, 1);
        osc2Pitch->setBounds(25, 155, 50, 50);
        addAndMakeVisible(osc2Pitch);
        
        osc2Cutoff = new TextSlider(cutoff);
        osc2Cutoff->setThreeChar(2);
        osc2Cutoff->setRange(20, 20000, 20000, 1, 15);
        osc2Cutoff->setBounds(290, 40, 50, 50);
        addAndMakeVisible(osc2Cutoff);
        
        osc2Res = new TextSlider(res);
        osc2Res->setRange(0, 99, 10, 1);
        osc2Res->setBounds(330, 40, 30, 50);
        addAndMakeVisible(osc2Res);
        
        this->kt = key;
        keytrack.setToggleState(kt->get(), juce::dontSendNotification);
        addAndMakeVisible(keytrack);
        keytrack.setBounds(365, 37, 100, 30);
        keytrack.onClick = [this] {
            if(*kt == false) {
                *kt = true;
                keytrack.setToggleState(true, juce::NotificationType::dontSendNotification);
            }
            else {
                *kt = false;
                keytrack.setToggleState(false, juce::NotificationType::dontSendNotification);
            }
        };
        
        ktAmt = new TextSlider(ktA);
        ktAmt->setRange(0, 99, 99, 1);
        ktAmt->setBounds(435, 40, 30, 50);
        addAndMakeVisible(ktAmt);
        
        juce::Typeface::Ptr maintface = juce::Typeface::createSystemTypefaceFor(BinaryData::Avenir_Regular_ttf, BinaryData::Avenir_Regular_ttfSize);
        juce::Font mainFont = juce::Font(maintface);
        mainFont.setHeight(15);
        
        addAndMakeVisible(ADSR);
        ADSR.setReadOnly(true);
        ADSR.setInterceptsMouseClicks (false, false);
        ADSR.setFont(mainFont);
        ADSR.setBounds(295, 125, 155, 40);
        ADSR.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        ADSR.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        ADSR.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "A       D       S       R    Depth";
        ADSR.setText(text);
        
        addAndMakeVisible(infoText);
        infoText.setReadOnly(true);
        infoText.setInterceptsMouseClicks (false, false);
        infoText.setFont(mainFont);
        infoText.setBounds(25, 167, 250, 20);
        infoText.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        infoText.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        infoText.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "Pitch         FM from N               Drive      Coeff";
        infoText.setText(text);
        
        addAndMakeVisible(sliderText);
        sliderText.setReadOnly(true);
        sliderText.setInterceptsMouseClicks (false, false);
        sliderText.setFont(mainFont);
        sliderText.setBounds(126, 132, 250, 20);
        sliderText.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        sliderText.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        sliderText.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "dB                                            Wet";
        sliderText.setText(text);
        
        addAndMakeVisible(filterText);
        filterText.setReadOnly(true);
        filterText.setInterceptsMouseClicks (false, false);
        filterText.setFont(mainFont);
        filterText.setBounds(290, 56, 150, 20);
        filterText.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        filterText.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        filterText.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "Cutoff      Res";
        filterText.setText(text);
        
        osc2Attack = new TextSlider(atk);
        osc2Attack->setRange(0, 99, 10, 1);
        osc2Attack->setBounds(290, 110, 30, 50);
        addAndMakeVisible(osc2Attack);
        osc2Decay = new TextSlider(dec);
        osc2Decay->setRange(0, 99, 10, 1);
        osc2Decay->setBounds(315, 110, 30, 50);
        addAndMakeVisible(osc2Decay);
        osc2Sustain = new TextSlider(sus);
        osc2Sustain->setRange(0, 99, 80, 1);
        osc2Sustain->setBounds(340, 110, 30, 50);
        addAndMakeVisible(osc2Sustain);
        osc2Release = new TextSlider(rel);
        osc2Release->setRange(0, 99, 30, 1);
        osc2Release->setBounds(365, 110, 30, 50);
        addAndMakeVisible(osc2Release);
        ADSRDepth = new TextSlider(depth);
        ADSRDepth->setRange(0, 99, 0, 1);
        ADSRDepth->setBounds(390, 110, 30, 50);
        addAndMakeVisible(ADSRDepth);
        
        this->envV = etV;
        envToVol.setToggleState(envV->get(), juce::dontSendNotification);
        addAndMakeVisible(envToVol);
        envToVol.setBounds(440, 80, 100, 30);
        envToVol.onClick = [this] {
            if(*envV == false) {
                *envV = true;
                envToVol.setToggleState(true, juce::NotificationType::dontSendNotification);
            }
            else {
                *envV = false;
                envToVol.setToggleState(false, juce::NotificationType::dontSendNotification);
            }
        };
        
        this->envD = etD;
        envToDist.setToggleState(envD->get(), juce::dontSendNotification);
        addAndMakeVisible(envToDist);
        envToDist.setBounds(440, 100, 100, 30);
        envToDist.onClick = [this] {
            if(*envD == false) {
                *envD = true;
                envToDist.setToggleState(true, juce::NotificationType::dontSendNotification);
            }
            else {
                *envD = false;
                envToDist.setToggleState(false, juce::NotificationType::dontSendNotification);
            }
        };
        
        this->envF = etF;
        envToFilter.setToggleState(envF->get(), juce::dontSendNotification);
        addAndMakeVisible(envToFilter);
        envToFilter.setBounds(440, 120, 100, 30);
        envToFilter.onClick = [this] {
            if(*envF == false) {
                *envF = true;
                envToFilter.setToggleState(true, juce::NotificationType::dontSendNotification);
            }
            else {
                *envF = false;
                envToFilter.setToggleState(false, juce::NotificationType::dontSendNotification);
            }
        };
        
        dist = distSel;
        distortionChoice.addItem("None", 1);
        distortionChoice.addItem("Arbitrary", 2);
        distortionChoice.addItem("Exponential", 3);
        distortionChoice.addItem("Soft-Clip", 4);
        distortionChoice.addItem("Fuzz", 5);
        distortionChoice.addItem("Bitcrush", 6);
        distortionChoice.onChange = [&]() {
            *dist = distortionChoice.getSelectedId();
            if(distortionChoice.getSelectedId() == 2) auxShaper->setVisible(true);
            else auxShaper->setVisible(false);
            repaint();
        };
        distortionChoice.setSelectedId(*dist);
        distortionChoice.setBounds(157, 135, 100, 20);
        distortionChoice.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        addAndMakeVisible(distortionChoice);
        
        wave = waveSel;
        wavetableChoice.addSectionHeading("Basic");
        wavetableChoice.addItem("Sine", 1);
        wavetableChoice.addItem("Triangle", 2);
        wavetableChoice.addItem("Saw", 3);
        wavetableChoice.addItem("Square", 4);
        wavetableChoice.addSectionHeading("Serum");
        wavetableChoice.addItem("FourtyFour", 5);
        wavetableChoice.addItem("Echo", 6);
        wavetableChoice.addItem("Juliett", 7);
        wavetableChoice.addItem("Crayon", 8);
        wavetableChoice.addItem("Gritty", 9);
        wavetableChoice.onChange = [&]() {
            *wave = wavetableChoice.getSelectedId();
            repaint();
        };
        wavetableChoice.setSelectedId(*wave);
        wavetableChoice.setBounds(25, 135, 100, 20);
        wavetableChoice.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        addAndMakeVisible(wavetableChoice);
        
        filt = filter;
        filterChoice.addSectionHeading("TPT");
        filterChoice.addItem("LPF 12", 1);
        filterChoice.addItem("HPF 12", 2);
        filterChoice.addItem("BPF 12", 3);
        filterChoice.addSectionHeading("Ladder");
        filterChoice.addItem("LPF 12", 4);
        filterChoice.addItem("LPF 24", 5);
        filterChoice.addItem("HPF 12", 6);
        filterChoice.addItem("HPF 24", 7);
        filterChoice.setSelectedId(*filt);
        filterChoice.onChange = [this] {
            *filt = filterChoice.getSelectedId();
        };
        filterChoice.setBounds(290, 80, 80, 20);
        filterChoice.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        addAndMakeVisible(filterChoice);
        
        waveS = wSlider;
        waveSlider.setLookAndFeel(&cs);
        waveSlider.setScrollWheelEnabled(true);
        waveSlider.setSliderStyle(juce::Slider::LinearVertical);
        waveSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 60, 20);
        waveSlider.setRange(waveS->range.start, waveS->range.end,1);
        waveSlider.setValue(*waveS);
        waveSlider.onValueChange = [this]
        {
            *waveS = waveSlider.getValue();
            waveSlider.setValue(*waveS);
        };
        waveSlider.setBounds(130, 25, 18, 120);
        addAndMakeVisible(waveSlider);
        
        distS = dSlider;
        distSlider.setLookAndFeel(&cs);
        distSlider.setScrollWheelEnabled(true);
        distSlider.setSliderStyle(juce::Slider::LinearVertical);
        distSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 60, 20);
        distSlider.setRange(distS->range.start, distS->range.end,1);
        distSlider.setValue(*distS);
        distSlider.onValueChange = [this]
        {
            *distS = distSlider.getValue();
            distSlider.setValue(*distS);
            repaint();
        };
        distSlider.setBounds(262, 25, 18, 120);
        addAndMakeVisible(distSlider);
        
        auxShaper = new AuxShaper(xP, yP, slopeP);
        auxShaper->setBounds(157, 32, 100, 100);
        addAndMakeVisible(auxShaper);
        
        updateParams();
    }
    
    void paint (juce::Graphics& g) override {
        g.setColour(juce::Colours::gold);
        g.setOpacity(0.3);
        g.fillRect(25, 32, 100, 100);
        g.fillRect(157, 32, 100, 100);
        if(draw) {
            juce::Path p;
            //audioPoints.clear();
            g.setColour(juce::Colours::lightblue);
            int start_x = 26;
            int start_y = 82;
            auto wavetable = osc2->getWavetable();
            p.startNewSubPath(start_x, start_y);
            auto ratio = osc2->WAVETABLE_LENGTH / 64;
            for (int sample = 0; sample < 64; sample++) {
                auto point = juce::jmap<float> (wavetable[sample*ratio], -1.f, 1.f, -50, 50);
                p.lineTo(start_x + sample * 1.56, start_y + point);
            }
            g.strokePath(p, juce::PathStrokeType(2));
            
            if(distortionChoice.getSelectedId() != 2) {
                juce::Path d;
                start_x = 157;
                start_y = 132;
                auto curve = osc2->dist.getCurve();
                p.startNewSubPath(start_x, start_y);
                for (int sample = 0; sample < 64; sample++) {
                    auto point = juce::jmap<float> (curve[sample], 0.f, 1.f, 0, 100);
                    p.lineTo(start_x + sample * 1.56, start_y - point);
                }
                g.strokePath(p, juce::PathStrokeType(2));
            }
            //draw = false;
        }
    }
    
    void updateParams() {
        osc2Drive->setValue();
        osc2DCoeff->setValue();
        osc2FM->setValue();
        osc2Pitch->setValue();
        osc2Cutoff->setValue();
        osc2Res->setValue();
        osc2Attack->setValue();
        osc2Decay->setValue();
        osc2Sustain->setValue();
        osc2Release->setValue();
        ADSRDepth->setValue();
        ktAmt->setValue();
        
        envToVol.setToggleState(*envV, juce::dontSendNotification);
        envToDist.setToggleState(*envD, juce::dontSendNotification);
        envToFilter.setToggleState(*envF, juce::dontSendNotification);
        keytrack.setToggleState(*kt, juce::dontSendNotification);
        distortionChoice.setSelectedId(*dist);
        wavetableChoice.setSelectedId(*wave);
        filterChoice.setSelectedId(*filt);
        waveSlider.setValue(*waveS);
        distSlider.setValue(*distS);
    }
    
    ~Osc2Page() {
        delete osc2Drive;
        delete osc2DCoeff;
        delete osc2FM;
        delete osc2Pitch;
        delete osc2Cutoff;
        delete osc2Res;
        delete osc2Attack;
        delete osc2Decay;
        delete osc2Sustain;
        delete osc2Release;
        delete ADSRDepth;
        delete ktAmt;
        delete auxShaper;
    }
    
private:
    Colin::Synth* osc2;
    juce::TextEditor name;
    juce::TextEditor name2;
    juce::TextEditor name3;
    juce::TextEditor ADSR;
    juce::TextEditor infoText;
    juce::TextEditor sliderText;
    juce::TextEditor filterText;
    customSlider cs;
    juce::Slider waveSlider;
    juce::Slider distSlider;
    juce::AudioParameterFloat * waveS;
    juce::AudioParameterFloat * distS;
    TextSlider* osc2Drive;
    TextSlider* osc2DCoeff;
    TextSlider* osc2FM;
    TextSlider* osc2Pitch;
    TextSlider* osc2Cutoff;
    TextSlider* osc2Res;
    TextSlider* osc2Attack;
    TextSlider* osc2Decay;
    TextSlider* osc2Sustain;
    TextSlider* osc2Release;
    TextSlider* ADSRDepth;
    TextSlider* ktAmt;
    juce::ToggleButton envToVol {"Vol"};
    juce::ToggleButton envToDist {"Dist"};
    juce::ToggleButton envToFilter {"Filt"};
    juce::ToggleButton keytrack {"Keytrack"};
    juce::ComboBox wavetableChoice;
    juce::ComboBox distortionChoice;
    juce::ComboBox filterChoice;
    juce::AudioParameterInt * filt;
    juce::AudioParameterInt * wave;
    juce::AudioParameterInt * dist;
    juce::AudioParameterBool * envV;
    juce::AudioParameterBool * envD;
    juce::AudioParameterBool * envF;
    juce::AudioParameterBool * kt;
    //std::vector<float> audioPoints;
    bool draw = true;
    AuxShaper* auxShaper;
};

struct NoisePage : public juce::Component
{
    NoisePage(Colin::Synth* n, juce::AudioParameterInt* distSel, juce::AudioParameterFloat* drive, juce::AudioParameterFloat* coeff, juce::AudioParameterInt* waveSel, juce::AudioParameterInt* filter, juce::AudioParameterFloat* cutoff, juce::AudioParameterFloat* res, juce::AudioParameterBool* key, juce::AudioParameterFloat* ktA, juce::AudioParameterFloat* atk, juce::AudioParameterFloat* dec, juce::AudioParameterFloat* sus, juce::AudioParameterFloat* rel, juce::AudioParameterFloat* depth, juce::AudioParameterBool* etV, juce::AudioParameterBool* etD, juce::AudioParameterBool* etF, juce::AudioParameterFloat* wSlider, juce::AudioParameterFloat* dSlider, AuxPort::Bezier* bezier, juce::AudioParameterFloat* xP, juce::AudioParameterFloat* yP, juce::AudioParameterFloat* slopeP)
    {
        noise = n;
        
        
        juce::Typeface::Ptr tface = juce::Typeface::createSystemTypefaceFor(BinaryData::EHSMB_TTF, BinaryData::EHSMB_TTFSize);
        juce::Font led = juce::Font(tface);
        led.setHeight(15);
        
        addAndMakeVisible(name);
        name.setReadOnly(true);
        name.setInterceptsMouseClicks (false, false);
        name.setFont(led);
        name.setBounds(20, 10, 60, 40);
        name.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        name.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        name.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        juce::String text = "NOISE";
        name.setText(text);
        
        addAndMakeVisible(name2);
        name2.setReadOnly(true);
        name2.setInterceptsMouseClicks (false, false);
        name2.setFont(led);
        name2.setBounds(152, 10, 60, 40);
        name2.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        name2.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        name2.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "DIST";
        name2.setText(text);
        
        addAndMakeVisible(name3);
        name3.setReadOnly(true);
        name3.setInterceptsMouseClicks (false, false);
        name3.setFont(led);
        name3.setBounds(285, 10, 100, 40);
        name3.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        name3.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        name3.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "FILT MOD";
        name3.setText(text);
        
        juce::Typeface::Ptr maintface = juce::Typeface::createSystemTypefaceFor(BinaryData::Avenir_Regular_ttf, BinaryData::Avenir_Regular_ttfSize);
        juce::Font mainFont = juce::Font(maintface);
        mainFont.setHeight(15);
        
        addAndMakeVisible(ADSR);
        ADSR.setReadOnly(true);
        ADSR.setInterceptsMouseClicks (false, false);
        ADSR.setFont(mainFont);
        ADSR.setBounds(295, 125, 155, 40);
        ADSR.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        ADSR.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        ADSR.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "A       D       S       R    Depth";
        ADSR.setText(text);
        
        addAndMakeVisible(infoText);
        infoText.setReadOnly(true);
        infoText.setInterceptsMouseClicks (false, false);
        infoText.setFont(mainFont);
        infoText.setBounds(25, 167, 250, 20);
        infoText.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        infoText.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        infoText.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "                                                  Drive      Coeff";
        infoText.setText(text);
        
        addAndMakeVisible(sliderText);
        sliderText.setReadOnly(true);
        sliderText.setInterceptsMouseClicks (false, false);
        sliderText.setFont(mainFont);
        sliderText.setBounds(126, 132, 250, 20);
        sliderText.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        sliderText.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        sliderText.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "dB                                            Wet";
        sliderText.setText(text);
        
        addAndMakeVisible(filterText);
        filterText.setReadOnly(true);
        filterText.setInterceptsMouseClicks (false, false);
        filterText.setFont(mainFont);
        filterText.setBounds(290, 56, 150, 20);
        filterText.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        filterText.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        filterText.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "Cutoff      Res";
        filterText.setText(text);
        
        noiseDrive = new TextSlider(drive);
        noiseDrive->setRange(0, 99, 50, 1);
        noiseDrive->setBounds(160, 155, 30, 50);
        noiseDrive->setValue();
        noiseDrive->setDistSlider(true);
        addAndMakeVisible(noiseDrive);
        
        noiseDCoeff = new TextSlider(coeff);
        noiseDCoeff->setRange(0, 99, 50, 1);
        noiseDCoeff->setBounds(200, 155, 30, 50);
        noiseDCoeff->setValue();
        noiseDCoeff->setDistSlider(true);
        addAndMakeVisible(noiseDCoeff);
        
        noiseCutoff = new TextSlider(cutoff);
        noiseCutoff->setThreeChar(2);
        noiseCutoff->setRange(0, 20000, 20000, 1, 15);
        noiseCutoff->setBounds(290, 40, 50, 50);
        noiseCutoff->setValue();
        addAndMakeVisible(noiseCutoff);
        
        noiseRes = new TextSlider(res);
        noiseRes->setRange(0, 99, 10, 1);
        noiseRes->setBounds(330, 40, 50, 50);
        noiseRes->setValue();
        addAndMakeVisible(noiseRes);
        
        this->kt = key;
        keytrack.setToggleState(kt->get(), juce::dontSendNotification);
        addAndMakeVisible(keytrack);
        keytrack.setBounds(365, 37, 100, 30);
        keytrack.onClick = [this] {
            if(*kt == false) {
                *kt = true;
                keytrack.setToggleState(true, juce::NotificationType::dontSendNotification);
            }
            else {
                *kt = false;
                keytrack.setToggleState(false, juce::NotificationType::dontSendNotification);
            }
        };
        
        ktAmt = new TextSlider(ktA);
        ktAmt->setRange(0, 99, 99, 1);
        ktAmt->setBounds(435, 40, 30, 50);
        addAndMakeVisible(ktAmt);
        
        led.setHeight(15);
        noiseAttack = new TextSlider(atk);
        noiseAttack->setRange(0, 99, 10, 1);
        noiseAttack->setBounds(290, 110, 30, 50);
        noiseAttack->setValue();
        addAndMakeVisible(noiseAttack);
        noiseDecay = new TextSlider(dec);
        noiseDecay->setRange(0, 99, 10, 1);
        noiseDecay->setBounds(315, 110, 30, 50);
        noiseDecay->setValue();
        addAndMakeVisible(noiseDecay);
        noiseSustain = new TextSlider(sus);
        noiseSustain->setRange(0, 99, 80, 1);
        noiseSustain->setBounds(340, 110, 30, 50);
        noiseSustain->setValue();
        addAndMakeVisible(noiseSustain);
        noiseRelease = new TextSlider(rel);
        noiseRelease->setRange(0, 99, 30, 1);
        noiseRelease->setBounds(365, 110, 30, 50);
        noiseRelease->setValue();
        addAndMakeVisible(noiseRelease);
        ADSRDepth = new TextSlider(depth);
        ADSRDepth->setRange(0, 99, 0, 1);
        ADSRDepth->setBounds(390, 110, 30, 50);
        ADSRDepth->setValue();
        addAndMakeVisible(ADSRDepth);
        
        this->envV = etV;
        envToVol.setToggleState(envV->get(), juce::dontSendNotification);
        addAndMakeVisible(envToVol);
        envToVol.setBounds(440, 80, 100, 30);
        envToVol.onClick = [this] {
            if(*envV == false) {
                *envV = true;
                envToVol.setToggleState(true, juce::NotificationType::dontSendNotification);
            }
            else {
                *envV = false;
                envToVol.setToggleState(false, juce::NotificationType::dontSendNotification);
            }
        };
        
        this->envD = etD;
        envToDist.setToggleState(envD->get(), juce::dontSendNotification);
        addAndMakeVisible(envToDist);
        envToDist.setBounds(440, 100, 100, 30);
        envToDist.onClick = [this] {
            if(*envD == false) {
                *envD = true;
                envToDist.setToggleState(true, juce::NotificationType::dontSendNotification);
            }
            else {
                *envD = false;
                envToDist.setToggleState(false, juce::NotificationType::dontSendNotification);
            }
        };
        
        this->envF = etF;
        envToFilter.setToggleState(envF->get(), juce::dontSendNotification);
        addAndMakeVisible(envToFilter);
        envToFilter.setBounds(440, 120, 100, 30);
        envToFilter.onClick = [this] {
            if(*envF == false) {
                *envF = true;
                envToFilter.setToggleState(true, juce::NotificationType::dontSendNotification);
            }
            else {
                *envF = false;
                envToFilter.setToggleState(false, juce::NotificationType::dontSendNotification);
            }
        };
        
        dist = distSel;
        distortionChoice.addItem("None", 1);
        //distortionChoice.addItem("Arbitrary", 2);
        distortionChoice.addItem("Exponential", 3);
        distortionChoice.addItem("Soft-Clip", 4);
        distortionChoice.addItem("Fuzz", 5);
        distortionChoice.addItem("Bitcrush", 6);
        distortionChoice.onChange = [&]() {
            *dist = distortionChoice.getSelectedId();
            if(distortionChoice.getSelectedId() == 2) auxShaper->setVisible(true);
            else auxShaper->setVisible(false);
            repaint(157, 32, 100, 100);
        };
        distortionChoice.setSelectedId(*dist);
        distortionChoice.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        distortionChoice.setBounds(157, 135, 100, 20);
        addAndMakeVisible(distortionChoice);
        
        wave = waveSel;
        wavetableChoice.addItem("Gauss", 1);
        wavetableChoice.addItem("Binary", 2);
        wavetableChoice.addItem("LP", 3);
        wavetableChoice.addItem("HP", 4);
        wavetableChoice.onChange = [&]() {
            *wave = wavetableChoice.getSelectedId();
            repaint();
        };
        wavetableChoice.setSelectedId(*wave);
        wavetableChoice.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        wavetableChoice.setBounds(25, 135, 100, 20);
        addAndMakeVisible(wavetableChoice);
        
        filt = filter;
        filterChoice.addSectionHeading("TPT");
        filterChoice.addItem("LPF 12", 1);
        filterChoice.addItem("HPF 12", 2);
        filterChoice.addItem("BPF 12", 3);
        filterChoice.addSectionHeading("Ladder");
        filterChoice.addItem("LPF 12", 4);
        filterChoice.addItem("LPF 24", 5);
        filterChoice.addItem("HPF 12", 6);
        filterChoice.addItem("HPF 24", 7);
        filterChoice.setSelectedId(*filt);
        filterChoice.onChange = [this] {
            *filt = filterChoice.getSelectedId();
        };
        filterChoice.setBounds(290, 80, 80, 20);
        filterChoice.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        addAndMakeVisible(filterChoice);
        
        waveS = wSlider;
        waveSlider.setLookAndFeel(&cs);
        waveSlider.setScrollWheelEnabled(true);
        waveSlider.setSliderStyle(juce::Slider::LinearVertical);
        waveSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 60, 20);
        waveSlider.setRange(waveS->range.start, waveS->range.end,1);
        waveSlider.setValue(*waveS);
        waveSlider.onValueChange = [this]
        {
            *waveS = waveSlider.getValue();
            waveSlider.setValue(*waveS);
        };
        waveSlider.setBounds(130, 25, 18, 120);
        addAndMakeVisible(waveSlider);
        
        distS = dSlider;
        distSlider.setLookAndFeel(&cs);
        distSlider.setScrollWheelEnabled(true);
        distSlider.setSliderStyle(juce::Slider::LinearVertical);
        distSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 60, 20);
        distSlider.setRange(distS->range.start, distS->range.end,1);
        distSlider.setValue(*distS);
        distSlider.onValueChange = [this]
        {
            *distS = distSlider.getValue();
            distSlider.setValue(*distS);
            repaint(157, 32, 100, 100);
        };
        distSlider.setBounds(262, 25, 18, 120);
        addAndMakeVisible(distSlider);
        
        auxShaper = new AuxShaper(xP, yP, slopeP);
        auxShaper->setBounds(157, 32, 100, 100);
        addAndMakeVisible(auxShaper);
        
        updateParams();
    }
    
    void paint (juce::Graphics& g) override {
        g.setColour(juce::Colours::gold);
        g.setOpacity(0.3);
        g.fillRect(25, 32, 100, 100);
        g.fillRect(157, 32, 100, 100);
        if(draw) {
            juce::Path p;
            //audioPoints.clear();
            g.setColour(juce::Colours::lightblue);
            int start_x = 26;
            int start_y = 82;
            auto wavetable = noise->getNoise();
            p.startNewSubPath(start_x, start_y);
            auto ratio = noise->WAVETABLE_LENGTH / 512;
            for (int sample = 0; sample < 64; sample++) {
                auto point = juce::jmap<float> (2*wavetable[sample*ratio], -1.f, 1.f, -50, 50);
                p.lineTo(start_x + sample * 1.56, start_y + point);
            }
            g.strokePath(p, juce::PathStrokeType(2));
            
            if(distortionChoice.getSelectedId() != 2) {
                juce::Path d;
                start_x = 157;
                start_y = 132;
                auto curve = noise->dist.getCurve();
                d.startNewSubPath(start_x, start_y);
                for (int sample = 0; sample < 64; sample++) {
                    auto point = juce::jmap<float> (curve[sample], 0.f, 1.f, 0, 100);
                    d.lineTo(start_x + sample * 1.56, start_y - point);
                }
                g.strokePath(d, juce::PathStrokeType(2));
            }
        }
    }
    
    void updateParams() {
        noiseDrive->setValue();
        noiseDCoeff->setValue();
        noiseCutoff->setValue();
        noiseRes->setValue();
        noiseAttack->setValue();
        noiseDecay->setValue();
        noiseSustain->setValue();
        noiseRelease->setValue();
        ADSRDepth->setValue();
        ktAmt->setValue();
        
        waveSlider.setValue(*waveS);
        distSlider.setValue(*distS);
        if(*dist != 2) distortionChoice.setSelectedId(*dist);
        else  distortionChoice.setSelectedId(0);
        keytrack.setToggleState(*kt, juce::dontSendNotification);
        envToVol.setToggleState(*envV, juce::dontSendNotification);
        envToFilter.setToggleState(*envF, juce::dontSendNotification);
        envToDist.setToggleState(*envD, juce::dontSendNotification);
        wavetableChoice.setSelectedId(*wave);
        filterChoice.setSelectedId(*filt);
        waveSlider.setValue(*waveS);
        distSlider.setValue(*distS);
    }
    
    ~NoisePage() {
        delete noiseDrive;
        delete noiseDCoeff;
        delete noiseCutoff;
        delete noiseRes;
        delete noiseAttack;
        delete noiseDecay;
        delete noiseSustain;
        delete noiseRelease;
        delete ADSRDepth;
        delete ktAmt;
        delete auxShaper;
    }
    
private:
    Colin::Synth* noise;
    juce::TextEditor name;
    juce::TextEditor name2;
    juce::TextEditor name3;
    juce::TextEditor ADSR;
    juce::TextEditor infoText;
    juce::TextEditor sliderText;
    juce::TextEditor filterText;
    customSlider cs;
    juce::Slider waveSlider;
    juce::Slider distSlider;
    juce::AudioParameterFloat * waveS;
    juce::AudioParameterFloat * distS;
    TextSlider* noiseDrive;
    TextSlider* noiseDCoeff;
    TextSlider* noiseCutoff;
    TextSlider* noiseRes;
    TextSlider* noiseAttack;
    TextSlider* noiseDecay;
    TextSlider* noiseSustain;
    TextSlider* noiseRelease;
    TextSlider* ADSRDepth;
    TextSlider* ktAmt;
    juce::ToggleButton envToVol {"Vol"};
    juce::ToggleButton envToDist {"Dist"};
    juce::ToggleButton envToFilter {"Filt"};
    juce::ToggleButton keytrack {"Keytrack"};
    juce::ComboBox distortionChoice;
    juce::ComboBox wavetableChoice;
    juce::ComboBox filterChoice;
    juce::AudioParameterInt * dist;
    juce::AudioParameterInt * filt;
    juce::AudioParameterInt * wave;
    juce::AudioParameterBool * envV;
    juce::AudioParameterBool * envD;
    juce::AudioParameterBool * envF;
    juce::AudioParameterBool * kt;
    bool draw = true;
    AuxShaper* auxShaper;
};

struct SamplerPage : public juce::Component
{
    SamplerPage(Colin::Sampler* s, juce::AudioParameterInt* distSel, juce::AudioParameterFloat* drive, juce::AudioParameterFloat* coeff, juce::AudioParameterInt* filter, juce::AudioParameterFloat* cutoff, juce::AudioParameterFloat* res, juce::AudioParameterBool* key, juce::AudioParameterFloat* ktA, juce::AudioParameterBool* loopEnabled, juce::AudioParameterFloat* pitch, juce::AudioParameterBool* re, juce::AudioParameterFloat* atk, juce::AudioParameterFloat* dec, juce::AudioParameterFloat* sus, juce::AudioParameterFloat* rel, juce::AudioParameterFloat* depth, juce::AudioParameterBool* etV, juce::AudioParameterBool* etD, juce::AudioParameterBool* etF, juce::AudioParameterFloat* wSlider, juce::AudioParameterFloat* dSlider, AuxPort::Bezier* bezier, juce::AudioParameterFloat* xP, juce::AudioParameterFloat* yP, juce::AudioParameterFloat* slopeP)
    {
        juce::Typeface::Ptr tface = juce::Typeface::createSystemTypefaceFor(BinaryData::EHSMB_TTF, BinaryData::EHSMB_TTFSize);
        juce::Font led = juce::Font(tface);
        led.setHeight(15);
        
        addAndMakeVisible(name);
        name.setReadOnly(true);
        name.setInterceptsMouseClicks (false, false);
        name.setFont(led);
        name.setBounds(20, 10, 60, 40);
        name.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        name.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        name.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        juce::String text = "SAMPL";
        name.setText(text);
        
        addAndMakeVisible(name2);
        name2.setReadOnly(true);
        name2.setInterceptsMouseClicks (false, false);
        name2.setFont(led);
        name2.setBounds(152, 10, 60, 40);
        name2.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        name2.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        name2.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "DIST";
        name2.setText(text);
        
        addAndMakeVisible(name3);
        name3.setReadOnly(true);
        name3.setInterceptsMouseClicks (false, false);
        name3.setFont(led);
        name3.setBounds(285, 10, 100, 40);
        name3.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        name3.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        name3.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "FILT MOD";
        name3.setText(text);
        
        samplerPitch = new TextSlider(pitch);
        samplerPitch->setThreeChar(1);
        samplerPitch->setRange(-24, 24, 0, 1);
        samplerPitch->setBounds(25, 155, 50, 50);
        samplerPitch->setValue();
        addAndMakeVisible(samplerPitch);
        
        samplerDrive = new TextSlider(drive);
        samplerDrive->setRange(0, 99, 50, 1);
        samplerDrive->setBounds(160, 155, 30, 50);
        samplerDrive->setValue();
        samplerDrive->setDistSlider(true);
        addAndMakeVisible(samplerDrive);
        
        samplerDCoeff = new TextSlider(coeff);
        samplerDCoeff->setRange(0, 99, 50, 1);
        samplerDCoeff->setBounds(200, 155, 30, 50);
        samplerDCoeff->setValue();
        samplerDCoeff->setDistSlider(true);
        addAndMakeVisible(samplerDCoeff);
        
        samplerCutoff = new TextSlider(cutoff);
        samplerCutoff->setThreeChar(2);
        samplerCutoff->setRange(20, 20000, 20000, 1, 15);
        samplerCutoff->setBounds(290, 40, 50, 50);
        samplerCutoff->setValue();
        addAndMakeVisible(samplerCutoff);
        
        samplerRes = new TextSlider(res);
        samplerRes->setRange(0, 99, 10, 1);
        samplerRes->setBounds(330, 40, 50, 50);
        samplerRes->setValue();
        addAndMakeVisible(samplerRes);
        
        this->kt = key;
        keytrack.setToggleState(kt->get(), juce::dontSendNotification);
        addAndMakeVisible(keytrack);
        keytrack.setBounds(365, 37, 100, 30);
        keytrack.onClick = [this] {
            if(*kt == false) {
                *kt = true;
                keytrack.setToggleState(true, juce::NotificationType::dontSendNotification);
            }
            else {
                *kt = false;
                keytrack.setToggleState(false, juce::NotificationType::dontSendNotification);
            }
        };
        
        ktAmt = new TextSlider(ktA);
        ktAmt->setRange(0, 99, 99, 1);
        ktAmt->setBounds(435, 40, 30, 50);
        addAndMakeVisible(ktAmt);
        
        sampler = s;
        juce::Rectangle<int> waveformArea(20, 20, 200, 200);
        loadSampleButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::transparentBlack);
        loadSampleButton.onClick = [&]() {
            sampler->loadFile();
            draw = true;
            repaint(waveformArea);
        };
        addAndMakeVisible(loadSampleButton);
        loadSampleButton.setBounds(25, 135, 50, 20);
        
        draw = sampler->isSampleLoaded();
        
        this->repitch = re;
        repitchButton.setToggleState(repitch->get(), juce::dontSendNotification);
        addAndMakeVisible(repitchButton);
        repitchButton.setBounds(75, 155, 100, 30);
        repitchButton.onClick = [this] {
            if(*repitch == false) {
                *repitch = true;
                repitchButton.setToggleState(true, juce::NotificationType::dontSendNotification);
            }
            else {
                *repitch = false;
                repitchButton.setToggleState(false, juce::NotificationType::dontSendNotification);
            }
        };
        
        juce::Typeface::Ptr maintface = juce::Typeface::createSystemTypefaceFor(BinaryData::Avenir_Regular_ttf, BinaryData::Avenir_Regular_ttfSize);
        juce::Font mainFont = juce::Font(maintface);
        mainFont.setHeight(15);
        
        addAndMakeVisible(ADSR);
        ADSR.setReadOnly(true);
        ADSR.setInterceptsMouseClicks (false, false);
        ADSR.setFont(mainFont);
        ADSR.setBounds(295, 125, 155, 40);
        ADSR.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        ADSR.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        ADSR.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "A       D       S       R    Depth";
        ADSR.setText(text);
        
        addAndMakeVisible(infoText);
        infoText.setReadOnly(true);
        infoText.setInterceptsMouseClicks (false, false);
        infoText.setFont(mainFont);
        infoText.setBounds(25, 167, 250, 20);
        infoText.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        infoText.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        infoText.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "Pitch                                          Drive      Coeff";
        infoText.setText(text);
        
        addAndMakeVisible(sliderText);
        sliderText.setReadOnly(true);
        sliderText.setInterceptsMouseClicks (false, false);
        sliderText.setFont(mainFont);
        sliderText.setBounds(126, 132, 250, 20);
        sliderText.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        sliderText.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        sliderText.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "L%                                            Wet";
        sliderText.setText(text);
        
        addAndMakeVisible(filterText);
        filterText.setReadOnly(true);
        filterText.setInterceptsMouseClicks (false, false);
        filterText.setFont(mainFont);
        filterText.setBounds(290, 56, 150, 20);
        filterText.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        filterText.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        filterText.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "Cutoff      Res";
        filterText.setText(text);
        
        led.setHeight(15);
        samplerAttack = new TextSlider(atk);
        samplerAttack->setRange(0, 99, 10, 1);
        samplerAttack->setBounds(290, 110, 30, 50);
        samplerAttack->setValue();
        addAndMakeVisible(samplerAttack);
        samplerDecay = new TextSlider(dec);
        samplerDecay->setRange(0, 99, 10, 1);
        samplerDecay->setBounds(315, 110, 30, 50);
        samplerDecay->setValue();
        addAndMakeVisible(samplerDecay);
        samplerSustain = new TextSlider(sus);
        samplerSustain->setRange(0, 99, 80, 1);
        samplerSustain->setBounds(340, 110, 30, 50);
        samplerSustain->setValue();
        addAndMakeVisible(samplerSustain);
        samplerRelease = new TextSlider(rel);
        samplerRelease->setRange(0, 99, 30, 1);
        samplerRelease->setBounds(365, 110, 30, 50);
        samplerRelease->setValue();
        addAndMakeVisible(samplerRelease);
        ADSRDepth = new TextSlider(depth);
        ADSRDepth->setRange(0, 99, 0, 1);
        ADSRDepth->setBounds(390, 110, 30, 50);
        ADSRDepth->setValue();
        addAndMakeVisible(ADSRDepth);
        
        this->envV = etV;
        envToVol.setToggleState(envV->get(), juce::dontSendNotification);
        addAndMakeVisible(envToVol);
        envToVol.setBounds(440, 80, 100, 30);
        envToVol.onClick = [this] {
            if(*envV == false) {
                *envV = true;
                envToVol.setToggleState(true, juce::NotificationType::dontSendNotification);
            }
            else {
                *envV = false;
                envToVol.setToggleState(false, juce::NotificationType::dontSendNotification);
            }
        };
        
        this->envD = etD;
        envToDist.setToggleState(envD->get(), juce::dontSendNotification);
        addAndMakeVisible(envToDist);
        envToDist.setBounds(440, 100, 100, 30);
        envToDist.onClick = [this] {
            if(*envD == false) {
                *envD = true;
                envToDist.setToggleState(true, juce::NotificationType::dontSendNotification);
            }
            else {
                *envD = false;
                envToDist.setToggleState(false, juce::NotificationType::dontSendNotification);
            }
        };
        
        this->envF = etF;
        envToFilter.setToggleState(envF->get(), juce::dontSendNotification);
        addAndMakeVisible(envToFilter);
        envToFilter.setBounds(440, 120, 100, 30);
        envToFilter.onClick = [this] {
            if(*envF == false) {
                *envF = true;
                envToFilter.setToggleState(true, juce::NotificationType::dontSendNotification);
            }
            else {
                *envF = false;
                envToFilter.setToggleState(false, juce::NotificationType::dontSendNotification);
            }
        };
        
        this->lp = loopEnabled;
        loop.setToggleState(lp->get(), juce::dontSendNotification);
        addAndMakeVisible(loop);
        loop.setBounds(75, 135, 100, 20);
        loop.onClick = [this] {
            if(*lp == false) {
                *lp = true;
                loop.setToggleState(true, juce::NotificationType::dontSendNotification);
            }
            else {
                *lp = false;
                loop.setToggleState(false, juce::NotificationType::dontSendNotification);
            }
        };
        
        dist = distSel;
        distortionChoice.addItem("None", 1);
        distortionChoice.addItem("Arbitrary", 2);
        distortionChoice.addItem("Exponential", 3);
        distortionChoice.addItem("Soft-Clip", 4);
        distortionChoice.addItem("Fuzz", 5);
        distortionChoice.addItem("Bitcrush", 6);
        distortionChoice.onChange = [&]() {
            *dist = distortionChoice.getSelectedId();
            if(distortionChoice.getSelectedId() == 2) auxShaper->setVisible(true);
            else auxShaper->setVisible(false);
            repaint();
        };
        distortionChoice.setSelectedId(*dist);
        distortionChoice.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        distortionChoice.setBounds(157, 135, 100, 20);
        addAndMakeVisible(distortionChoice);
        
        filt = filter;
        filterChoice.addSectionHeading("TPT");
        filterChoice.addItem("LPF 12", 1);
        filterChoice.addItem("HPF 12", 2);
        filterChoice.addItem("BPF 12", 3);
        filterChoice.addSectionHeading("Ladder");
        filterChoice.addItem("LPF 12", 4);
        filterChoice.addItem("LPF 24", 5);
        filterChoice.addItem("HPF 12", 6);
        filterChoice.addItem("HPF 24", 7);
        filterChoice.setSelectedId(*filt);
        filterChoice.onChange = [this] {
            *filt = filterChoice.getSelectedId();
        };
        filterChoice.setBounds(290, 80, 80, 20);
        filterChoice.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        addAndMakeVisible(filterChoice);
        
        waveS = wSlider;
        waveSlider.setLookAndFeel(&cs);
        waveSlider.setScrollWheelEnabled(true);
        waveSlider.setSliderStyle(juce::Slider::LinearVertical);
        waveSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 60, 20);
        waveSlider.setRange(waveS->range.start, waveS->range.end,1);
        waveSlider.setValue(*waveS);
        waveSlider.onValueChange = [this]
        {
            *waveS = waveSlider.getValue();
            waveSlider.setValue(*waveS);
            repaint();
        };
        waveSlider.setBounds(130, 25, 18, 120);
        addAndMakeVisible(waveSlider);
        
        distS = dSlider;
        distSlider.setLookAndFeel(&cs);
        distSlider.setScrollWheelEnabled(true);
        distSlider.setSliderStyle(juce::Slider::LinearVertical);
        distSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 60, 20);
        distSlider.setRange(distS->range.start, distS->range.end,1);
        distSlider.setValue(*distS);
        distSlider.onValueChange = [this]
        {
            *distS = distSlider.getValue();
            distSlider.setValue(*distS);
            repaint();
        };
        distSlider.setBounds(262, 25, 18, 120);
        addAndMakeVisible(distSlider);
        
        auxShaper = new AuxShaper(xP, yP, slopeP);
        auxShaper->setBounds(157, 32, 100, 100);
        addAndMakeVisible(auxShaper);
        
        updateParams();
    }
    
    void paint (juce::Graphics& g) override {
        g.setColour(juce::Colours::gold);
        g.setOpacity(0.3);
        g.fillRect(25, 32, 100, 100);
        g.fillRect(157, 32, 100, 100);
        int start_x = 157;
        int start_y = 132;
        g.setColour(juce::Colours::lightblue);
        if(distortionChoice.getSelectedId() != 2) {
            juce::Path d;
            auto curve = sampler->dist.getCurve();
            d.startNewSubPath(start_x, start_y);
            for (int sample = 0; sample < 64; sample++) {
                auto point = juce::jmap<float> (curve[sample], 0.f, 1.f, 0, 100);
                d.lineTo(start_x + sample * 1.56, start_y - point);
            }
            g.strokePath(d, juce::PathStrokeType(2));
        }
        
        if(draw) {
            juce::Path p;
            audioPoints.clear();
            start_x = 26;
            start_y = 82;
            auto waveform = sampler->getWaveform();
            auto ratio = sampler->sampleLength / 100;
            // x-axis scale
            for (int sample = 0; sample < sampler->sampleLength; sample+=ratio) {
                audioPoints.push_back(waveform.getReadPointer(0)[sample]);
            }
            p.startNewSubPath(start_x, start_y);
            
            // y-axis scale
            for (int sample = 0; sample < 100; sample++) {
                auto point = juce::jmap<float> (audioPoints[sample], -1.f, 1.f, -50, 50);
                p.lineTo(start_x + sample, start_y + point);
            }
            
            g.strokePath(p, juce::PathStrokeType(2));
        }
    }
    
    void updateParams() {
        samplerDrive->setValue();
        samplerDCoeff->setValue();
        samplerCutoff->setValue();
        samplerRes->setValue();
        samplerPitch->setValue();
        samplerAttack->setValue();
        samplerDecay->setValue();
        samplerSustain->setValue();
        samplerRelease->setValue();
        ADSRDepth->setValue();
        ktAmt->setValue();
        
        envToVol.setToggleState(*envV, juce::NotificationType::dontSendNotification);
        envToDist.setToggleState(*envD, juce::NotificationType::dontSendNotification);
        envToFilter.setToggleState(*envF, juce::NotificationType::dontSendNotification);
        keytrack.setToggleState(*kt, juce::NotificationType::dontSendNotification);
        loop.setToggleState(*lp, juce::NotificationType::dontSendNotification);
        
        waveSlider.setValue(*waveS);
        distSlider.setValue(*distS);
        distortionChoice.setSelectedId(*dist);
        filterChoice.setSelectedId(*filt);
    }
    
    ~SamplerPage() {
        delete samplerDrive;
        delete samplerDCoeff;
        delete samplerCutoff;
        delete samplerRes;
        delete samplerPitch;
        delete samplerAttack;
        delete samplerDecay;
        delete samplerSustain;
        delete samplerRelease;
        delete ADSRDepth;
        delete ktAmt;
        delete auxShaper;
    }
    
private:
    juce::TextEditor name;
    juce::TextEditor name2;
    juce::TextEditor name3;
    juce::TextEditor ADSR;
    juce::TextEditor infoText;
    juce::TextEditor sliderText;
    juce::TextEditor filterText;
    customSlider cs;
    juce::Slider waveSlider;
    juce::Slider distSlider;
    juce::AudioParameterFloat * waveS;
    juce::AudioParameterFloat * distS;
    TextSlider* samplerDrive;
    TextSlider* samplerDCoeff;
    TextSlider* samplerCutoff;
    TextSlider* samplerRes;
    TextSlider* samplerPitch;
    TextSlider* samplerAttack;
    TextSlider* samplerDecay;
    TextSlider* samplerSustain;
    TextSlider* samplerRelease;
    TextSlider* ADSRDepth;
    TextSlider* ktAmt;
    juce::ToggleButton envToVol {"Vol"};
    juce::ToggleButton envToDist {"Dist"};
    juce::ToggleButton envToFilter {"Filt"};
    juce::ToggleButton keytrack {"Keytrack"};
    juce::ToggleButton loop {"Loop"};
    juce::AudioParameterBool* repitch;
    juce::ComboBox distortionChoice;
    juce::ComboBox filterChoice;
    juce::AudioParameterInt* dist;
    juce::AudioParameterInt* filt;
    juce::AudioParameterBool * envV;
    juce::AudioParameterBool * envD;
    juce::AudioParameterBool * envF;
    juce::AudioParameterBool * kt;
    juce::AudioParameterBool * lp;
    Sampler* sampler;
    juce::TextButton loadSampleButton {"Load"};
    juce::ToggleButton repitchButton {"Repitch"};
    std::vector<float> audioPoints;
    bool draw = false;
    AuxShaper * auxShaper;
};

struct MainPage : public juce::Component
{
    MainPage(Colin::Distortion* distortion, juce::AudioParameterInt* distSel, juce::AudioParameterFloat* drive, juce::AudioParameterFloat* coeff, juce::AudioParameterInt * filter, juce::AudioParameterFloat* cutoff, juce::AudioParameterFloat* res, juce::AudioParameterFloat* atk, juce::AudioParameterFloat* dec, juce::AudioParameterFloat* sus, juce::AudioParameterFloat* rel, juce::AudioParameterFloat* cThresh, juce::AudioParameterFloat* cRatio, juce::AudioParameterFloat* cAtk, juce::AudioParameterFloat* cRel, juce::AudioParameterFloat* wSlider, juce::AudioParameterFloat* dSlider, Oscilloscope* osc, AuxPort::Bezier* bezier, juce::AudioParameterFloat* xP, juce::AudioParameterFloat* yP, juce::AudioParameterFloat* slopeP)
    {        
        juce::Typeface::Ptr tface = juce::Typeface::createSystemTypefaceFor(BinaryData::EHSMB_TTF, BinaryData::EHSMB_TTFSize);
        juce::Font led = juce::Font(tface);
        led.setHeight(15);
        
        addAndMakeVisible(name);
        name.setReadOnly(true);
        name.setInterceptsMouseClicks (false, false);
        name.setFont(led);
        name.setBounds(20, 10, 60, 40);
        name.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        name.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        name.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        juce::String text = "MAIN";
        name.setText(text);
        
        addAndMakeVisible(name2);
        name2.setReadOnly(true);
        name2.setInterceptsMouseClicks (false, false);
        name2.setFont(led);
        name2.setBounds(152, 10, 60, 40);
        name2.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        name2.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        name2.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "DIST";
        name2.setText(text);
        
        addAndMakeVisible(name3);
        name3.setReadOnly(true);
        name3.setInterceptsMouseClicks (false, false);
        name3.setFont(led);
        name3.setBounds(285, 10, 100, 40);
        name3.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        name3.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        name3.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "FILT MOD";
        name3.setText(text);
        
        mainDrive = new TextSlider(drive);
        mainDrive->setRange(0, 99, 50, 1);
        mainDrive->setBounds(160, 155, 30, 50);
        mainDrive->setValue();
        mainDrive->setDistSlider(true);
        addAndMakeVisible(mainDrive);
        
        mainDCoeff = new TextSlider(coeff);
        mainDCoeff->setRange(0, 99, 50, 1);
        mainDCoeff->setBounds(200, 155, 30, 50);
        mainDCoeff->setValue();
        mainDCoeff->setDistSlider(true);
        addAndMakeVisible(mainDCoeff);
        
        mainCutoff = new TextSlider(cutoff);
        mainCutoff->setThreeChar(2);
        mainCutoff->setRange(20, 20000, 20000, 1, 15);
        mainCutoff->setBounds(290, 40, 50, 50);
        mainCutoff->setValue();
        addAndMakeVisible(mainCutoff);
        
        mainRes = new TextSlider(res);
        mainRes->setRange(0, 99, 10, 1);
        mainRes->setBounds(330, 40, 50, 50);
        mainRes->setValue();
        addAndMakeVisible(mainRes);
        
        juce::Typeface::Ptr maintface = juce::Typeface::createSystemTypefaceFor(BinaryData::Avenir_Regular_ttf, BinaryData::Avenir_Regular_ttfSize);
        juce::Font mainFont = juce::Font(maintface);
        mainFont.setHeight(15);
        
        addAndMakeVisible(ADSR);
        ADSR.setReadOnly(true);
        ADSR.setInterceptsMouseClicks (false, false);
        ADSR.setFont(mainFont);
        ADSR.setBounds(295, 125, 155, 40);
        ADSR.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        ADSR.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        ADSR.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "A       D       S       R  ";
        ADSR.setText(text);
        
        addAndMakeVisible(compText);
        compText.setReadOnly(true);
        compText.setInterceptsMouseClicks(false, false);
        compText.setFont(mainFont);
        compText.setBounds(410, 45, 70, 20);
        compText.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        compText.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        compText.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "Compressor";
        compText.setText(text);
        
        addAndMakeVisible(infoText);
        infoText.setReadOnly(true);
        infoText.setInterceptsMouseClicks (false, false);
        infoText.setFont(mainFont);
        infoText.setBounds(25, 167, 250, 20);
        infoText.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        infoText.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        infoText.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "                                                  Drive      Coeff";
        infoText.setText(text);
        
        addAndMakeVisible(sliderText);
        sliderText.setReadOnly(true);
        sliderText.setInterceptsMouseClicks (false, false);
        sliderText.setFont(mainFont);
        sliderText.setBounds(126, 132, 250, 20);
        sliderText.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        sliderText.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        sliderText.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "+/-                                            Wet";
        sliderText.setText(text);
        
        addAndMakeVisible(filterText);
        filterText.setReadOnly(true);
        filterText.setInterceptsMouseClicks (false, false);
        filterText.setFont(mainFont);
        filterText.setBounds(290, 56, 150, 20);
        filterText.setColour(juce::TextEditor::ColourIds::textColourId, juce::Colours::gold);
        filterText.setColour(juce::TextEditor::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        filterText.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentBlack);
        text = "Cutoff      Res";
        filterText.setText(text);
        
        led.setHeight(15);
        mainAttack = new TextSlider(atk);
        mainAttack->setRange(0, 99, 10, 1);
        mainAttack->setBounds(290, 110, 30, 50);
        mainAttack->setValue();
        addAndMakeVisible(mainAttack);
        mainDecay = new TextSlider(dec);
        mainDecay->setRange(0, 99, 10, 1);
        mainDecay->setBounds(315, 110, 30, 50);
        mainDecay->setValue();
        addAndMakeVisible(mainDecay);
        mainSustain = new TextSlider(sus);
        mainSustain->setRange(0, 99, 80, 1);
        mainSustain->setBounds(340, 110, 30, 50);
        mainSustain->setValue();
        addAndMakeVisible(mainSustain);
        mainRelease = new TextSlider(rel);
        mainRelease->setRange(0, 99, 30, 1);
        mainRelease->setBounds(365, 110, 30, 50);
        mainRelease->setValue();
        addAndMakeVisible(mainRelease);
        
        compThresh = new TextSlider(cThresh);
        compThresh->setThreeChar(1);
        compThresh->setRange(-40, 0, -3, 1);
        compThresh->setBounds(430, 60, 50, 50);
        compThresh->setValue();
        addAndMakeVisible(compThresh);
        
        compRatio = new TextSlider(cRatio);
        compRatio->setThreeChar(4);
        compRatio->setRange(1, 9, 9, 1);
        compRatio->setBounds(430, 80, 50, 50);
        compRatio->setValue();
        addAndMakeVisible(compRatio);
        
        compAtk = new TextSlider(cAtk);
        compAtk->setThreeChar(3);
        compAtk->setRange(5, 200, 10, 5);
        compAtk->setBounds(430, 100, 50, 50);
        compAtk->setValue();
        addAndMakeVisible(compAtk);
        
        compRel = new TextSlider(cRel);
        compRel->setThreeChar(3);
        compRel->setRange(5, 200, 10, 5);
        compRel->setBounds(430, 120, 50, 50);
        compRel->setValue();
        addAndMakeVisible(compRel);
        
    distMain = distortion;
        dist = distSel;
        distortionChoice.addItem("None", 1);
        distortionChoice.addItem("Arbitrary", 2);
        distortionChoice.addItem("Exponential", 3);
        distortionChoice.addItem("Soft-Clip", 4);
        distortionChoice.addItem("Fuzz", 5);
        distortionChoice.addItem("Bitcrush", 6);
        distortionChoice.onChange = [&]() {
            *dist = distortionChoice.getSelectedId();
            if(distortionChoice.getSelectedId() == 2) auxShaper->setVisible(true);
            else auxShaper->setVisible(false);
            repaint();
        };
        distortionChoice.setSelectedId(*dist);
        distortionChoice.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        distortionChoice.setBounds(157, 135, 100, 20);
        addAndMakeVisible(distortionChoice);
        
        filt = filter;
        filterChoice.addSectionHeading("Ladder");
        filterChoice.addItem("LPF 12", 1);
        filterChoice.addItem("LPF 24", 2);
        filterChoice.addItem("HPF 12", 3);
        filterChoice.addItem("HPF 24", 4);
        filterChoice.setSelectedId(*filt);
        filterChoice.onChange = [this] {
            *filt = filterChoice.getSelectedId();
        };
        filterChoice.setBounds(290, 80, 80, 20);
        filterChoice.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::transparentBlack);
        addAndMakeVisible(filterChoice);
        
        
        waveS = wSlider;
        waveSlider.setLookAndFeel(&cs);
        waveSlider.setScrollWheelEnabled(true);
        waveSlider.setSliderStyle(juce::Slider::LinearVertical);
        waveSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 60, 20);
        waveSlider.setRange(waveS->range.start, waveS->range.end,1);
        waveSlider.setValue(*waveS);
        waveSlider.onValueChange = [this]
        {
            *waveS = waveSlider.getValue();
            waveSlider.setValue(*waveS);
        };
        waveSlider.setBounds(130, 25, 18, 120);
        addAndMakeVisible(waveSlider);
        
        distS = dSlider;
        distSlider.setLookAndFeel(&cs);
        distSlider.setScrollWheelEnabled(true);
        distSlider.setSliderStyle(juce::Slider::LinearVertical);
        distSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 60, 20);
        distSlider.setRange(distS->range.start, distS->range.end,1);
        distSlider.setValue(*distS);
        distSlider.onValueChange = [this]
        {
            *distS = distSlider.getValue();
            distSlider.setValue(*distS);
            repaint();
        };
        distSlider.setBounds(262, 25, 18, 120);
        addAndMakeVisible(distSlider);
        
        oscilloscope = osc;
        oscilloscope->setBounds(25, 32, 100, 100);
        addAndMakeVisible(oscilloscope);
        
        auxShaper = new AuxShaper(xP, yP, slopeP);
        auxShaper->setBounds(157, 32, 100, 100);
        addAndMakeVisible(auxShaper);
        
        updateParams();
    }
    
    void updateParams() {
        mainDrive->setValue();
        mainDCoeff->setValue();
        mainCutoff->setValue();
        mainRes->setValue();
        mainAttack->setValue();
        mainDecay->setValue();
        mainSustain->setValue();
        mainRelease->setValue();
        compThresh->setValue();
        compRatio->setValue();
        compAtk->setValue();
        compRel->setValue();
        
        waveSlider.setValue(*waveS);
        distSlider.setValue(*distS);
        distortionChoice.setSelectedId(*dist);
        filterChoice.setSelectedId(*filt);
    }
    
    ~MainPage() {
        delete mainDrive;
        delete mainDCoeff;
        delete mainCutoff;
        delete mainRes;
        delete mainAttack;
        delete mainDecay;
        delete mainSustain;
        delete mainRelease;
        delete compThresh;
        delete compRatio;
        delete compAtk;
        delete compRel;
        delete auxShaper;
    }
    
    void paint (juce::Graphics& g) override {
        g.setColour(juce::Colours::gold);
        g.setOpacity(0.3);
        g.fillRect(157, 32, 100, 100);
        if(distortionChoice.getSelectedId() != 2) {
            g.setColour(juce::Colours::lightblue);
            juce::Path d;
            int start_x = 157;
            int start_y = 132;
            auto curve = distMain->getCurve();
            d.startNewSubPath(start_x, start_y);
            for (int sample = 0; sample < 64; sample++) {
                auto point = juce::jmap<float> (curve[sample], 0.f, 1.f, 0, 100);
                d.lineTo(start_x + sample * 1.56, start_y - point);
            }
            g.strokePath(d, juce::PathStrokeType(2));
        }
    }
    
private:
    juce::TextEditor name;
    juce::TextEditor name2;
    juce::TextEditor name3;
    juce::TextEditor ADSR;
    juce::TextEditor infoText;
    juce::TextEditor compText;
    juce::TextEditor sliderText;
    juce::TextEditor filterText;
    customSlider cs;
    juce::Slider waveSlider;
    juce::Slider distSlider;
    juce::AudioParameterFloat * waveS;
    juce::AudioParameterFloat * distS;
    TextSlider* mainDrive;
    TextSlider* mainDCoeff;
    TextSlider* mainCutoff;
    TextSlider* mainRes;
    TextSlider* mainAttack;
    TextSlider* mainDecay;
    TextSlider* mainSustain;
    TextSlider* mainRelease;
    TextSlider* compThresh;
    TextSlider* compRatio;
    TextSlider* compAtk;
    TextSlider* compRel;
    juce::ComboBox distortionChoice;
    juce::ComboBox filterChoice;
    juce::AudioParameterInt * dist;
    juce::AudioParameterInt * filt;
    juce::AudioParameterInt * wave;
    Colin::Distortion * distMain;
    Colin::Oscilloscope * oscilloscope;
    
    AuxShaper * auxShaper;
};

}
        
        
