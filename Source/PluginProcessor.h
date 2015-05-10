/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PitchShifter.h"
#include "FrequencyShifter.h"
#include "SampleDelay.h"
#include "SensorEvent.h"
#include "Modulator.h"
#include <memory>

#include <queue>
#include <boost/smart_ptr.hpp>

//==============================================================================
/**
*/
class XyprocessorAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    XyprocessorAudioProcessor();
    ~XyprocessorAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();

    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);

    //==============================================================================
    AudioProcessorEditor* createEditor();
    bool hasEditor() const;

    //==============================================================================
    const String getName() const;

    int getNumParameters();

    float getParameter (int index);
    void setParameter (int index, float newValue);

    const String getParameterName (int index);
    const String getParameterText (int index);

    const String getInputChannelName (int channelIndex) const;
    const String getOutputChannelName (int channelIndex) const;
    bool isInputChannelStereoPair (int index) const;
    bool isOutputChannelStereoPair (int index) const;

    bool acceptsMidi() const;
    bool producesMidi() const;
    bool silenceInProducesSilenceOut() const;
    double getTailLengthSeconds() const;

    //==============================================================================
    int getNumPrograms();
    int getCurrentProgram();
    void setCurrentProgram (int index);
    const String getProgramName (int index);
    void changeProgramName (int index, const String& newName);

    //==============================================================================
    void getStateInformation (MemoryBlock& destData);
    void setStateInformation (const void* data, int sizeInBytes);

    void setEventQueue(boost::shared_ptr<std::queue<SensorEvent>> eventQueue)
    {
        m_eventQueue = eventQueue;
    }
    
private:
    std::unique_ptr<PitchShifter> m_pPitchShifter;
    std::unique_ptr<FrequencyShifter> m_pFrequencyShifter;
    std::unique_ptr<SampleDelay> m_pSampleDelay;
    std::unique_ptr<Modulator> m_pModulator;
    
    boost::shared_ptr<std::queue<SensorEvent>> m_eventQueue;
    
    void processOSCData(MidiBuffer& midiMessages);
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XyprocessorAudioProcessor)
};

#endif  // PLUGINPROCESSOR_H_INCLUDED
