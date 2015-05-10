//
//  Modulator.cpp
//  XYProcessor
//
//  Created by Carl Bussey on 10/05/2015.
//
////////////////////////////////////////////////////////////////////////////////

#include "Modulator.h"

Modulator::Modulator()
:   m_fMix(0)
{}

////////////////////////////////////////////////////////////////////////////////

void Modulator::process(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    processEvents(midiMessages);
    processAudio(buffer);
}

////////////////////////////////////////////////////////////////////////////////

void Modulator::processAudio(juce::AudioSampleBuffer &buffer)
{
    float* pAudio = buffer.getWritePointer(0);
    
    for (size_t nIndex = 0; nIndex < buffer.getNumSamples(); ++nIndex)
    {
        float fDry = pAudio[nIndex];
        float fWet = (sin(2*M_PI*m_nSampleIndex/m_nSampleRate)* m_fMix + 1-m_fMix)*fDry;
        pAudio[nIndex] = fWet;
        
        ++m_nSampleIndex;
        if (m_nSampleIndex >= m_nSampleRate)
        {
            m_nSampleIndex = 0;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Modulator::processEvents(juce::MidiBuffer &midiMessages)
{
    MidiMessage aMessage;
    int nSampleIndex = 0;
    
    MidiBuffer::Iterator it(midiMessages);
    while (it.getNextEvent(aMessage, nSampleIndex))
    {
        ++nSampleIndex;
        
        if (aMessage.isController() && aMessage.getControllerNumber() == CC_MOD)
        {
            int nValue = aMessage.getControllerValue();
            float fMix = 64 - nValue;
            fMix = fMix >= 0 ? fMix : 0;
            fMix = fMix / 63 * 0.8;
            
            setMix(fMix);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////