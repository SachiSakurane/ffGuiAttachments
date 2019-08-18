//
// Created by Himatya on 2019-08-15.
//

#pragma once

#include <mutex>
#include <utility>

class ValueTreeButtonAttachment : public juce::Button::Listener,
                                  public juce::ValueTree::Listener
{
public:
    ValueTreeButtonAttachment (juce::ValueTree& tree,
                               juce::Identifier property,
                               juce::Button& button,
                               juce::UndoManager* undo = nullptr)
        :   tree_ (tree),
            button_ (button),
            property_ (std::move(property)),
            undo_ (undo)
    {
        // Don't attach an invalid valuetree!
        jassert (tree_.isValid());

        if (tree_.hasProperty (property_))
        {
            button_.setToggleState (tree_.getProperty(property_), juce::NotificationType::dontSendNotification);
        }
        else
        {
            tree_.setProperty (property_, button_.getToggleState(), undo_);
        }

        tree_.addListener (this);
        button_.addListener (this);
    }

    ~ValueTreeButtonAttachment () override
    {
        tree_.removeListener (this);
        button_.removeListener (this);
    }

    void buttonClicked (juce::Button *button) override
    {
        if (std::unique_lock lock{mutex_, std::try_to_lock}; lock)
        {
            if (&button_ == button)
            {
                tree_.setProperty(property_, button_.getToggleState(), undo_);
            }
        }
    }

    void valueTreePropertyChanged (juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &changedProperty) override
    {
        if (std::unique_lock lock{mutex_, std::try_to_lock}; lock)
        {
            if (treeWhosePropertyHasChanged == tree_)
            {
                if (changedProperty == property_)
                {
                    button_.setToggleState (tree_.getProperty(property_), juce::NotificationType::sendNotificationAsync);
                }
            }
        }
    }

private:
    juce::ValueTree&   tree_;
    juce::Button&      button_;
    juce::Identifier   property_;
    juce::UndoManager* undo_;
    std::mutex         mutex_;
};