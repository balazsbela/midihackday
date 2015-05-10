//
//  FrequencyShifter.cpp
//  XYProcessor
//
//  Created by Carl Bussey on 06/05/2015.
//
//

#include "FrequencyShifter.h"

FrequencyShifter::FrequencyShifter()
:   m_nWindowSize(1024)
,   m_nHopSize(m_nWindowSize/2)
,   m_nSampleShift(0)
,   m_aInputBuffer(m_nWindowSize)
,   m_aOutputBuffer(m_nWindowSize)
,   m_aPrevBinValues(m_nWindowSize/2 + 1, std::complex<float>(0.0, 0.0))
,   m_aHanningWindow(0.0, m_nWindowSize)
,   m_nSamplesSinceLastProcess(0)
,   m_aTimeDomainBuffer(std::complex<float>(), m_nWindowSize)
,   m_aFftDomainBuffer(std::complex<float>(), m_nWindowSize)
{
    m_aInputBuffer.enableAutoHopRead(true);
    m_aInputBuffer.setHopSize(m_nHopSize);
    m_aInputBuffer.setWriteOffset(m_nWindowSize - m_nHopSize);
    
    m_aOutputBuffer.enableAutoHopWrite(true);
    m_aOutputBuffer.enableOverlapAdd(true);
    m_aOutputBuffer.setHopSize(m_nHopSize);
    m_aOutputBuffer.setReadOffset(m_nWindowSize - m_nHopSize);
    
    setupFftw();
    setupHanningWindow();
    
    std::cout << "FrequencyShifter initialised." << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////

FrequencyShifter::~FrequencyShifter()
{
    fftwf_destroy_plan(m_aFftFwdPlan);
    fftwf_destroy_plan(m_aFftBwdPlan);
}

////////////////////////////////////////////////////////////////////////////////////////
// Processing
////////////////////////////////////////////////////////////////////////////////////////

void FrequencyShifter::process(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    processEvents(midiMessages);
    processAudio(buffer);
}

////////////////////////////////////////////////////////////////////////////////////////

void FrequencyShifter::processAudio(AudioSampleBuffer& buffer)
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
        if (m_nSamplesSinceLastProcess == m_nHopSize)
        {
            m_nSamplesSinceLastProcess = 0;
            
            for (size_t nIndex = 0; nIndex < m_nWindowSize; ++nIndex)
            {
                size_t nCircularShiftedIndex = nIndex;
                
                float fValue = m_aInputBuffer.read();
                m_aTimeDomainBuffer[nCircularShiftedIndex].real(fValue * m_aHanningWindow[nCircularShiftedIndex]);
                m_aTimeDomainBuffer[nCircularShiftedIndex].imag(0);
            }
            
            // Do FFT
            std::fill(std::begin(m_aFftDomainBuffer), std::end(m_aFftDomainBuffer), std::complex<float>(0, 0));
            fftwf_execute(m_aFftFwdPlan);
            
            // Frequency domain processing
            shiftFrequencies();
            
            // Reverse FFT
            std::fill(std::begin(m_aTimeDomainBuffer), std::end(m_aTimeDomainBuffer), std::complex<float>(0, 0));
            fftwf_execute(m_aFftBwdPlan);
            
            for (size_t nIndex = 0; nIndex < m_nWindowSize; ++nIndex)
            {
                size_t nCircularShiftedIndex = nIndex;
                
                m_aOutputBuffer.write((m_aTimeDomainBuffer[nCircularShiftedIndex].real()
                    / m_nWindowSize));
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////

void FrequencyShifter::processEvents(MidiBuffer& midiMessages)
{
    MidiMessage aMessage;
    int nSampleIndex = 0;

    MidiBuffer::Iterator it(midiMessages);
    while (it.getNextEvent(aMessage, nSampleIndex))
    {
        ++nSampleIndex;
        
        if (aMessage.isController() && aMessage.getControllerNumber() == CC_FREQ_SHIFT)
        {
            int nValue = aMessage.getControllerValue();
            int nSampleShift = fabs((nValue - 64.0)) / 64.0 * 4.0; // Offer half range
                
            setSampleShift(nSampleShift);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////

void FrequencyShifter::shiftFrequencies()
{
    m_aPrevBinValues.clear();
    
    for (size_t nIndex = 1; nIndex <= m_nWindowSize / 2; ++nIndex)
    {
        m_aPrevBinValues[nIndex] = m_aFftDomainBuffer[nIndex];
        std::complex<float> fBinValue(0, 0);
        
        if (nIndex > 1 && nIndex >= m_nSampleShift && nIndex - m_nSampleShift <= m_nWindowSize/2)
        {
            fBinValue = m_aPrevBinValues[nIndex - m_nSampleShift];
        }
        
        m_aFftDomainBuffer[nIndex] = fBinValue;
        
        if (nIndex != m_nWindowSize / 2)
        {
            m_aFftDomainBuffer[m_nWindowSize - nIndex] = std::conj(fBinValue);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////
// Helpers
////////////////////////////////////////////////////////////////////////////////////////

void FrequencyShifter::setupHanningWindow()
{
    for (size_t nIndex = 0; nIndex < m_nWindowSize; ++nIndex)
    {
        m_aHanningWindow[nIndex] = 0.5 * (1 - cos((2 * M_PI * nIndex)/(m_nWindowSize - 1)));
    }
}

////////////////////////////////////////////////////////////////////////////////////////

void FrequencyShifter::setupFftw()
{
    m_aFftFwdPlan = fftwf_plan_dft_1d(m_nWindowSize, STD_TO_FFTW(m_aTimeDomainBuffer),
        STD_TO_FFTW(m_aFftDomainBuffer), FFTW_FORWARD, FFTW_MEASURE);
    m_aFftBwdPlan = fftwf_plan_dft_1d(m_nWindowSize, STD_TO_FFTW(m_aFftDomainBuffer),
        STD_TO_FFTW(m_aTimeDomainBuffer), FFTW_BACKWARD, FFTW_MEASURE);
        
    std::fill(std::begin(m_aTimeDomainBuffer), std::end(m_aTimeDomainBuffer),
        std::complex<float>(0, 0));
    std::fill(std::begin(m_aFftDomainBuffer), std::end(m_aFftDomainBuffer),
        std::complex<float>(0, 0));
}

////////////////////////////////////////////////////////////////////////////////////////

size_t FrequencyShifter::circularShiftIndex(const size_t nIndex)
{
    return (nIndex + m_nWindowSize/2) % m_nWindowSize;
}

////////////////////////////////////////////////////////////////////////////////////////
