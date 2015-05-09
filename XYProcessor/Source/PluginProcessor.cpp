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
    return true;
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
//    for (int channel = 0; channel < getNumInputChannels(); ++channel)
//    {
//        float* channelData = buffer.getWritePointer (channel);
//        
//        // ..do something to the data...
//    }

    //m_pPitchShifter->process(buffer, midiMessages);
    
    processOSCData(midiMessages);
    
    m_pFrequencyShifter->process(buffer, midiMessages);
    
    // In case we have more outputs than inputs, we'll clear any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (int i = 1; i < getNumOutputChannels(); ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
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
    MidiMessage m;
    
    MidiBuffer output;
    
    //MidiMessageSequence sequence;
    
    
    while (m_eventQueue && !m_eventQueue->empty())
    {
        SensorEvent event("", 0.0f, 0.0f, 0.0f);
        m_eventQueue->pop(event);
        
        int mX = event.x();
        MidiMessage aPitchMidiEvent = MidiMessage::pitchWheel(1, mX);
        
        output.addEvent(aPitchMidiEvent, 0);
        output.addEvent(MidiMessage::noteOn(1, 65, 100.0f), (int)(midiMessages.getNumEvents() / 2));
        
    }
    
    midiMessages.clear();
    midiMessages = output;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new XyprocessorAudioProcessor();
}