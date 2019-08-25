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

    ValueTreeLabelAttachment.h
    Created: 4 Jun 2016 11:17:03am
    Author:  Oli Larkin / Daniel Walz

  ==============================================================================
*/

#pragma once

#include <mutex>
#include <utility>

/**
 \class ValueTreeLabelAttachment
 \brief Connects a Label to a ValueTree node to synchronise
 */
class ValueTreeLabelAttachment : public juce::Label::Listener,
                                 public juce::ValueTree::Listener
{
public:
    /**
     Creates an attachment to synchronise a Label to a ValueTree node.
     You can set a \param property, in which the text the label is showing is stored
     */
    ValueTreeLabelAttachment (const juce::ValueTree& tree,
                              juce::Identifier property,
                              juce::Label& label,
                              juce::UndoManager* undo = nullptr)
    : tree_ (tree),
      label (label),
      property (std::move(property)),
      undoMgr (undo)
    {
        // Don't attach an invalid valuetree!
        jassert (tree.isValid());

        if (tree.hasProperty (property)) {
            label.setText (tree.getProperty(property), juce::dontSendNotification);
        }
        else {
            tree_.setProperty (property, label.getText(), undoMgr);
        }

        tree_.addListener (this);
        label.addListener (this);
    }

    ~ValueTreeLabelAttachment () override
    {
        tree_.removeListener (this);
        label.removeListener (this);
    }

    /**
     This updates the ValueTree to the Label's text
     */
    void labelTextChanged (juce::Label *_label) override
    {
        if (std::unique_lock lock{mutex_, std::try_to_lock}; lock && &label == _label)
        {
            tree_.setProperty (property, label.getText(), undoMgr);
        }
    }

    /**
     This updates the Label to display the ValueTree's property
     */
    void valueTreePropertyChanged (juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &_property) override
    {
        if (std::unique_lock lock{mutex_, std::try_to_lock}; lock)
        {
            if (treeWhosePropertyHasChanged == tree_) {
                if (_property == property) {
                    label.setText(tree_.getProperty (property), juce::dontSendNotification);
                }
            }
        }
    }

private:
    juce::ValueTree    tree_;
    juce::Label&       label;
    juce::Identifier   property;
    juce::UndoManager* undoMgr  = nullptr;
    std::mutex         mutex_;
};

