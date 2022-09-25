#include "MainComponent.h"
#include "../Source/TPDK_LookAndFeel.h"

//==============================================================================
MainComponent::MainComponent()
{
    juce::LookAndFeel::setDefaultLookAndFeel(&lf);
    setWantsKeyboardFocus(true);
    // Initialise logger for tracking app issues
    //juce::FileLogger *logger = juce::FileLogger::createDateStampedLogger("Tapedeck/logs", "TapedeckLogger_", "txt", "//========= TAPEDECK LOGS =========//");
    
    logger = juce::FileLogger::createDefaultAppLogger("Tapedeck/logs", "TapedeckLogger.txt", "//========= TAPEDECK LOGS =========//");
    juce::FileLogger::setCurrentLogger(logger);
    
    // Initialise Tapedeck Model
    deviceManager = std::make_shared<juce::AudioDeviceManager>();
    tapedeckModel = make_shared<AudioManager>(deviceManager, false, "test_session");
    jassert(deviceManager != nullptr); //Device manager is not initialising properly, check the device logs...

    initialiseSession("test_session");

    // Initialise Tapedeck UI
    tapedeckUI = std::make_unique<Tapedeck>(getWidth(), getHeight() - 30, tapedeckModel->params->sampleRate);
    mainMenuBar.reset(new juce::MenuBarComponent(this));
    // Initialise Command Manager
    commandManager = std::make_shared<juce::ApplicationCommandManager>();
    setApplicationCommandManagerToWatch(commandManager.get());
    commandManager->registerAllCommandsForTarget(this);
    addKeyListener(commandManager->getKeyMappings());

    //Initialise Properties Window
    propertiesPanel.reset(new PropertiesPanel(deviceManager));
    propertiesWindow.reset(new PropertiesWindow(commandManager, "Properties", juce::Colours::black));
    propertiesWindow->setContentComponent(propertiesPanel.get());
    addChildComponent(propertiesWindow.get());

    addAndMakeVisible(mainMenuBar.get());
    addAndMakeVisible(tapedeckUI.get());
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
    curY += 15; //should be 30 but for some reason 30 creates a gap between tapedeck widget and top menu
    if(tapedeckUI)
    {
        tapedeckUI->setBounds(0, curY, getWidth(), getHeight() - 15);
    }
    if (propertiesWindow->isVisible()) {
        propertiesWindow->setBounds(80, 80, 700, 700);
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
        menu.addCommandItem(commandManager.get(), CommandIDs::loadSession);
        menu.addCommandItem(commandManager.get(), CommandIDs::saveSession);
        menu.addCommandItem(commandManager.get(), CommandIDs::togglePropertiesWindow);
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
                              CommandIDs::togglePropertiesWindow};
    commandIDs.addArray(commands);
}

void MainComponent::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &result)
{
    switch (commandID) {
    case CommandIDs::loadSession:
        result.setInfo("Open", "Loads a Tapedeck session from an xml file", "Menu", 0);
        result.addDefaultKeypress('o', juce::ModifierKeys::ctrlModifier);
        break;
    case CommandIDs::saveSession:
        result.setInfo("Save As", "Saves Tapedeck session to an xml file", "Menu", 0);
        result.addDefaultKeypress('s', juce::ModifierKeys::ctrlModifier);
        break;
    case CommandIDs::togglePropertiesWindow:
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
    case CommandIDs::togglePropertiesWindow:
        if (propertiesWindow->isVisible()) {
            propertiesWindow->setVisible(false);
        }
        else {
            propertiesWindow->setVisible(true);
            resized();
        }
        break;
    default:
        return false;
    }

    return true;
}
// ======= Command Functions =========//

void MainComponent::initialiseSession(std::string sessionName)
{
    if (filesystem::exists(sessionName)) {
        // Here we should add check for setting up loading functionality
        // For now to test the project we'll clear out the directory and start from scratch
        filesystem::remove_all(sessionName);
    }
    tapedeckModel->session->sessionXml->createXmlDocument();
}