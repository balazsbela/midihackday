//
//  PitchShifter.cpp
//  XYProcessor
//
//  Created by Carl Bussey on 06/05/2015.
//
//

#include "PitchShifter.h"

PitchShifter::PitchShifter()
:   m_nWindowSize(1024)
,   m_nHopSizeIn(m_nWindowSize/2)
,   m_aInputBuffer(m_nWindowSize)
,   m_aStretchedBuffer(m_nWindowSize * 2)
,   m_aOutputBuffer(m_nWindowSize)
,   m_aHanningWindow(0.0, m_nWindowSize)
,   m_aPreviousPhases(0.0, m_nWindowSize)
,   m_aOutputPhases(0.0, m_nWindowSize)
,   m_aOmegaR(0.0, m_nWindowSize)
,   m_nHopSizeOut(m_nHopSizeIn)
,   m_nSamplesSinceLastProcess(0)
,   m_aTimeDomainBuffer(std::complex<float>(), m_nWindowSize)
,   m_aFftDomainBuffer(std::complex<float>(), m_nWindowSize)
{
    m_aInputBuffer.enableAutoHopRead(true);
    m_aInputBuffer.setWriteOffset(m_nWindowSize - m_nHopSizeIn);
    // input buffer hopsize set in setRatio()
    
    m_aStretchedBuffer.enableAutoHopWrite(true);
    m_aStretchedBuffer.enableOverlapAdd(true);
    m_aStretchedBuffer.setHopRate(m_nWindowSize);
    m_aOutputBuffer.setReadOffset(m_nWindowSize - m_nHopSizeIn);
    
    setupFftw();
    setupHanningWindow();
    setupOmegaR();
    setRatio(1);
    
    std::cout << "PitchShifter initialised." << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////

PitchShifter::~PitchShifter()
{
    fftwf_destroy_plan(m_aFftFwdPlan);
    fftwf_destroy_plan(m_aFftBwdPlan);
}

////////////////////////////////////////////////////////////////////////////////////////
// Access
////////////////////////////////////////////////////////////////////////////////////////

void PitchShifter::setRatio(const float& fRatio)
{
    m_nHopSizeIn = floor((m_nHopSizeOut / fRatio) + 0.5);
    
    // m_nHopSizeOut = floor((m_nHopSizeIn * fRatio) + 0.5);
    m_fRatio = (float) m_nHopSizeOut / m_nHopSizeIn;
    // m_aInputBuffer.setWindowSize(m_fRatio * m_nWindowSize);
    m_aStretchedBuffer.setReadRate(m_fRatio);
    m_aInputBuffer.setHopSize(m_nHopSizeIn);
};

////////////////////////////////////////////////////////////////////////////////////////

void PitchShifter::process(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    
    size_t nNumSamples = buffer.getNumSamples();
    float* pAudioBuffer = buffer.getWritePointer(0);
    
    for (size_t nSample = 0; nSample < nNumSamples; ++nSample)
    {
        // Input audio
        m_aInputBuffer.write(pAudioBuffer[nSample]);
        
        // Output audio (delayed by m_nHopSizeIn samples)
        pAudioBuffer[nSample] = m_aOutputBuffer.read();
        
        ++m_nSamplesSinceLastProcess;
        if (m_nSamplesSinceLastProcess == m_nHopSizeIn)
        {
            m_nSamplesSinceLastProcess = 0;
            
            for (size_t nIndex = 0; nIndex < m_nWindowSize; ++nIndex)
            {
                size_t nCircularShiftedIndex = (nIndex + m_nWindowSize/2) % m_nWindowSize;
                
                float fValue = m_aInputBuffer.read();
                m_aTimeDomainBuffer[nCircularShiftedIndex].real(fValue * m_aHanningWindow[nCircularShiftedIndex]);
                m_aTimeDomainBuffer[nCircularShiftedIndex].imag(0);
            }
            
            // Do FFT
            std::fill(std::begin(m_aFftDomainBuffer), std::end(m_aFftDomainBuffer), std::complex<float>(0, 0));
            fftwf_execute(m_aFftFwdPlan);
            
            calculateOutputPhases();
            
            // Reverse FFT
            std::fill(std::begin(m_aTimeDomainBuffer), std::end(m_aTimeDomainBuffer), std::complex<float>(0, 0));
            fftwf_execute(m_aFftBwdPlan);
            
            // Overlap-add
            for (size_t nIndex = 0; nIndex < m_nWindowSize; ++nIndex)
            {
                size_t nCircularShiftedIndex = (nIndex + m_nWindowSize/2) % m_nWindowSize;
                
                m_aStretchedBuffer.write((m_aTimeDomainBuffer[nCircularShiftedIndex].real() / m_nWindowSize) * m_aHanningWindow[nCircularShiftedIndex]);
            }
            
            for (size_t nIndex = 0; nIndex < m_nHopSizeIn; ++nIndex)
            {
                m_aOutputBuffer.write(m_aStretchedBuffer.read());
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////
// Helpers
////////////////////////////////////////////////////////////////////////////////////////

float PitchShifter::princarg(const float fPhase)
{
    return fmod(fPhase+M_PI, -2*M_PI) + M_PI;
}

////////////////////////////////////////////////////////////////////////////////////////

void PitchShifter::calculateOutputPhases()
{
    // Calculate and set output phases
    for (size_t nIndex = 0; nIndex < m_nWindowSize; ++nIndex)
    {
        float fCurrentPhase = std::arg(m_aFftDomainBuffer[nIndex]);
        float fMagnitude = std::abs(m_aFftDomainBuffer[nIndex]);
        float fPhaseInDiff = m_aOmegaR[nIndex] + princarg(fCurrentPhase - m_aPreviousPhases[nIndex] - m_aOmegaR[nIndex]);
        
        m_aOutputPhases[nIndex] = princarg(m_aOutputPhases[nIndex] + fPhaseInDiff * m_fRatio);
        m_aFftDomainBuffer[nIndex] = std::polar(fMagnitude, m_aOutputPhases[nIndex]);
        m_aPreviousPhases[nIndex] = fCurrentPhase;
    }
}

////////////////////////////////////////////////////////////////////////////////////////

void PitchShifter::setupHanningWindow()
{
    for (size_t nIndex = 0; nIndex < m_nWindowSize; ++nIndex)
    {
        m_aHanningWindow[nIndex] = 0.5 * (1 - cos((2 * M_PI * nIndex)/(m_nWindowSize - 1)));
    }
}

////////////////////////////////////////////////////////////////////////////////////////

void PitchShifter::setupOmegaR()
{
    for (size_t nIndex = 0; nIndex < m_nWindowSize; ++nIndex)
    {
        m_aOmegaR[nIndex] = 2.0 * M_PI * nIndex * m_nHopSizeIn / m_nWindowSize;
    }
}

////////////////////////////////////////////////////////////////////////////////////////

void PitchShifter::setupFftw()
{
    m_aFftFwdPlan = fftwf_plan_dft_1d(m_nWindowSize, STD_TO_FFTW(m_aTimeDomainBuffer), STD_TO_FFTW(m_aFftDomainBuffer), FFTW_FORWARD, FFTW_MEASURE);
    m_aFftBwdPlan = fftwf_plan_dft_1d(m_nWindowSize, STD_TO_FFTW(m_aFftDomainBuffer), STD_TO_FFTW(m_aTimeDomainBuffer), FFTW_BACKWARD, FFTW_MEASURE);
    
    std::fill(std::begin(m_aTimeDomainBuffer), std::end(m_aTimeDomainBuffer), std::complex<float>(0, 0));
    std::fill(std::begin(m_aFftDomainBuffer), std::end(m_aFftDomainBuffer), std::complex<float>(0, 0));
}

////////////////////////////////////////////////////////////////////////////////////////