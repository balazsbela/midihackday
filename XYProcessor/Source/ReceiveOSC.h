
#ifndef RECEIVEOSC_H_INCLUDED
#define RECEIVEOSC_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include <iostream>
#include <cstring>
#include <cstdlib>


// OSC send includes
#include "osc/OscOutboundPacketStream.h"
#include "ip/IpEndpointName.h"

// OSC receive includes
#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"

// OSC shared includes
#include "ip/UdpSocket.h"
#include "ip/PacketListener.h"

#include "SensorEvent.h"

#include <memory>

#include <boost/lockfree/queue.hpp>

#define PORT 8338
class ReceiveOSC : public osc::OscPacketListener,
    public Thread
{
public:
    // Constructor
    ReceiveOSC();//, MainContentComponent* const owner); //from mlrVSTAudioProcessor * const owner);
    ~ReceiveOSC()
    {
        // stop the OSC Listener thread running
        s.AsynchronousBreak();

        // allow the thread 2 seconds to stop cleanly - should be plenty of time.
        stopThread(2000);
    }

    // Start the oscpack OSC Listener Thread
    // NOTE: s.Run() won't return unless we force it to with
    // s.AsynchronousBreak() as is done in the destructor
    void run()
    {
        DBG("calling run");
        s.Run();
    }

    void setEventQueue(boost::lockfree::queue<SensorEvent>& eventQueue)
    {
        m_eventQueue = &eventQueue;
    }

private:
    int incomingPort;
    UdpListeningReceiveSocket s;

    boost::lockfree::queue<SensorEvent>* m_eventQueue;

protected:
    virtual void ProcessMessage(const osc::ReceivedMessage& m,
        const IpEndpointName& remoteEndpoint) override;

};
#endif  // RECEIVEOSC_H_INCLUDED
