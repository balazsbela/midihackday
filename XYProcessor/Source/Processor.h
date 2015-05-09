//
//  Processor.h
//  XYProcessor
//
//  Created by Carl Bussey on 06/05/2015.
//
//

#ifndef __XYProcessor__Processor__
#define __XYProcessor__Processor__
#include "../JuceLibraryCode/JuceHeader.h"

class Processor
{
public:
    Processor() {};
    virtual ~Processor() {};
    
    virtual void process(AudioSampleBuffer& buffer, MidiBuffer& midiMessages) = 0;
    
};

#endif /* defined(__XYProcessor__Processor__) */
