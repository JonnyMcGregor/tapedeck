#include "MainComponent.h"
#include "../Source/TPDK_LookAndFeel.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (1440, 800);
    juce::LookAndFeel::setDefaultLookAndFeel(&lf);

    tapedeck = std::make_unique<Tapedeck>(getWidth(), getHeight());
    addAndMakeVisible(tapedeck.get());
}

MainComponent::~MainComponent()
{
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
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
