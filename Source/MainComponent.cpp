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
    
    // Initialise Command Manager
    commandManager = std::make_shared<juce::ApplicationCommandManager>();
    setApplicationCommandManagerToWatch(commandManager.get());
    commandManager->registerAllCommandsForTarget(this);
    addKeyListener(commandManager->getKeyMappings());

    // Initialise Tapedeck Model
    deviceManager = std::make_shared<juce::AudioDeviceManager>();
    tapedeckModel = make_shared<AudioManager>(deviceManager, false, "test_session");
    jassert(deviceManager != nullptr); //Device manager is not initialising properly, check the device logs...

    initialiseSession("test_session");

    // Initialise Tapedeck UI
    tapedeckUI = std::make_unique<Tapedeck>(getWidth(), getHeight() - 30, tapedeckModel->params->sampleRate, commandManager);
    mainMenuBar.reset(new juce::MenuBarComponent(this));
    
    
    // Start with 4 tracks...
    for (int i = 0; i < 4; i++) {
        perform(createTrack);
    }

    //Initialise Properties Window
    propertiesPanel.reset(new PropertiesPanel(deviceManager));
    propertiesWindow.reset(new PropertiesWindow(commandManager, "Properties", juce::Colours::black));
    propertiesWindow->setContentComponent(propertiesPanel.get());
    addChildComponent(propertiesWindow.get());

    addAndMakeVisible(mainMenuBar.get());
    addAndMakeVisible(tapedeckUI.get());
    startTimerHz(30);

    setSize(1440, 800);
}

MainComponent::~MainComponent()
{
    stopTimer();
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
    juce::FileLogger::setCurrentLogger(nullptr);
    delete logger; //Should probably use unique_ptr instead

    setApplicationCommandManagerToWatch(nullptr);
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

//======== Timer Function Overrides ==========//
void MainComponent::timerCallback()
{
    if (!tapedeckUI->getTrackStack()) {
        return;
    }
    // Update Playhead Position
    int currentTimeSamples = tapedeckModel->session->getCurrentTimeInSamples();
    tapedeckUI->getTrackStack()->updatePlayheadPosition(currentTimeSamples, false);
    //Update Master Meter Values
    tapedeckUI->masterMeter[0].setLevel(tapedeckModel->session->getOutputDB(0));
    tapedeckUI->masterMeter[1].setLevel(tapedeckModel->session->getOutputDB(1));
    tapedeckUI->masterMeter[0].repaint();
    tapedeckUI->masterMeter[1].repaint();
}

//======== MenuBar Function Overrides ==========//
juce::StringArray MainComponent::getMenuBarNames()
{
    return {"File", "Edit"};
}

juce::PopupMenu MainComponent::getMenuForIndex(int menuIndex, const juce::String &menuName)
{
    juce::PopupMenu menu;

    if (menuName == "File") {
        menu.addCommandItem(commandManager.get(), CommandIDs::loadSession);
        menu.addCommandItem(commandManager.get(), CommandIDs::saveSession);
        menu.addCommandItem(commandManager.get(), CommandIDs::togglePropertiesWindow);
    }
    if (menuName == "Edit") {
        menu.addCommandItem(commandManager.get(), CommandIDs::createTrack);
        menu.addCommandItem(commandManager.get(), CommandIDs::deleteSelectedTracks);
        menu.addCommandItem(commandManager.get(), CommandIDs::togglePlayback);
        menu.addCommandItem(commandManager.get(), CommandIDs::recordPlayback);
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
                            CommandIDs::togglePropertiesWindow,
                            CommandIDs::createTrack,
                            CommandIDs::deleteSelectedTracks,
                            CommandIDs::togglePlayback,
                            CommandIDs::recordPlayback,
                            CommandIDs::updatePlayheadInModel
    };
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
    case CommandIDs::createTrack:
        result.setInfo("Create Track", "Creates a new track", "Menu", 0);
        result.setTicked(!result.isTicked);
        result.addDefaultKeypress('t', juce::ModifierKeys::ctrlModifier);
        break;
    case CommandIDs::deleteSelectedTracks:
        result.setInfo("Delete Track", "Deletes the selected tracks", "Menu", 0);
        result.setTicked(!result.isTicked);
        result.addDefaultKeypress(juce::KeyPress::deleteKey, juce::ModifierKeys::ctrlModifier);
        break;
    case CommandIDs::togglePlayback:
        result.setInfo("Play", "Starts Playback", "Menu", 0);
        result.addDefaultKeypress(juce::KeyPress::spaceKey, juce::ModifierKeys::noModifiers);
        break;
    case CommandIDs::recordPlayback:
        result.setInfo("Record", "Starts recording any record armed tracks", "Menu", 0);
        result.setTicked(!result.isTicked);
        result.addDefaultKeypress(juce::KeyPress::spaceKey, juce::ModifierKeys::ctrlModifier);
        break;
    case CommandIDs::updatePlayheadInModel:
        result.setInfo("Update Playhead", "Updates the playhead in Model to match the UI position", "Internal", 0);
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
        //tapedeckModel->session->sessionXml->
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
    case CommandIDs::createTrack:
        tapedeckModel->session->createTrack();
        tapedeckUI->createTrackWidget(tapedeckModel->session->tracks.back());
        
        break;
    case CommandIDs::deleteSelectedTracks:
        deleteSelectedTracks();
        break;
    case CommandIDs::togglePlayback:
        if (tapedeckModel) {
            if (tapedeckModel->isPlaying) {
                tapedeckModel->stopAudioStream();
                tapedeckModel->isPlaying = false;
                tapedeckUI->updateTrackStack();
                for (auto trackWidget : tapedeckUI->getTrackStack()->trackWidgets) {
                        trackWidget->isRecording = false;
                    }
            }
            else {
                tapedeckModel->startAudioStream();
                tapedeckModel->isPlaying = true;
                for (auto trackWidget : tapedeckUI->getTrackStack()->trackWidgets) {

                    if (trackWidget->but_recordArm->getToggleState()) {
                        trackWidget->isRecording = true;
                    }
                }
            }
        }
        break;
    case CommandIDs::recordPlayback:
        break;
    case CommandIDs::updatePlayheadInModel: {
        int newTimeInSamples = tapedeckUI->getTrackStack()->timeRuler->xPositionToTimeInSamples(tapedeckUI->getTrackStack()->getPlayheadXPosition());
        tapedeckModel->session->setCurrentTimeInSamples(newTimeInSamples);
        break;
    }

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

void MainComponent::deleteSelectedTracks() {
    auto selectedTrackWidgets = tapedeckUI->getSelectedTracks();
    for (auto it = selectedTrackWidgets.begin(); it != selectedTrackWidgets.end(); it++) {
        selectedTrackWidgets.erase(it);
    }
    for (auto &track : tapedeckModel->session->getSelectedTracks()) {
        tapedeckModel->session->deleteTrack(track);
    }
}