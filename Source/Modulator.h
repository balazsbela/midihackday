//
//  Modulator.h
//  XYProcessor
//
//  Created by Carl Bussey on 10/05/2015.
//
//

#ifndef __XYProcessor__Modulator__
#define __XYProcessor__Modulator__

#include "Processor.h"
#include <valarray>

#define CC_MOD 22

class Modulator : public Processor
{
public:
    void process(AudioSampleBuffer& buffer, MidiBuffer& midiMessages);
    
    Modulator();
    
    void setMix(const float fMix) { m_fMix = fMix; };
    void setSampleRate(const size_t nSampleRate) { m_nSampleRate = nSampleRate; };
    
private:
    float m_fMix;
    size_t m_nSampleRate;
    
    size_t m_nSampleIndex;
    
    void processAudio(AudioSampleBuffer& buffer);
    void processEvents(MidiBuffer& midiMessages);
    
};

#endif /* defined(__XYProcessor__Modulator__) */
