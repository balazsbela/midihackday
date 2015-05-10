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
:   m_aDelayedSamples(2048)
,   m_nDelay(1024)
,   m_fFeedback(0.5)
{
    m_aDelayedSamples.setWriteOffset(m_nDelay);
}

////////////////////////////////////////////////////////////////////////////////////////

void SampleDelay::process(juce::AudioSampleBuffer &buffer, juce::MidiBuffer &midiMessages)
{
    processAudio(buffer);
    processEvents(midiMessages);
}

////////////////////////////////////////////////////////////////////////////////////////

void SampleDelay::processAudio(juce::AudioSampleBuffer &buffer)
{
    float* pAudio = buffer.getWritePointer(0);
    
    for (size_t nIndex = 0; nIndex < buffer.getNumSamples(); ++nIndex)
    {
        float fDelaySample = m_aDelayedSamples.read();
        m_aDelayedSamples.write(pAudio[nIndex]);
        pAudio[nIndex] = fDelaySample * m_fFeedback;
    }
}

////////////////////////////////////////////////////////////////////////////////////////

void SampleDelay::processEvents(juce::MidiBuffer &midiMessages)
{
    
}

////////////////////////////////////////////////////////////////////////////////////////
