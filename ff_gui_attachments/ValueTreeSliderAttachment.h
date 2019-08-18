/*
 ==============================================================================

 Copyright (c) 2016, Daniel Walz
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 3. Neither the name of the copyright holder nor the names of its contributors
 may be used to endorse or promote products derived from this software without
 specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 OF THE POSSIBILITY OF SUCH DAMAGE.

 ==============================================================================
 */

/*
  ==============================================================================

    ValueTreeSliderAttachment.h
    Created: 4 Jun 2016 11:17:03am
    Author:  Daniel Walz / Foleys Finest Audio

  ==============================================================================
*/

#pragma once

#include <mutex>
#include <utility>

/**
 \class ValueTreeSliderAttachment
 \brief This class updates a Slider to a property in a ValueTree
 */
class ValueTreeSliderAttachment : public juce::Slider::Listener,
                                  public juce::ValueTree::Listener
{
public:
    /**
     Creates a ValueTreeSliderAttachment. The Slider gets it's values from properties of the ValueTree node.
     You can specify the names of the corresponding properties here.
    */
    ValueTreeSliderAttachment (juce::ValueTree& attachToTree,
                               juce::Identifier valueProperty,
                               juce::Slider& _slider,
                               juce::UndoManager* undoManagerToUse = nullptr)
    :   tree     (attachToTree),
        slider   (_slider),
        property (std::move(valueProperty)),
        undoMgr  (undoManagerToUse)
    {
        // Don't attach an invalid valuetree!
        jassert (tree.isValid());

        if (tree.hasProperty (property))
        {
            slider.setValue (tree.getProperty(property));
        }
        else
        {
            tree.setProperty (property, slider.getValue(), undoMgr);
        }

        tree.addListener (this);
        slider.addListener (this);
    }

    ~ValueTreeSliderAttachment ()
    {
        tree.removeListener (this);
        slider.removeListener (this);
    }

    /**
     This updates the ValueTree's property to reflect the Slider's position
     */
    void sliderValueChanged (juce::Slider *sliderThatChanged) override
    {
        if (std::unique_lock lock{mutex_, std::try_to_lock}; lock)
        {
            if (&slider == sliderThatChanged)
            {
                tree.setProperty (property, slider.getValue(), undoMgr);
            }
        }
    }

    /**
     This updates the Slider to reflect the ValueTree's property
     */
    void valueTreePropertyChanged (juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &changedProperty) override
    {
        if (std::unique_lock lock{mutex_, std::try_to_lock}; lock)
        {
            if (treeWhosePropertyHasChanged == tree)
            {
                if (changedProperty == property)
                {
                    slider.setValue (tree.getProperty (property));
                }
            }
        }
    }
    void valueTreeChildAdded (juce::ValueTree &parentTree, juce::ValueTree &childWhichHasBeenAdded) override {}
    void valueTreeChildRemoved (juce::ValueTree &parentTree, juce::ValueTree &childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) override {}
    void valueTreeChildOrderChanged (juce::ValueTree &parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex) override {}
    void valueTreeParentChanged (juce::ValueTree &treeWhoseParentHasChanged) override {}
    void valueTreeRedirected (juce::ValueTree &treeWhichHasBeenChanged) override {}


private:
    juce::ValueTree&   tree;
    juce::Slider&      slider;
    juce::Identifier   property;
    juce::UndoManager* undoMgr;
    std::mutex         mutex_;
};
