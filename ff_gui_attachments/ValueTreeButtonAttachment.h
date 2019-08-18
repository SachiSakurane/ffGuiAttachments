//
// Created by Himatya on 2019-08-15.
//

#pragma once

#include <mutex>

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

        if (tree_.hasProperty (property))
        {
            button_->setToggleState (tree_.getProperty(property_), juce::NotificationType::dontSendNotification);
        }
        else
        {
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
        if (std::unique_lock lock{mutex_, std::try_to_lock}; lock)
        {
            if (button_ == button)
            {
                tree_.setProperty(property_, button_->getToggleState(), undo_);
            }
        }
    }

    void valueTreePropertyChanged (juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &changedProperty) override
    {
        if (std::unique_lock lock{mutex_, std::try_to_lock}; lock)
        {
            if (treeWhosePropertyHasChanged == tree_ && button_)
            {
                if (changedProperty == property_)
                {
                    button_->setToggleState (tree_.getProperty(property_), juce::NotificationType::sendNotificationAsync);
                }
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