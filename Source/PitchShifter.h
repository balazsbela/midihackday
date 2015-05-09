//
//  PitchShifter.h
//  XYProcessor
//
//  Created by Carl Bussey on 06/05/2015.
//
//

#ifndef __XYProcessor__PitchShifter__
#define __XYProcessor__PitchShifter__

#define STD_TO_FFTW(VECTOR) (fftwf_complex*) &VECTOR[0]

// User-defined

#include "Processor.h"
#include "CircularBuffer.h"

// STL

#include <valarray>
#include <complex>

// FFTW

#include "fftw3.h"

class PitchShifter : public Processor
{
public:
    PitchShifter();
    ~PitchShifter();
    
    void setRatio(const float& fRatio);
    
    void process(AudioSampleBuffer& buffer, MidiBuffer& midiMessages) override;
    
    
private:
    
    // typedefs
    
    using tFftwBuffer = std::valarray<std::complex<float>>;
    
    size_t m_nWindowSize;
    size_t m_nHopSizeIn;
    float m_fRatio;
    
    CircularBuffer<float> m_aInputBuffer;
    CircularBuffer<float> m_aStretchedBuffer;
    CircularBuffer<float> m_aOutputBuffer;
    
    std::valarray<float> m_aHanningWindow;
    std::valarray<float> m_aPreviousPhases;
    std::valarray<float> m_aOutputPhases;
    std::valarray<float> m_aOmegaR;
    
    size_t m_nHopSizeOut;
    size_t m_nSamplesSinceLastProcess;
    
    tFftwBuffer m_aTimeDomainBuffer;
    tFftwBuffer m_aFftDomainBuffer;
    fftwf_plan m_aFftFwdPlan;
    fftwf_plan m_aFftBwdPlan;
    
    float princarg(float fPhase);
    void calculateOutputPhases();
    void setupHanningWindow();
    void setupOmegaR();
    void setupFftw();
    
};

#endif /* defined(__XYProcessor__PitchShifter__) */
