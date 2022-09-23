#pragma once

#include "UI/Widgets/Tapedeck.hpp"
#include "TPDK_LookAndFeel.h"
#include "defs_UI.h"
#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public juce::Component, public juce::MenuBarModel, public juce::ApplicationCommandTarget

{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...
    std::unique_ptr<Tapedeck> tapedeck;
    juce::FileLogger* logger;
    TPDK_LookAndFeel lf;
    
    //Use this to handle application commands
    juce::ApplicationCommandManager commandManager;
    //Top Menu bar component
    std::unique_ptr<juce::MenuBarComponent> mainMenuBar;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)

    // Inherited via MenuBarModel
    juce::StringArray getMenuBarNames() override;
    juce::PopupMenu getMenuForIndex(int topLevelMenuIndex, const juce::String &menuName) override;
    void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;

    // Implementation of ApplicationCommandTarget Interface
    ApplicationCommandTarget *getNextCommandTarget();
    void getAllCommands(juce::Array<juce::CommandID> &commands);
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &result);
    bool perform(const InvocationInfo &info) override;
};
