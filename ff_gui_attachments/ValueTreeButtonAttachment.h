//
// Created by Himatya on 2019-08-15.
//

#pragma once

#include <mutex>
#include "../JuceLibraryCode/JuceHeader.h"

class ValueTreeButtonAttachment : public juce::Button::Listener,
                                  public juce::ValueTree::Listener
{
public:
    ValueTreeButtonAttachment (juce::ValueTree& tree,
                               juce::Button* button,
                               juce::Identifier property,
                               juce::UndoManager* undo = nullptr)
        :   tree_ (tree),
            property_ (property),
            undo_ (undo)
    {
        // Don't attach an invalid valuetree!
        jassert (tree_.isValid());
        button_ = button;

        if (tree_.hasProperty (property)) {
            button_->setToggleState (tree_.getProperty(property_), juce::NotificationType::sendNotificationAsync);
        }
        else {
            tree_.setProperty (property, button_->getToggleState(), undo_);
        }

        tree_.addListener (this);
        button_->addListener (this);
    }

    ~ValueTreeButtonAttachment () override
    {
        tree_.removeListener (this);
        if (button_) {
            button_->removeListener (this);
        }
    }

    void buttonClicked (juce::Button *button) override
    {
        std::scoped_lock lock{mutex_};

        if (button_ == button) {
            tree_.setProperty (property_, button_->getToggleState(), undo_);
        }
    }

    void valueTreePropertyChanged (juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &changedProperty) override
    {
        std::scoped_lock lock{mutex_};

        if (treeWhosePropertyHasChanged == tree_ && button_) {
            if (changedProperty == property_) {
                button_->setToggleState (tree_.getProperty(property_), juce::NotificationType::sendNotificationAsync);
            }
        }
    }

private:
    juce::ValueTree                             tree_;
    juce::Component::SafePointer<juce::Button>  button_;
    juce::Identifier                            property_;
    juce::UndoManager*                          undo_ = nullptr;
    std::mutex                                  mutex_;
};