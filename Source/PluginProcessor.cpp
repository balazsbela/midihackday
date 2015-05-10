/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
XyprocessorAudioProcessor::XyprocessorAudioProcessor()
:   m_pPitchShifter(new PitchShifter)
,   m_pFrequencyShifter(new FrequencyShifter)
,   m_pSampleDelay(new SampleDelay)
,   m_pModulator(new Modulator)
{
}

XyprocessorAudioProcessor::~XyprocessorAudioProcessor()
{
}

//==============================================================================
const String XyprocessorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int XyprocessorAudioProcessor::getNumParameters()
{
    return 0;
}

float XyprocessorAudioProcessor::getParameter (int index)
{
    return 0.0f;
}

void XyprocessorAudioProcessor::setParameter (int index, float newValue)
{
}

const String XyprocessorAudioProcessor::getParameterName (int index)
{
    return String::empty;
}

const String XyprocessorAudioProcessor::getParameterText (int index)
{
    return String::empty;
}

const String XyprocessorAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String XyprocessorAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool XyprocessorAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool XyprocessorAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool XyprocessorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool XyprocessorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool XyprocessorAudioProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

double XyprocessorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int XyprocessorAudioProcessor::getNumPrograms()
{
    return 0;
}

int XyprocessorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void XyprocessorAudioProcessor::setCurrentProgram (int index)
{
}

const String XyprocessorAudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void XyprocessorAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void XyprocessorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    m_pModulator->setSampleRate(sampleRate);
}

void XyprocessorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void XyprocessorAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    
    processOSCData(midiMessages);
    m_pSampleDelay->process(buffer, midiMessages);
    m_pFrequencyShifter->process(buffer, midiMessages);
    m_pModulator->process(buffer, midiMessages);
    
    // In case we have more outputs than inputs, we'll clear any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    
    float* pMasterCopyBuffer = buffer.getWritePointer(0);
    for (int i = 1; i < getNumOutputChannels(); ++i)
    {
        float* pReplicaBuffer = buffer.getWritePointer(i);
        
        for (size_t nIndex = 0; nIndex < buffer.getNumSamples(); ++nIndex)
        {
            pReplicaBuffer[nIndex] = pMasterCopyBuffer[nIndex];
        }
    }
}

//==============================================================================
bool XyprocessorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* XyprocessorAudioProcessor::createEditor()
{
    return new XyprocessorAudioProcessorEditor (this);
}

//==============================================================================
void XyprocessorAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void XyprocessorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================

void XyprocessorAudioProcessor::processOSCData(MidiBuffer& midiMessages)
{
    if (m_eventQueue && !m_eventQueue->empty())
    {
        SensorEvent event = m_eventQueue->front();
        m_eventQueue->pop();
        
        {   // Process X
            int mX = event.x();
            mX = ((mX + 10.0) / 20.0) * 127;
        
            MidiMessage aControllerEvent = MidiMessage::controllerEvent (1, CC_FREQ_SHIFT, mX);
            midiMessages.addEvent(aControllerEvent, 0);
        }
        
        {   // Process Y
            int mY = event.y();
            mY = ((mY + 10.0) / 20.0) * 127;
            
            MidiMessage aControllerEvent = MidiMessage::controllerEvent (1, CC_DELAY, mY);
            midiMessages.addEvent(aControllerEvent, 1);
        }
        
        {   // Process Z
            int mZ = event.z();
            mZ = ((mZ + 10.0) / 20.0) * 127;
            
            MidiMessage aControllerEvent = MidiMessage::controllerEvent (2, CC_MOD, mZ);
            midiMessages.addEvent(aControllerEvent, 0);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new XyprocessorAudioProcessor();
}
