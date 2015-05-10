//
//  SampleDelay.cpp
//  XYProcessor
//
//  Created by Carl Bussey on 10/05/2015.
//
//

////////////////////////////////////////////////////////////////////////////////////////

#include "SampleDelay.h"

SampleDelay::SampleDelay()
:   m_aDelayedSamples(22050)
,   m_nDelay(22050)
,   m_fFeedback(0.5)
{
    m_aDelayedSamples.setWriteOffset(m_nDelay);
}

////////////////////////////////////////////////////////////////////////////////////////

void SampleDelay::process(juce::AudioSampleBuffer &buffer, juce::MidiBuffer &midiMessages)
{
    processEvents(midiMessages);
    processAudio(buffer);
}

////////////////////////////////////////////////////////////////////////////////////////

void SampleDelay::processAudio(juce::AudioSampleBuffer &buffer)
{
    float* pAudio = buffer.getWritePointer(0);
    
    for (size_t nIndex = 0; nIndex < buffer.getNumSamples(); ++nIndex)
    {
        float fDelaySample = m_aDelayedSamples.read();
        pAudio[nIndex] = pAudio[nIndex] + fDelaySample * m_fFeedback;
        m_aDelayedSamples.write(pAudio[nIndex]);
    }
}

////////////////////////////////////////////////////////////////////////////////////////

void SampleDelay::processEvents(juce::MidiBuffer &midiMessages)
{
    MidiMessage aMessage;
    int nSampleIndex = 0;
    
    MidiBuffer::Iterator it(midiMessages);
    while (it.getNextEvent(aMessage, nSampleIndex))
    {
        ++nSampleIndex;
        
        if (aMessage.isController() && aMessage.getControllerNumber() == CC_DELAY)
        {
            int nValue = aMessage.getControllerValue();
            float fFeedback = fabs((nValue - 64.0)) / 64.0 * 1.1;
            
            setFeedback(fFeedback);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////
