#include "MainComponent.h"
#include "../Source/TPDK_LookAndFeel.h"

//==============================================================================
MainComponent::MainComponent()
{
    juce::LookAndFeel::setDefaultLookAndFeel(&lf);
    // Initialise logger for tracking app issues
    //juce::FileLogger *logger = juce::FileLogger::createDateStampedLogger("Tapedeck/logs", "TapedeckLogger_", "txt", "//========= TAPEDECK LOGS =========//");
    logger = juce::FileLogger::createDefaultAppLogger("Tapedeck/logs", "TapedeckLogger.txt", "//========= TAPEDECK LOGS =========//");
    juce::FileLogger::setCurrentLogger(logger);
    tapedeck = std::make_unique<Tapedeck>(getWidth(), getHeight() - 30);

    mainMenuBar.reset(new juce::MenuBarComponent(this));
    setApplicationCommandManagerToWatch(&commandManager);
    commandManager.registerAllCommandsForTarget(this);
    addKeyListener(commandManager.getKeyMappings());

    addAndMakeVisible(mainMenuBar.get());
    addAndMakeVisible(tapedeck.get());
    setSize(1440, 800);
}

MainComponent::~MainComponent()
{
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
    juce::FileLogger::setCurrentLogger(nullptr);
    delete logger; //Should probably use unique_ptr instead
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{

}

void MainComponent::resized()
{
    int curY = 0;
    if (mainMenuBar) {
        mainMenuBar->setBounds(0, curY, getWidth(), 30);
    }
    curY += 15;
    if(tapedeck)
    {
        tapedeck->setBounds(0, curY, getWidth(), getHeight() - 15);
    }
    else {
        //Do nothing, Tapedeck should be initialised in MainComponent constructor.
    }
}
//======== MenuBar Function Overrides ==========//
juce::StringArray MainComponent::getMenuBarNames()
{
    return {"File"};
}

juce::PopupMenu MainComponent::getMenuForIndex(int menuIndex, const juce::String &menuName)
{
    juce::PopupMenu menu;

    if (menuIndex == 0) {
        menu.addCommandItem(&commandManager, CommandIDs::loadSession);
        menu.addCommandItem(&commandManager, CommandIDs::saveSession);
        menu.addCommandItem(&commandManager, CommandIDs::openPropertiesWindow);
    }
    
    return menu;
}

void MainComponent::menuItemSelected(int menuItemID, int topLevelMenuIndex)
{
}

// ======= Application Command Target Function Overrides =========//

 juce::ApplicationCommandTarget *MainComponent::getNextCommandTarget()
{
    return nullptr;
}

void MainComponent::getAllCommands(juce::Array<juce::CommandID> &commandIDs)
{
    juce::Array<juce::CommandID> commands{CommandIDs::loadSession,
                              CommandIDs::saveSession,
                              CommandIDs::openPropertiesWindow};
    commandIDs.addArray(commands);
}

void MainComponent::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &result)
{
    switch (commandID) {
    case CommandIDs::loadSession:
        result.setInfo("Open", "Loads a Tapedeck session from an xml file", "Menu", 0);
        result.addDefaultKeypress('l', juce::ModifierKeys::ctrlModifier);
        break;
    case CommandIDs::saveSession:
        result.setInfo("Save As", "Saves Tapedeck session to an xml file", "Menu", 0);
        result.addDefaultKeypress('s', juce::ModifierKeys::ctrlModifier);
        break;
    case CommandIDs::openPropertiesWindow:
        result.setInfo("Properties", "Opens the properties menu", "Menu", 0);
        result.setTicked(!result.isTicked);
        result.addDefaultKeypress('p', juce::ModifierKeys::ctrlModifier);
        break;
    default:
        break;
    }
}

bool MainComponent::perform(const InvocationInfo &info)
{
    switch (info.commandID) {
    case CommandIDs::loadSession :
        break;
    case CommandIDs::saveSession:
        break;
    case CommandIDs::openPropertiesWindow:
        break;
    default:
        return false;
    }

    return true;
}