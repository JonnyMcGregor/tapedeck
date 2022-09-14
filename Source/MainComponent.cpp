#include "MainComponent.h"
#include "../Source/TPDK_LookAndFeel.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (1440, 800);
    juce::LookAndFeel::setDefaultLookAndFeel(&lf);
    // Initialise logger for tracking app issues
    //juce::FileLogger *logger = juce::FileLogger::createDateStampedLogger("Tapedeck/logs", "TapedeckLogger_", "txt", "//========= TAPEDECK LOGS =========//");
    juce::FileLogger *logger = juce::FileLogger::createDefaultAppLogger("Tapedeck/logs", "TapedeckLogger.txt", "//========= TAPEDECK LOGS =========//");
    juce::FileLogger::setCurrentLogger(logger);
    tapedeck = std::make_unique<Tapedeck>(getWidth(), getHeight());
    addAndMakeVisible(tapedeck.get());
}

MainComponent::~MainComponent()
{
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
    juce::FileLogger::setCurrentLogger(nullptr);
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{

}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    if(tapedeck)
    {
        tapedeck->setBounds(0, 0, getWidth(), getHeight());
    }
}
