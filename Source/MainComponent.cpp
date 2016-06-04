/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"


//==============================================================================
MainContentComponent::MainContentComponent()
{
    slider = new Slider();
    slider2 = new Slider();

    tree = ValueTree ("TestTree");
    tree.setProperty ("number", 5.0, nullptr);

    ValueTree select = tree.getOrCreateChildWithName ("ComboBox", nullptr);
    ValueTree option1 = ValueTree ("Option");
    option1.setProperty ("name", "Anything", nullptr);
    select.addChild (option1, 0, nullptr);
    ValueTree option2 = ValueTree ("Option");
    option2.setProperty ("name", "Something", nullptr);
    select.addChild (option2, 1, nullptr);
    ValueTree option3 = ValueTree ("Option");
    option3.setProperty ("name", "Nothing", nullptr);
    select.addChild (option3, 2, nullptr);

    combo = new ComboBox();
    combo1 = new ComboBox();

    addAndMakeVisible (slider);
    addAndMakeVisible (slider2);

    addAndMakeVisible (combo);
    addAndMakeVisible (combo1);

    juce::Array<juce::Button*> btns;

    ToggleButton* b = new ToggleButton ("Anything");
    b->setComponentID ("Anything");
    b->setRadioGroupId (100);
    buttons.add (b);
    btns.add (b);
    addAndMakeVisible (b);
    b = new ToggleButton ("Something");
    b->setComponentID ("Something");
    b->setRadioGroupId (100);
    buttons.add (b);
    btns.add (b);
    addAndMakeVisible (b);
    b = new ToggleButton ("Nothing");
    b->setComponentID ("Nothing");
    b->setRadioGroupId (100);
    buttons.add (b);
    btns.add (b);
    addAndMakeVisible (b);

    attachment = new ValueTreeSliderAttachment (tree, slider, "number");
    attachment2 = new ValueTreeSliderAttachment (tree, slider2, "number");

    comboAttachment = new ValueTreeComboBoxAttachment (select, combo, "name", true);
    comboAttachment1 = new ValueTreeComboBoxAttachment (select, combo1, "name", true);

    buttonGroupAttachment = new ValueTreeRadioButtonGroupAttachment (select, btns, "name", true);

    setSize (600, 400);
}

MainContentComponent::~MainContentComponent()
{
}

void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (Colours::lightcoral);

    g.setFont (Font (16.0f));
    g.setColour (Colours::white);
    g.drawText ("Hello World!", getLocalBounds(), Justification::centred, true);
}

void MainContentComponent::resized()
{
    slider->setBounds (5, 5, 150, 60);
    slider2->setBounds(160, 5, 150, 60);

    combo->setBounds (5, 70, 150, 20);
    combo1->setBounds (5, 100, 150, 20);

    for (int i=0; i < buttons.size(); ++i) {
        buttons.getUnchecked (i)->setBounds (160, 70 + i * 30, 150, 25);
    }
}
