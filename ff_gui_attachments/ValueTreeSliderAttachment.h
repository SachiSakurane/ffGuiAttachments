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
    ValueTreeSliderAttachment (const juce::ValueTree& attachToTree,
                               juce::Identifier valueProperty,
                               juce::Slider& slider,
                               juce::UndoManager* undoManagerToUse = nullptr)
    : tree_     (attachToTree),
      slider_   (slider),
      property_ (std::move(valueProperty)),
      undo_  (undoManagerToUse)
    {
        // Don't attach an invalid valuetree!
        jassert (tree_.isValid());

        if (tree_.hasProperty (property_))
        {
            slider_.setValue (tree_.getProperty(property_));
        }
        else
        {
            tree_.setProperty (property_, slider_.getValue(), undo_);
        }

        tree_.addListener (this);
        slider_.addListener (this);
    }

    ~ValueTreeSliderAttachment () override
    {
        tree_.removeListener (this);
        slider_.removeListener (this);
    }

    /**
     This updates the ValueTree's property to reflect the Slider's position
     */
    void sliderValueChanged (juce::Slider *sliderThatChanged) override
    {
        if (std::unique_lock lock{mutex_, std::try_to_lock}; lock)
        {
            if (&slider_ == sliderThatChanged)
            {
                tree_.setProperty (property_, slider_.getValue(), undo_);
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
            if (treeWhosePropertyHasChanged == tree_)
            {
                if (changedProperty == property_)
                {
                    slider_.setValue (tree_.getProperty (property_));
                }
            }
        }
    }

private:
    juce::ValueTree    tree_;
    juce::Slider&      slider_;
    juce::Identifier   property_;
    juce::UndoManager* undo_;
    std::mutex         mutex_;
};
