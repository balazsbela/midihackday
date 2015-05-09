//
//  FrequencyShifter.h
//  XYProcessor
//
//  Created by Carl Bussey on 06/05/2015.
//
//

#ifndef __XYProcessor__FrequencyShifter__
#define __XYProcessor__FrequencyShifter__

#define STD_TO_FFTW(VECTOR) (fftwf_complex*) &VECTOR[0]

// User-defined

#include "Processor.h"
#include "CircularBuffer.h"

// STL

#include <valarray>
#include <complex>

// FFTW

#include "fftw3.h"

class FrequencyShifter : public Processor
{
public:
    FrequencyShifter();
    ~FrequencyShifter();
    
    void process(AudioSampleBuffer& buffer, MidiBuffer& midiMessages) override;
    void setSampleShift(size_t nSampleShift) { m_nSampleShift = nSampleShift; };
    
private:
    
    // typedefs
    
    using tFftwBuffer = std::valarray<std::complex<float>>;
    
    size_t m_nWindowSize;
    size_t m_nHopSize;
    
    size_t m_nSampleShift;
    
    CircularBuffer<float> m_aInputBuffer;
    CircularBuffer<float> m_aOutputBuffer;
    std::vector<std::complex<float>> m_aPrevBinValues;
    
    std::valarray<float> m_aHanningWindow;
    
    size_t m_nSamplesSinceLastProcess;
    
    tFftwBuffer m_aTimeDomainBuffer;
    tFftwBuffer m_aFftDomainBuffer;
    fftwf_plan m_aFftFwdPlan;
    fftwf_plan m_aFftBwdPlan;
    
    void setupHanningWindow();
    void setupFftw();
    size_t circularShiftIndex(size_t nIndex);
    
    void shiftFrequencies();

};

#endif /* defined(__XYProcessor__FrequencyShifter__) */
