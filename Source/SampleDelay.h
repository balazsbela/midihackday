//
//  SampleDelay.h
//  XYProcessor
//
//  Created by Carl Bussey on 10/05/2015.
//
//

#ifndef __XYProcessor__SampleDelay__
#define __XYProcessor__SampleDelay__

#include "CircularBuffer.h"
#include "Processor.h"

#define CC_DELAY 21

class SampleDelay : public Processor
{
public:
    SampleDelay();
    void process(AudioSampleBuffer& buffer, MidiBuffer& midiMessages) override;
    
    void setFeedback(const float fFeedback) { m_fFeedback = fFeedback; };
    
private:
    CircularBuffer<float> m_aDelayedSamples;
    size_t m_nDelay;
    float m_fFeedback;
    
    void processAudio(AudioSampleBuffer& buffer);
    void processEvents(MidiBuffer& midiMessages);
};

#endif /* defined(__XYProcessor__SampleDelay__) */
