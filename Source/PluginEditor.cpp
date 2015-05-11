/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
XyprocessorAudioProcessorEditor::XyprocessorAudioProcessorEditor (XyprocessorAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter)
    , m_eventQueue(std::make_shared<std::queue<SensorEvent> >())
{
    // This is where our plugin's editor size is set.
    setSize (400, 300);
    
    m_receiveOSC.setEventQueue(m_eventQueue);
    ownerFilter->setEventQueue(m_eventQueue);
    
    // start osc thread
    m_receiveOSC.startThread();
}

XyprocessorAudioProcessorEditor::~XyprocessorAudioProcessorEditor()
{
}

//==============================================================================
void XyprocessorAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::white);
    g.setColour (Colours::black);
    g.setFont (15.0f);
    g.drawFittedText ("[Insert awesome GUI here]",
                      0, 0, getWidth(), getHeight(),
                      Justification::centred, 1);
}
