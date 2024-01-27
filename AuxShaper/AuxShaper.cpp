/*
  ==============================================================================

    AuxShaper.cpp
    Created: 28 Mar 2023 5:13:37pm
    Author:  satya

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AuxShaper.h"


AuxShaper::AuxPoint::AuxPoint(juce::AudioParameterFloat* x,juce::AudioParameterFloat* y,juce::AudioParameterFloat* slope)
{
    this->x = x;
    this->y = y;
    this->slope = slope;
    this->pointColour = juce::Colours::gold;
    drawingBool = false;
    this->slopeFactor = 0;
}

AuxShaper::AuxPoint::~AuxPoint()
{
}

void AuxShaper::AuxPoint::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::transparentBlack);
    g.setColour(pointColour);
    g.drawText(this->text, this->getLocalBounds(), juce::Justification::horizontallyCentred, true);
    if (drawingBool)
    {
        //juce::Colour ellipseColour = pointColour.darker(0.8);
        g.setColour(juce::Colours::gold);
        g.strokePath(ellipsePath, juce::PathStrokeType(3));
    }
}

void AuxShaper::AuxPoint::resized()
{
    if (!drawingBool)
    {
        ellipsePath.addEllipse(this->getWidth() * 0.1, this->getHeight() * 0.1, this->getWidth() * 0.8, this->getHeight() * 0.8);
        drawingBool = true;
        origin = xyCoordinate;
        getParentComponent()->repaint();
    }
}

void AuxShaper::AuxPoint::setColour(const juce::Colour& colour)
{
    this->pointColour = colour;
}

juce::Colour* AuxShaper::AuxPoint::getColour()
{
    return &pointColour;
}

void AuxShaper::AuxPoint::mouseDrag(const juce::MouseEvent& e)
{
    auto deltaX = e.getDistanceFromDragStartX();
    auto deltaY = e.getDistanceFromDragStartY();

    deltaX += origin.x;
    deltaY += origin.y;

    xyCoordinate.x = deltaX;
    xyCoordinate.y = deltaY;

    negBand.x = xyCoordinate.x - (slopeFactor * xyCoordinate.x);
    negBand.y = xyCoordinate.y - (slopeFactor * xyCoordinate.y);

    posBand.x = xyCoordinate.x + (slopeFactor * xyCoordinate.x);
    posBand.y = xyCoordinate.y + (slopeFactor * xyCoordinate.y);



    limit(&xyCoordinate);
    limit(&negBand);
    limit(&posBand);


    getParentComponent()->resized();
    getParentComponent()->repaint();

    const std::lock_guard<std::mutex> lock(pointMutex);
    *x = static_cast<float>(xyCoordinate.x) / static_cast<float>(getParentComponent()->getWidth());
    *y = 1 - static_cast<float>(xyCoordinate.y) / static_cast<float>(getParentComponent()->getHeight());

}

void AuxShaper::AuxPoint::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
{
    auto deltaY = wheel.deltaY;
    if (deltaY > 0)
    {
        slopeFactor += abs(deltaY);
        if (slopeFactor > slope->range.end)
            slopeFactor = slope->range.end;
    }
    else
    {
        slopeFactor -= abs(deltaY);
        if (slopeFactor < slope->range.start)
            slopeFactor = slope->range.start;
    }
    negBand.x = xyCoordinate.x - (slopeFactor * xyCoordinate.x);
    negBand.y = xyCoordinate.y - (slopeFactor * xyCoordinate.y);

    posBand.x = xyCoordinate.x + (slopeFactor * xyCoordinate.x);
    posBand.y = xyCoordinate.y + (slopeFactor * xyCoordinate.y);
    this->getParentComponent()->repaint();
    const std::lock_guard<std::mutex> lock(pointMutex);
    *slope = slopeFactor;
}

void AuxShaper::AuxPoint::mouseUp(const juce::MouseEvent& e)
{
    const std::lock_guard<std::mutex> lock(pointMutex);
    origin = xyCoordinate;
    *x = static_cast<float>(xyCoordinate.x) / static_cast<float>(getParentComponent()->getWidth());
    *y = 1 - static_cast<float>(xyCoordinate.y) / static_cast<float>(getParentComponent()->getHeight());
    
}

void AuxShaper::AuxPoint::setPointBounds(const juce::Rectangle<int> pointBounds)
{
    this->pointBounds = pointBounds;
}

juce::Rectangle<int>* AuxShaper::AuxPoint::getPointBounds()
{
    return &pointBounds;
}

void AuxShaper::AuxPoint::setCoordinate(int x, int y)
{
    xyCoordinate.x = x;
    xyCoordinate.y = y;
    negBand = xyCoordinate;
    posBand = xyCoordinate;
}

juce::Point<int>* AuxShaper::AuxPoint::getCoordinate()
{
    return &xyCoordinate;
}

juce::Point<int>* AuxShaper::AuxPoint::getNegBand()
{
    return &negBand;
}

juce::Point<int>* AuxShaper::AuxPoint::getPosBand()
{
    return &posBand;
}

void AuxShaper::AuxPoint::setText(const juce::String& text)
{
    this->text = text;
}

void AuxShaper::AuxPoint::limit(juce::Point<int>* point)
{
    if (point->x > this->pointBounds.getWidth() + this->pointBounds.getX() - this->getLocalBounds().getWidth())
        point->x = this->pointBounds.getWidth() + this->pointBounds.getX() - this->getLocalBounds().getWidth();
    if (point->x < this->pointBounds.getX())
        point->x = this->pointBounds.getX();
    if (point->y > this->pointBounds.getHeight() + this->pointBounds.getY() - this->getLocalBounds().getHeight())
        point->y = this->pointBounds.getHeight() + this->pointBounds.getY() - this->getLocalBounds().getHeight();
    if (point->y < this->pointBounds.getY())
        point->y = this->pointBounds.getY();
}



//==============================================================================
AuxShaper::AuxShaper(AuxPort::ParameterMap* parameterMap) : p1(parameterMap->getFloatParameter("x1"),parameterMap->getFloatParameter("y1"),parameterMap->getFloatParameter("slope")), bezier(400, AuxPort::Bezier::FourthOrder)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    this->parameterMap = parameterMap;
    addAndMakeVisible(p1);
    p1.setColour(juce::Colours::pink);
    drawingBool = false;
}

AuxShaper::AuxShaper(juce::AudioParameterFloat* x, juce::AudioParameterFloat *y, juce::AudioParameterFloat* s) : p1(x,y,s), bezier(400, AuxPort::Bezier::FourthOrder)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    xP = x;
    yP = y;
    slopeP = s;
    addAndMakeVisible(p1);
    p1.setColour(juce::Colours::gold);
    drawingBool = false;
}

AuxShaper::~AuxShaper()
{
}

void AuxShaper::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */
    auto x = this->getLocalBounds().getWidth() / 10;
    auto y = this->getLocalBounds().getHeight() / 10;
    g.fillAll(juce::Colours::transparentBlack);

    g.setColour(juce::Colours::white);
    //g.drawLine(0, 5 * y, 10 * x, 5 * y);
    //g.drawLine(5 * x, 0, 5 * x, 10 * y);

    if (drawingBool)
    {
        bezier.setPoint(startPoint, 0, normalize);
        bezier.setPoint(*p1.getNegBand(), 1,normalize);
        bezier.setPoint(*p1.getCoordinate(), 2, normalize);
        bezier.setPoint(*p1.getPosBand(), 3, normalize);
        bezier.setPoint(endPoint, 4, normalize);

        bezier.calcPoints();
        //g.setColour(juce::Colour::fromRGBA(9, 12, 255, 255));
        g.setColour(juce::Colours::lightblue);
        for (uint32_t i = 1; i < bezier.getSize(); i++)
        {
            auto x1 = bezier.getPoint(i - 1)->getX() * this->getLocalBounds().getWidth();
            auto y1 = bezier.getPoint(i - 1)->getY() * this->getLocalBounds().getHeight();
            
            auto x2 = bezier.getPoint(i)->getX() * this->getLocalBounds().getWidth();
            auto y2 = bezier.getPoint(i)->getY() * this->getLocalBounds().getHeight();

            g.drawLine(x1, y1, x2, y2, 2);
        }
    }
    
}

void AuxShaper::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    auto x = this->getLocalBounds().getWidth() / 10;
    auto y = this->getLocalBounds().getHeight() / 10;
    if (!drawingBool)
    {
        p1.setPointBounds(this->getLocalBounds());
        //p1.setCoordinate(10*x * *parameterMap->getFloatParameter("x1"), 10 * y * *parameterMap->getFloatParameter("y1"));
        p1.setCoordinate(10*x * *xP, 10 * y * *yP);
        normalize.setXY(10 * x, 10 * y);
        startPoint.setXY(0, 10 * y);
        endPoint.setXY(10 * x, 0);

        drawingBool = true;
    }
    p1.setBounds(p1.getCoordinate()->x, p1.getCoordinate()->y, 1.5 * x, 1.5 * y);
    
}



