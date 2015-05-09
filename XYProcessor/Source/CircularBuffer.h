//
//  CircularBuffer.h
//  XYProcessor
//
//  Created by Carl Bussey on 06/05/2015.
//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __XYProcessor__CircularBuffer__
#define __XYProcessor__CircularBuffer__
#include <vector>

template <class T>
class CircularBuffer
{
public:
    // Lifetime
    CircularBuffer(size_t nWindowSize, size_t nMaxWindowSize = 0);
    
    // Access
    // Warning: don't change vector length
    std::vector<T>& getBuffer() { return m_aBuffer; };
    std::vector<T>& getContiguousBuffer();
    
    void write(T aValue);
    T read();
    
    void hopWrite();
    void hopRead();
    
    inline void enableAutoHopWrite(bool bAutoHop) { m_bAutoHopWrite = bAutoHop; };
    inline void enableAutoHopRead(bool bAutoHop) { m_bAutoHopRead = bAutoHop; };
    inline void enableOverlapAdd(bool bOverlapAdd) { m_bOverlapAdd = bOverlapAdd; };
    
    inline void setReadRate(float fReadRate) { m_fReadRate = fReadRate; };
    inline void setWindowSize(size_t nWindowSize);
    
    void setHopSize(size_t nHopSize);
    
    inline void setWriteOffset(size_t nOffset) { m_nWritePosition += nOffset; m_nWritePosition %= m_nWindowSize; };
    inline void setReadOffset(size_t nOffset) { m_nReadPosition += nOffset; m_nReadPosition %= m_nWindowSize; };
    
    void setHopRate(const size_t nHopRate);
    void setEnableDefaultHopRate(const bool bEnable) { m_bDefaultHopRate = bEnable; }
    
    void clear();
    
private:
    
    size_t m_nWindowSize;
    size_t m_nMaxWindowSize;
    
    std::vector<T> m_aBuffer;
    std::vector<T> m_aContiguousBuffer;
    size_t m_nHopSize;
    
    size_t m_nWritePosition;
    size_t m_nReadPosition;
    size_t m_nHoppedWritePosition;
    size_t m_nHoppedReadPosition;
    size_t m_nOverlapSize;
    
    bool m_bAutoHopWrite;
    bool m_bAutoHopRead;
    bool m_bOverlapAdd;
    
    size_t m_nWriteCounter;
    size_t m_nReadCounter;
    
    float m_fReadRate;
    float m_nHopRate;
    bool m_bDefaultHopRate;
    
    bool m_bDataHasChanged;
    
    // Helpers
    void incrementCursor(size_t& nCursor, const float fIncrement = 1);
};

//////////////////////////////////////////////////////////////////////////////////////////
// Lifetime
//////////////////////////////////////////////////////////////////////////////////////////

template <class T>
CircularBuffer<T>::CircularBuffer(size_t nWindowSize, size_t nMaxWindowSize)
:   m_nWindowSize(nWindowSize)
,   m_nMaxWindowSize(nMaxWindowSize && nMaxWindowSize >= nWindowSize ? nMaxWindowSize : nWindowSize)
,   m_aBuffer(m_nMaxWindowSize, 0)
,   m_aContiguousBuffer(m_nMaxWindowSize, 0)
,   m_nHopSize(0)
,   m_nWritePosition(0)
,   m_nReadPosition(0)
,   m_nHoppedWritePosition(0)
,   m_nHoppedReadPosition(0)
,   m_nOverlapSize(0)
,   m_bAutoHopWrite(false)
,   m_bAutoHopRead(false)
,   m_bOverlapAdd(false)
,   m_nWriteCounter(0)
,   m_nReadCounter(0)
,   m_fReadRate(1)
,   m_nHopRate(m_nWindowSize)
,   m_bDefaultHopRate(true)
,   m_bDataHasChanged(false)
{
    std::cerr << "CircularBuffer initialised." << std::endl;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Access
//////////////////////////////////////////////////////////////////////////////////////////

template <class T>
std::vector<T>& CircularBuffer<T>::getContiguousBuffer()
{
    if (m_bDataHasChanged)
    {
        size_t nTmpReadIndex = m_nHoppedReadPosition;
        
        for (size_t nIndex = 0; nIndex < m_nWindowSize; ++nIndex)
        {
            m_aContiguousBuffer[nIndex] = m_aBuffer[nTmpReadIndex];
            incrementCursor(nTmpReadIndex);
        }
        
        m_bDataHasChanged = false;
    }
    
    return m_aContiguousBuffer;
}

//////////////////////////////////////////////////////////////////////////////////////////

template<class T>
void CircularBuffer<T>::setWindowSize(size_t nWindowSize)
{
    if (nWindowSize <= m_nMaxWindowSize)
    {
        m_nWindowSize = nWindowSize;
    }
    else
    {
        std::cerr << "Error: the window size is larger than the selected maximum size."
        << std::endl;
    }
    
    if (m_bDefaultHopRate)
    {
        m_nHopRate = m_nWindowSize;
    }
    
    m_bDataHasChanged = true;
}

//////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void CircularBuffer<T>::write(T aValue)
{
    if (m_bOverlapAdd && m_nWriteCounter < m_nOverlapSize)
    {
        m_aBuffer[m_nWritePosition] += aValue;
    }
    else
    {
        m_aBuffer[m_nWritePosition] = aValue;
    }
        
    incrementCursor(m_nWritePosition);
    
    ++m_nWriteCounter;
    if(m_nWriteCounter == m_nWindowSize)
    {
        if (m_bAutoHopWrite)
        {
            hopWrite();
        }
        m_nWriteCounter = 0;
    }
    
    m_bDataHasChanged = true;
}

//////////////////////////////////////////////////////////////////////////////////////////

template <class T>
T CircularBuffer<T>::read()
{
    size_t nTmpReadPosition = m_nReadPosition;
    
    incrementCursor(m_nReadPosition, m_fReadRate);
    
    m_nReadCounter += m_fReadRate;
    if(m_nReadCounter >= m_nWindowSize)
    {
        if (m_bAutoHopRead)
        {
            hopRead();
        }
        m_nReadCounter %= m_nWindowSize;
    }
    
    return m_aBuffer[nTmpReadPosition];
}

//////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void CircularBuffer<T>::hopWrite()
{
    m_nHoppedWritePosition += m_nHopSize;
    m_nHoppedWritePosition %= m_nWindowSize;
    
    m_nWritePosition = m_nHoppedWritePosition;
}

//////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void CircularBuffer<T>::hopRead()
{
    m_nHoppedReadPosition += m_nHopSize;
    m_nHoppedReadPosition %= m_nWindowSize;
    
    m_nReadPosition = m_nHoppedReadPosition;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Helpers
//////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void CircularBuffer<T>::incrementCursor(size_t& nCursor, float fIncrement)
{
    nCursor += fIncrement;
    
    if (nCursor >= m_nWindowSize)
    {
        nCursor %= m_nWindowSize;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void CircularBuffer<T>::setHopRate(const size_t nHopRate)
{
    m_nHopRate = nHopRate;
    m_bDefaultHopRate = false;
}

//////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void CircularBuffer<T>::setHopSize(const size_t nHopSize)
{
    m_nHopSize = nHopSize;
    m_nOverlapSize = m_nWindowSize - m_nHopSize;
}

//////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void CircularBuffer<T>::clear()
{
    m_aBuffer.clear();
    m_aContiguousBuffer.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////

template class CircularBuffer<float>;

#endif /* defined(__XYProcessor__CircularBuffer__) */
