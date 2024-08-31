/*
  ==============================================================================

    TextSlider.cpp
    Created: 10 Mar 2023 5:44:27pm
    Author:  Colin Raab
 
    Custom component that functions like a slider but only displays numbers
    1, 2, 3, or 4 digit sliders can be made
    Drag up or down on the numbers to adjust the slider, double-click to reset
    Mostly designed for this particular use case, might make more broadly applicable in the future

  ==============================================================================
*/

#include "TextSlider.h"

void TextSlider::setRange(float start, float end, float current, float interval, float skew) {
    range.start = start;
    range.end = end;
    range.interval = interval;
    currentVal = current;
    defaultVal = current;
    range.skew = skew;
    updateText();
}

void TextSlider::mouseDown(const juce::MouseEvent& e) {
    startVal = currentVal;
    oldDiff = 0;
}

void TextSlider::mouseDrag(const juce::MouseEvent& e) {
    update(e);
    std::lock_guard<std::mutex> lock(sliderMutex);
    *param = (float)currentVal;
}

void TextSlider::mouseDoubleClick(const juce::MouseEvent& e) {
    currentVal = defaultVal;
    *param = currentVal;
    updateText();
    if(shouldRepaint) {
        auto* parent = juce::Component::getParentComponent();
        if(isDistSlider) parent->repaint(157, 32, 100, 100);
        else parent->repaint();
    }
}

void TextSlider::update(const juce::MouseEvent& e) {
    int difference = e.getDistanceFromDragStartY();
    if(range.skew != 1.f) {
        if(currentVal > 10000) {
            if(interval != 200) {
                oldDiff = difference;
                interval = 200;
                startVal = currentVal;
            }
        }
        else if(currentVal > 1000) {
            if(interval != 50) {
                oldDiff = difference;
                interval = 50;
                startVal = currentVal;
            }
        }
        else {
            if(interval != 5) {
                oldDiff = difference;
                interval = 5;
                startVal = currentVal;
            }
        }
        currentVal = startVal - (difference - oldDiff) * interval;
    }
    else {
        if(isPitchSlider) {
            auto dif = (difference / 8) * range.interval;
            currentVal = startVal - dif;
        }
        else currentVal = startVal - (difference / 2) * range.interval;
    }
    
    if(currentVal <= range.start) currentVal = range.start;
    else if(currentVal >= range.end) currentVal = range.end;
    updateText();
    if(shouldRepaint) {
        auto* parent = juce::Component::getParentComponent();
        if(isDistSlider) parent->repaint(157, 32, 100, 100);
        else parent->repaint();
        if(isMainSlider) {
            auto* top = parent->getParentComponent();
            top->repaint();
        }
    }
}

void TextSlider::updateText() {
    if(threechar == 1) {
        if(currentVal >= 0 && currentVal < 10) {
            juce::String text = "00" + std::to_string(currentVal);
            textBox.setText(text);
        }
        else if (currentVal >= 10) {
            juce::String text = "0" + std::to_string(currentVal);
            textBox.setText(text);
        }
        else if (currentVal > -10 && currentVal < 0) {
            juce::String text = std::to_string(currentVal * -1);
            text = "-0" + text;
            textBox.setText(text);
        }
        else {
            textBox.setText(std::to_string(currentVal));
        }
    }
    else if(threechar == 2) {
        if (currentVal < 100) {
            juce::String text = "0" + std::to_string(currentVal);
            textBox.setText(text);
        }
        else if (currentVal >= 10000) {
            int thousands = currentVal/1000;
            juce::String text = std::to_string(thousands) + "k";
            textBox.setText(text);
        }
        else if (currentVal >= 1000) {
            int thousands = currentVal/1000;
            int hundreds = (currentVal - thousands*1000) / 100;
            juce::String text = std::to_string(thousands) + "k" + std::to_string(hundreds);
            textBox.setText(text);
        }
        else textBox.setText(std::to_string(currentVal));
    }
    else if(threechar == 3) {
        if(currentVal >= 0 && currentVal < 10) {
            juce::String text = "00" + std::to_string(currentVal);
            textBox.setText(text);
        }
        else if(currentVal >= 10 && currentVal < 100) {
            juce::String text = "0" + std::to_string(currentVal);
            textBox.setText(text);
        }
        else textBox.setText(std::to_string(currentVal));
    }
    else if(threechar == 4) {
        juce::String text = std::to_string(currentVal) + ":1";
        textBox.setText(text);
    }
    else {
        if(currentVal >= 0 && currentVal < 10) {
            juce::String text = "0" + std::to_string(currentVal);
            textBox.setText(text);
        }
        else textBox.setText(std::to_string(currentVal));
    }
}

void TextSlider::setText(std::string text) {
    textBox.setText(text);
}

void TextSlider::setValue() {
    if(currentVal == std::round(*param)) return;
    currentVal = *param;
    updateText();
}

float TextSlider::getValue() {
    return currentVal;
}

void TextSlider::setThreeChar(int isThree) {
    threechar = isThree;
    updateText();
}

void TextSlider::paint(juce::Graphics& g) {
}
