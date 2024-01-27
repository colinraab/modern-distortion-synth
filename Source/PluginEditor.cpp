#include "PluginProcessor.h"
#include "PluginEditor.h"

blackKnob::blackKnob() {
    img1 = juce::ImageCache::getFromMemory(BinaryData::black_knob_png, BinaryData::black_knob_pngSize);
}

void blackKnob::drawRotarySlider(juce::Graphics& g,
    int x, int y, int width, int height, float sliderPos,
    float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{

    if (img1.isValid())
    {
        const double rotation = (slider.getValue()
            - slider.getMinimum())
            / (slider.getMaximum()
                - slider.getMinimum());

        const int frames = img1.getHeight() / img1.getWidth();
        const int frameId = (int)ceil(rotation * ((double)frames - 1.0));
        const float radius = juce::jmin(width / 2.0f, height / 2.0f);
        const float centerX = x + width * 0.5f;
        const float centerY = y + height * 0.5f;
        const float rx = centerX - radius - 1.0f;
        const float ry = centerY - radius;

        g.drawImage(img1,
            (int)rx,
            (int)ry,
            2 * (int)radius,
            2 * (int)radius,
            0,
            frameId*img1.getWidth(),
            img1.getWidth(),
            img1.getWidth());
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

silverKnob::silverKnob() {
    img1 = juce::ImageCache::getFromMemory(BinaryData::silver_knob_png, BinaryData::silver_knob_pngSize);
}

void silverKnob::drawRotarySlider(juce::Graphics& g,
    int x, int y, int width, int height, float sliderPos,
    float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{

    if (img1.isValid())
    {
        const double rotation = (slider.getValue()
            - slider.getMinimum())
            / (slider.getMaximum()
                - slider.getMinimum());

        const int frames = img1.getHeight() / img1.getWidth();
        const int frameId = (int)ceil(rotation * ((double)frames - 1.0));
        const float radius = juce::jmin(width / 2.0f, height / 2.0f);
        const float centerX = x + width * 0.5f;
        const float centerY = y + height * 0.5f;
        const float rx = centerX - radius - 1.0f;
        const float ry = centerY - radius;

        g.drawImage(img1,
            (int)rx,
            (int)ry,
            2 * (int)radius,
            2 * (int)radius,
            0,
            frameId*img1.getWidth(),
            img1.getWidth(),
            img1.getWidth());
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

//==============================================================================
CapstoneAudioProcessorEditor::CapstoneAudioProcessorEditor (CapstoneAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    mainWindow(juce::TabbedButtonBar::Orientation::TabsAtBottom),
    presetPanel(p.getPresetManager()),
    verticalDiscreteMeterL([&]() { return audioProcessor.getRmsValue(0); }),
    verticalDiscreteMeterR([&]() { return audioProcessor.getRmsValue(1); })
{
    setSize (750, 300);
    juce::LookAndFeel::setDefaultLookAndFeel(&newLook);
    
    /// Begin constructor by grabbing parameters from parameterMap defined in the PluginProcessor
    
    auto osc1DSel = audioProcessor.parameterMap.getIntParameter("osc1DistSel");
    auto osc2DSel = audioProcessor.parameterMap.getIntParameter("osc2DistSel");
    auto noiseDSel = audioProcessor.parameterMap.getIntParameter("noiseDistSel");
    auto samplerDSel = audioProcessor.parameterMap.getIntParameter("samplerDistSel");
    auto mainDSel = audioProcessor.parameterMap.getIntParameter("mainDistSel");
    auto osc1DAmt = audioProcessor.parameterMap.getFloatParameter("osc1Drive");
    auto osc2DAmt = audioProcessor.parameterMap.getFloatParameter("osc2Drive");
    auto noiseDAmt = audioProcessor.parameterMap.getFloatParameter("noiseDrive");
    auto samplerDAmt = audioProcessor.parameterMap.getFloatParameter("samplerDrive");
    mainDAmt = audioProcessor.parameterMap.getFloatParameter("mainDrive");
    auto osc1DCoeff = audioProcessor.parameterMap.getFloatParameter("osc1DCoeff");
    auto osc2DCoeff = audioProcessor.parameterMap.getFloatParameter("osc2DCoeff");
    auto noiseDCoeff = audioProcessor.parameterMap.getFloatParameter("noiseDCoeff");
    auto samplerDCoeff = audioProcessor.parameterMap.getFloatParameter("samplerDCoeff");
    auto mainDCoeff = audioProcessor.parameterMap.getFloatParameter("mainDCoeff");
    
    auto osc1Atk = audioProcessor.parameterMap.getFloatParameter("osc1Atk");
    auto osc1Dec = audioProcessor.parameterMap.getFloatParameter("osc1Dec");
    auto osc1Sus = audioProcessor.parameterMap.getFloatParameter("osc1Sus");
    auto osc1Rel = audioProcessor.parameterMap.getFloatParameter("osc1Rel");
    auto osc1Depth = audioProcessor.parameterMap.getFloatParameter("osc1Depth");
    auto osc2Atk = audioProcessor.parameterMap.getFloatParameter("osc2Atk");
    auto osc2Dec = audioProcessor.parameterMap.getFloatParameter("osc2Dec");
    auto osc2Sus = audioProcessor.parameterMap.getFloatParameter("osc2Sus");
    auto osc2Rel = audioProcessor.parameterMap.getFloatParameter("osc2Rel");
    auto osc2Depth = audioProcessor.parameterMap.getFloatParameter("osc2Depth");
    auto noiseAtk = audioProcessor.parameterMap.getFloatParameter("noiseAtk");
    auto noiseDec = audioProcessor.parameterMap.getFloatParameter("noiseDec");
    auto noiseSus = audioProcessor.parameterMap.getFloatParameter("noiseSus");
    auto noiseRel = audioProcessor.parameterMap.getFloatParameter("noiseRel");
    auto noiseDepth = audioProcessor.parameterMap.getFloatParameter("noiseDepth");
    auto samplerAtk = audioProcessor.parameterMap.getFloatParameter("samplerAtk");
    auto samplerDec = audioProcessor.parameterMap.getFloatParameter("samplerDec");
    auto samplerSus = audioProcessor.parameterMap.getFloatParameter("samplerSus");
    auto samplerRel = audioProcessor.parameterMap.getFloatParameter("samplerRel");
    auto samplerDepth = audioProcessor.parameterMap.getFloatParameter("samplerDepth");
    auto mainAtk = audioProcessor.parameterMap.getFloatParameter("mainAtk");
    auto mainDec = audioProcessor.parameterMap.getFloatParameter("mainDec");
    auto mainSus = audioProcessor.parameterMap.getFloatParameter("mainSus");
    auto mainRel = audioProcessor.parameterMap.getFloatParameter("mainRel");
    
    auto osc1etV = audioProcessor.parameterMap.getBoolParameter("osc1envToVol");
    auto osc1etD = audioProcessor.parameterMap.getBoolParameter("osc1envToDist");
    auto osc1etF = audioProcessor.parameterMap.getBoolParameter("osc1envToFilt");
    auto osc2etV = audioProcessor.parameterMap.getBoolParameter("osc2envToVol");
    auto osc2etD = audioProcessor.parameterMap.getBoolParameter("osc2envToDist");
    auto osc2etF = audioProcessor.parameterMap.getBoolParameter("osc2envToFilt");
    auto noiseetV = audioProcessor.parameterMap.getBoolParameter("noiseenvToVol");
    auto noiseetD = audioProcessor.parameterMap.getBoolParameter("noiseenvToDist");
    auto noiseetF = audioProcessor.parameterMap.getBoolParameter("noiseenvToFilt");
    auto sampleretV = audioProcessor.parameterMap.getBoolParameter("samplerenvToVol");
    auto sampleretD = audioProcessor.parameterMap.getBoolParameter("samplerenvToDist");
    auto sampleretF = audioProcessor.parameterMap.getBoolParameter("samplerenvToFilt");
    
    auto fm1 = audioProcessor.parameterMap.getFloatParameter("FMDepth1");
    auto fm2 = audioProcessor.parameterMap.getFloatParameter("FMDepth2");
    
    auto osc1WaveSlider = audioProcessor.parameterMap.getFloatParameter("osc1WaveSlider");
    auto osc2WaveSlider = audioProcessor.parameterMap.getFloatParameter("osc2WaveSlider");
    auto noiseWaveSlider = audioProcessor.parameterMap.getFloatParameter("noiseWaveSlider");
    auto samplerWaveSlider = audioProcessor.parameterMap.getFloatParameter("samplerWaveSlider");
    auto mainWaveSlider = audioProcessor.parameterMap.getFloatParameter("mainWaveSlider");
    auto osc1DistSlider = audioProcessor.parameterMap.getFloatParameter("osc1DistSlider");
    auto osc2DistSlider = audioProcessor.parameterMap.getFloatParameter("osc2DistSlider");
    auto noiseDistSlider = audioProcessor.parameterMap.getFloatParameter("noiseDistSlider");
    auto samplerDistSlider = audioProcessor.parameterMap.getFloatParameter("samplerDistSlider");
    auto mainDistSlider = audioProcessor.parameterMap.getFloatParameter("mainDistSlider");
    
    auto osc1Filter = audioProcessor.parameterMap.getIntParameter("osc1Filter");
    auto osc2Filter = audioProcessor.parameterMap.getIntParameter("osc2Filter");
    auto noiseFilter = audioProcessor.parameterMap.getIntParameter("noiseFilter");
    auto samplerFilter = audioProcessor.parameterMap.getIntParameter("samplerFilter");
    auto mainFilter = audioProcessor.parameterMap.getIntParameter("mainFilter");
    auto osc1Cutoff = audioProcessor.parameterMap.getFloatParameter("osc1Cutoff");
    auto osc2Cutoff = audioProcessor.parameterMap.getFloatParameter("osc2Cutoff");
    auto noiseCutoff = audioProcessor.parameterMap.getFloatParameter("noiseCutoff");
    auto samplerCutoff = audioProcessor.parameterMap.getFloatParameter("samplerCutoff");
    mainCutoff = audioProcessor.parameterMap.getFloatParameter("mainCutoff");
    auto osc1Res = audioProcessor.parameterMap.getFloatParameter("osc1Res");
    auto osc2Res = audioProcessor.parameterMap.getFloatParameter("osc2Res");
    auto noiseRes = audioProcessor.parameterMap.getFloatParameter("noiseRes");
    auto samplerRes = audioProcessor.parameterMap.getFloatParameter("samplerRes");
    auto mainRes = audioProcessor.parameterMap.getFloatParameter("mainRes");
    auto osc1Keytrack = audioProcessor.parameterMap.getBoolParameter("osc1Keytrack");
    auto osc2Keytrack = audioProcessor.parameterMap.getBoolParameter("osc1Keytrack");
    auto noiseKeytrack = audioProcessor.parameterMap.getBoolParameter("noiseKeytrack");
    auto samplerKeytrack = audioProcessor.parameterMap.getBoolParameter("samplerKeytrack");
    auto osc1ktA = audioProcessor.parameterMap.getFloatParameter("osc1ktA");
    auto osc2ktA = audioProcessor.parameterMap.getFloatParameter("osc2ktA");
    auto noisektA = audioProcessor.parameterMap.getFloatParameter("noisektA");
    auto samplerktA = audioProcessor.parameterMap.getFloatParameter("samplerktA");
    
    auto osc1Pitch = audioProcessor.parameterMap.getFloatParameter("osc1Pitch");
    auto osc2Pitch = audioProcessor.parameterMap.getFloatParameter("osc2Pitch");
    auto samplerPitch = audioProcessor.parameterMap.getFloatParameter("samplerPitch");
    auto samplerRepitch = audioProcessor.parameterMap.getBoolParameter("samplerRepitch");
    auto samplerLoop = audioProcessor.parameterMap.getBoolParameter("samplerLoop");
    auto osc1Wave = audioProcessor.parameterMap.getIntParameter("osc1Wave");
    auto osc2Wave = audioProcessor.parameterMap.getIntParameter("osc2Wave");
    auto noiseWave = audioProcessor.parameterMap.getIntParameter("noiseWave");
    
    osc1Vol = audioProcessor.parameterMap.getFloatParameter("osc1Vol");
    osc2Vol = audioProcessor.parameterMap.getFloatParameter("osc2Vol");
    noiseVol = audioProcessor.parameterMap.getFloatParameter("noiseVol");
    samplerVol = audioProcessor.parameterMap.getFloatParameter("samplerVol");
    mainVol = audioProcessor.parameterMap.getFloatParameter("mainVol");
    
    auto cThresh = audioProcessor.parameterMap.getFloatParameter("cThresh");
    auto cRatio = audioProcessor.parameterMap.getFloatParameter("cRatio");
    auto cAtk = audioProcessor.parameterMap.getFloatParameter("cAtk");
    auto cRel = audioProcessor.parameterMap.getFloatParameter("cRel");
    
    auto xParam1 = audioProcessor.parameterMap.getFloatParameter("xParam1");
    auto yParam1 = audioProcessor.parameterMap.getFloatParameter("yParam1");
    auto slopeParam1 = audioProcessor.parameterMap.getFloatParameter("slopeParam1");
    auto xParam2 = audioProcessor.parameterMap.getFloatParameter("xParam2");
    auto yParam2 = audioProcessor.parameterMap.getFloatParameter("yParam2");
    auto slopeParam2 = audioProcessor.parameterMap.getFloatParameter("slopeParam2");
    auto xParamN = audioProcessor.parameterMap.getFloatParameter("xParamN");
    auto yParamN = audioProcessor.parameterMap.getFloatParameter("yParamN");
    auto slopeParamN = audioProcessor.parameterMap.getFloatParameter("slopeParamN");
    auto xParamS = audioProcessor.parameterMap.getFloatParameter("xParamS");
    auto yParamS = audioProcessor.parameterMap.getFloatParameter("yParamS");
    auto slopeParamS = audioProcessor.parameterMap.getFloatParameter("slopeParamS");
    auto xParamM = audioProcessor.parameterMap.getFloatParameter("xParamM");
    auto yParamM = audioProcessor.parameterMap.getFloatParameter("yParamM");
    auto slopeParamM = audioProcessor.parameterMap.getFloatParameter("slopeParamM");
    
    /// Create TabbedComponent with five pages (subcomponents) for each of the sound sources and a main page, assign parameters to them
    
    osc1Page = new Colin::Osc1Page(audioProcessor.osc1, osc1DSel, osc1DAmt, osc1DCoeff, fm1, osc1Pitch, osc1Wave, osc1Filter, osc1Cutoff, osc1Res, osc1Keytrack, osc1ktA, osc1Atk, osc1Dec, osc1Sus, osc1Rel, osc1Depth, osc1etV, osc1etD, osc1etF, osc1WaveSlider, osc1DistSlider, audioProcessor.bezier1, xParam1, yParam1, slopeParam1);
    
    osc2Page = new Colin::Osc2Page(audioProcessor.osc2, osc2DSel, osc2DAmt, osc2DCoeff, fm2, osc2Pitch, osc2Wave, osc2Filter, osc2Cutoff, osc2Res, osc2Keytrack, osc2ktA, osc2Atk, osc2Dec, osc2Sus, osc2Rel, osc2Depth, osc2etV, osc2etD, osc2etF, osc2WaveSlider, osc2DistSlider, audioProcessor.bezier2, xParam2, yParam2, slopeParam2);
    
    noisePage = new Colin::NoisePage(audioProcessor.noise, noiseDSel, noiseDAmt, noiseDCoeff, noiseWave, noiseFilter, noiseCutoff, noiseRes, noiseKeytrack, noisektA, noiseAtk, noiseDec, noiseSus, noiseRel, noiseDepth, noiseetV, noiseetD, noiseetF, noiseWaveSlider, noiseDistSlider, audioProcessor.bezierN, xParamN, yParamN, slopeParamN);
    
    samplerPage = new Colin::SamplerPage(audioProcessor.sampler, samplerDSel, samplerDAmt, samplerDCoeff, samplerFilter, samplerCutoff, samplerRes, samplerKeytrack, samplerktA, samplerLoop, samplerPitch, samplerRepitch, samplerAtk, samplerDec, samplerSus, samplerRel, samplerDepth, sampleretV, sampleretD, sampleretF, samplerWaveSlider, samplerDistSlider, audioProcessor.bezierS, xParamS, yParamS, slopeParamS);
    
    mainPage = new Colin::MainPage(audioProcessor.distMain, mainDSel, mainDAmt, mainDCoeff, mainFilter, mainCutoff, mainRes, mainAtk, mainDec, mainSus, mainRel, cThresh, cRatio, cAtk, cRel, mainWaveSlider, mainDistSlider, audioProcessor.oscilloscope, audioProcessor.bezierM, xParamM, yParamM, slopeParamM);
    
    juce::Colour tabcolor(juce::Colours::black);
    tabcolor = tabcolor.withAlpha(0.1f);
    
    mainWindow.setOutline(2);
    mainWindow.addTab("OSC 1", tabcolor, osc1Page, true);
    mainWindow.addTab("OSC 2", tabcolor, osc2Page, true);
    mainWindow.addTab("NOISE", tabcolor, noisePage, true);
    mainWindow.addTab("SAMPLE", tabcolor, samplerPage, true);
    mainWindow.addTab("MAIN", tabcolor, mainPage, true);
    addAndMakeVisible(mainWindow);
    
    /// Setup global synth controls (outside of the TabbedComponent)
    
    addAndMakeVisible(osc1V);
    osc1V.setLookAndFeel(&bk);
    osc1V.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    osc1V.setTextBoxStyle(juce::Slider::NoTextBox, false, 60, 20);
    osc1V.setRange(osc1Vol->range.start, osc1Vol->range.end,1);
    osc1V.setValue(*osc1Vol);
    osc1V.onValueChange = [this]
    {
        *osc1Vol = osc1V.getValue();
        osc1V.setValue(*osc1Vol);
    };
    
    addAndMakeVisible(osc2V);
    osc2V.setLookAndFeel(&bk);
    osc2V.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    osc2V.setTextBoxStyle(juce::Slider::NoTextBox, false, 60, 20);
    osc2V.setRange(osc2Vol->range.start, osc2Vol->range.end,1);
    osc2V.setValue(*osc2Vol);
    osc2V.onValueChange = [this]
    {
        *osc2Vol = osc2V.getValue();
        osc2V.setValue(*osc2Vol);
    };
    
    addAndMakeVisible(noiseV);
    noiseV.setLookAndFeel(&bk);
    noiseV.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    noiseV.setTextBoxStyle(juce::Slider::NoTextBox, false, 60, 20);
    noiseV.setRange(noiseVol->range.start, noiseVol->range.end,1);
    noiseV.setValue(*noiseVol);
    noiseV.onValueChange = [this]
    {
        *noiseVol = noiseV.getValue();
        noiseV.setValue(*noiseVol);
    };
    
    addAndMakeVisible(samplerV);
    samplerV.setLookAndFeel(&bk);
    samplerV.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    samplerV.setTextBoxStyle(juce::Slider::NoTextBox, false, 60, 20);
    samplerV.setRange(samplerVol->range.start, samplerVol->range.end,1);
    samplerV.setValue(*samplerVol);
    samplerV.onValueChange = [this]
    {
        *samplerVol = samplerV.getValue();
        samplerV.setValue(*samplerVol);
    };
    
    addAndMakeVisible(mainV);
    mainV.setLookAndFeel(&sk);
    mainV.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mainV.setTextBoxStyle(juce::Slider::NoTextBox, false, 60, 20);
    mainV.setRange(mainVol->range.start, mainVol->range.end,1);
    mainV.setValue(*mainVol);
    mainV.onValueChange = [this]
    {
        *mainVol = mainV.getValue();
        mainV.setValue(*mainVol);
    };
    
    addAndMakeVisible(mainC);
    mainC.setLookAndFeel(&sk);
    mainC.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mainC.setTextBoxStyle(juce::Slider::NoTextBox, false, 60, 20);
    mainC.setRange(mainCutoff->range.start, mainCutoff->range.end,1);
    mainC.setSkewFactor(0.3);
    mainC.setValue(*mainCutoff);
    mainC.onValueChange = [this]
    {
        *mainCutoff = mainC.getValue();
        mainC.setValue(*mainCutoff);
        mainPage->updateParams();
    };
    
    addAndMakeVisible(mainD);
    mainD.setLookAndFeel(&sk);
    mainD.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mainD.setTextBoxStyle(juce::Slider::NoTextBox, false, 60, 20);
    mainD.setRange(mainDAmt->range.start, mainDAmt->range.end,1);
    mainD.setValue(*mainDAmt);
    mainD.onValueChange = [this]
    {
        *mainDAmt = mainD.getValue();
        mainD.setValue(*mainDAmt);
        mainPage->updateParams();
    };
    
    /// Initialize preset panel UI elements
    
    addAndMakeVisible(randomize);
    juce::Image randUp = juce::ImageCache::getFromMemory(BinaryData::rand_button_png, BinaryData::rand_button_pngSize).getClippedImage(juce::Rectangle<int>(0,37,128,52));
    juce::Image randDown = juce::ImageCache::getFromMemory(BinaryData::rand_button_png, BinaryData::rand_button_pngSize).getClippedImage(juce::Rectangle<int>(0,165,128,52));
    randomize.setImages(false, true, true, randUp, 1, juce::Colours::transparentBlack, randUp, 1, juce::Colours::transparentBlack, randDown, 1, juce::Colours::transparentBlack);
    randomize.onClick = [this] {
        audioProcessor.randomizeParams();
        updateParamUI();
        repaint();
    };
    
    addAndMakeVisible(init);
    juce::Image initUp = juce::ImageCache::getFromMemory(BinaryData::init_button_png, BinaryData::init_button_pngSize).getClippedImage(juce::Rectangle<int>(0,37,128,52));
    juce::Image initDown = juce::ImageCache::getFromMemory(BinaryData::init_button_png, BinaryData::init_button_pngSize).getClippedImage(juce::Rectangle<int>(0,165,128,52));
    init.setImages(false, true, true, initUp, 1, juce::Colours::transparentBlack, initUp, 1, juce::Colours::transparentBlack, initDown, 1, juce::Colours::transparentBlack);
    init.onClick = [this] {
        audioProcessor.parameterMap.init();
        updateParamUI();
    };
    
    addAndMakeVisible(presetPanel);
    
    /// Initialize VU Meter
    
    addAndMakeVisible(verticalDiscreteMeterL);
    addAndMakeVisible(verticalDiscreteMeterR);
    
    //auto fps = 10.f;
    //startTimer(int(1000.f / fps));
}

CapstoneAudioProcessorEditor::~CapstoneAudioProcessorEditor()
{
    osc1V.setLookAndFeel(nullptr);
    osc2V.setLookAndFeel(nullptr);
    noiseV.setLookAndFeel(nullptr);
    samplerV.setLookAndFeel(nullptr);
    mainV.setLookAndFeel(nullptr);
    mainC.setLookAndFeel(nullptr);
    mainD.setLookAndFeel(nullptr);
}

//==============================================================================
void CapstoneAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
    juce::Image background = juce::ImageFileFormat::loadFrom(BinaryData::background_png, BinaryData::background_pngSize);
    
    g.drawImage(background,0,0,750,300,0,0,1500,600);
    updateParamUI();
}

void CapstoneAudioProcessorEditor::resized()
{
    auto x = getWidth() / 10;
    auto y = getHeight() / 10;
    
    randomize.setBounds(10, 65, 60, 30);
    init.setBounds(10, 30, 60, 30);
    
    osc1V.setBounds(1.2*x, 7.7*y, 1*x, 2*y);
    osc2V.setBounds(2*x, 7.7*y, 1*x, 2*y);
    noiseV.setBounds(2.8*x, 7.7*y, 1*x, 2*y);
    samplerV.setBounds(3.6*x, 7.7*y, 1*x, 2*y);
    mainV.setBounds(583, .27*y, 3*x, 3.5*y);
    mainD.setBounds(583, 3.3*y, 3*x, 3*y);
    mainC.setBounds(583, 5.87*y, 3*x, 3*y);
    
    mainWindow.setBounds(95, 18, 525, 218);
        
    verticalDiscreteMeterL.setBounds(418, 220, 200, 40);
    verticalDiscreteMeterR.setBounds(418, 235, 200, 40);
    
    presetPanel.setBounds(10, 100, 60, 200);
}

void CapstoneAudioProcessorEditor::timerCallback() {
}

void CapstoneAudioProcessorEditor::updateParamUI() {
    osc1V.setValue(*osc1Vol);
    osc2V.setValue(*osc2Vol);
    noiseV.setValue(*noiseVol);
    samplerV.setValue(*samplerVol);
    mainV.setValue(*mainVol);
    mainC.setValue(*mainCutoff);
    mainD.setValue(*mainDAmt);
    
    osc1Page->updateParams();
    osc2Page->updateParams();
    noisePage->updateParams();
    samplerPage->updateParams();
    mainPage->updateParams();
}
