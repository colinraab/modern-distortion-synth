/*
  ==============================================================================

    PresetPanel.h
    Created: 8 Jun 2023 1:36:45pm
    Author:  Colin Raab
 
    Adapted from Akash Murthy's Preset Manager tutorial
    https://www.youtube.com/watch?v=YwAtWuGA4Cg

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace Gui
{
    class PresetPanel : public juce::Component, juce::Button::Listener, juce::ComboBox::Listener
    {
    public:
        PresetPanel(Service::PresetManager& pm) : presetManager(pm)
        {
            configureButton(saveButton, "Save");
            configureButton(deleteButton, "Delete");
            configureButton(previousPresetButton, "<");
            configureButton(nextPresetButton, ">");

            presetList.setTextWhenNothingSelected("");
            presetList.setMouseCursor(juce::MouseCursor::PointingHandCursor);
            addAndMakeVisible(presetList);
            presetList.addListener(this);
            
            juce::Image saveUp = juce::ImageCache::getFromMemory(BinaryData::save_button_png, BinaryData::save_button_pngSize).getClippedImage(juce::Rectangle<int>(0,37,128,52));
            juce::Image saveDown = juce::ImageCache::getFromMemory(BinaryData::save_button_png, BinaryData::save_button_pngSize).getClippedImage(juce::Rectangle<int>(0,165,128,52));
            saveButton.setImages(false, true, true, saveUp, 1, juce::Colours::transparentBlack, saveUp, 1, juce::Colours::transparentBlack, saveDown, 1, juce::Colours::transparentBlack);
            
            juce::Image delUp = juce::ImageCache::getFromMemory(BinaryData::del_button_png, BinaryData::del_button_pngSize).getClippedImage(juce::Rectangle<int>(0,37,128,52));
            juce::Image delDown = juce::ImageCache::getFromMemory(BinaryData::del_button_png, BinaryData::del_button_pngSize).getClippedImage(juce::Rectangle<int>(0,165,128,52));
            deleteButton.setImages(false, true, true, delUp, 1, juce::Colours::transparentBlack, delUp, 1, juce::Colours::transparentBlack, delDown, 1, juce::Colours::transparentBlack);
            
            juce::Image leftUp = juce::ImageCache::getFromMemory(BinaryData::left_button_png, BinaryData::left_button_pngSize).getClippedImage(juce::Rectangle<int>(0,37,61,52));
            juce::Image leftDown = juce::ImageCache::getFromMemory(BinaryData::left_button_png, BinaryData::left_button_pngSize).getClippedImage(juce::Rectangle<int>(0,165,61,52));
            previousPresetButton.setImages(false, true, true, leftUp, 1, juce::Colours::transparentBlack, leftUp, 1, juce::Colours::transparentBlack, leftDown, 1, juce::Colours::transparentBlack);
            
            juce::Image rightUp = juce::ImageCache::getFromMemory(BinaryData::right_button_png, BinaryData::right_button_pngSize).getClippedImage(juce::Rectangle<int>(0,37,61,52));
            juce::Image rightDown = juce::ImageCache::getFromMemory(BinaryData::right_button_png, BinaryData::right_button_pngSize).getClippedImage(juce::Rectangle<int>(0,165,61,52));
            nextPresetButton.setImages(false, true, true, rightUp, 1, juce::Colours::transparentBlack, rightUp, 1, juce::Colours::transparentBlack, rightDown, 1, juce::Colours::transparentBlack);
            
            previousPresetButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::black);
            nextPresetButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::black);
            presetList.setColour(juce::ComboBox::ColourIds::backgroundColourId, juce::Colours::black);
            
            loadPresetList();
        }

        ~PresetPanel()
        {
            saveButton.removeListener(this);
            deleteButton.removeListener(this);
            previousPresetButton.removeListener(this);
            nextPresetButton.removeListener(this);
            presetList.removeListener(this);
        }

        void resized() override
        {
            saveButton.setBounds(0, 0, 60, 30);
            previousPresetButton.setBounds(0, 32, 25, 30);
            presetList.setBounds(0, 65, 60, 20);
            nextPresetButton.setBounds(35, 32, 25, 30);
            deleteButton.setBounds(0, 90, 60, 30);
        }
        
        
    private:
        void buttonClicked(juce::Button* button) override
        {
            if (button == &saveButton)
            {
                fileChooser = std::make_unique<juce::FileChooser>(
                    "Please enter the name of the preset to save",
                    Service::PresetManager::defaultDirectory,
                    "*." + Service::PresetManager::extension
                );
                fileChooser->launchAsync(juce::FileBrowserComponent::saveMode, [&](const juce::FileChooser& chooser)
                    {
                        const auto resultFile = chooser.getResult();
                        presetManager.savePreset(resultFile.getFileNameWithoutExtension());
                        loadPresetList();
                    });
            }
            if (button == &previousPresetButton)
            {
                const auto index = presetManager.loadPreviousPreset();
                presetList.setSelectedItemIndex(index, juce::dontSendNotification);
            }
            if (button == &nextPresetButton)
            {
                const auto index = presetManager.loadNextPreset();
                presetList.setSelectedItemIndex(index, juce::dontSendNotification);
            }
            if (button == &deleteButton)
            {
                presetManager.deletePreset(presetManager.getCurrentPreset());
                loadPresetList();
            }
            
            getParentComponent()->resized();
        }
        void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override
        {
            if (comboBoxThatHasChanged == &presetList)
            {
                presetManager.loadPreset(presetList.getItemText(presetList.getSelectedItemIndex()));
                getParentComponent()->resized();
            }
        }

        void configureButton(juce::Button& button, const juce::String& buttonText)
        {
            button.setButtonText(buttonText);
            button.setMouseCursor(juce::MouseCursor::PointingHandCursor);
            addAndMakeVisible(button);
            button.addListener(this);
        }

        void loadPresetList()
        {
            presetList.clear(juce::dontSendNotification);
            const auto allPresets = presetManager.getAllPresets();
            const auto currentPreset = presetManager.getCurrentPreset();
            presetList.addItemList(allPresets, 1);
            presetList.setSelectedItemIndex(allPresets.indexOf(currentPreset), juce::dontSendNotification);
        }

        Service::PresetManager& presetManager;
        juce::ImageButton saveButton, deleteButton, previousPresetButton, nextPresetButton;
        juce::ComboBox presetList;
        std::unique_ptr<juce::FileChooser> fileChooser;
        

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetPanel)
    };
}
