/*
    Example of two different ways to process received OSC messages using oscpack.
    Receives the messages from the SimpleSend.cpp example.
*/

#include "ReceiveOSC.h"
#include <fstream>


ReceiveOSC::ReceiveOSC():
    Thread("OscListener Thread"),
    incomingPort(12001),
    m_ipEndpoint(IpEndpointName::ANY_ADDRESS, incomingPort),
    s(m_ipEndpoint, this)
{
    std::cout << m_ipEndpoint.port << std::endl;
    
    DBG("Now called the Constructor");
}

void ReceiveOSC::ProcessMessage(const osc::ReceivedMessage& m,
    const IpEndpointName& /*remoteEndpoint*/)
{
  /*  std::ofstream outfile;
    outfile.open("output.txt",  std::fstream::in | std::fstream::out | std::fstream::app);
    outfile << m.AddressPattern() << std::endl;*/

    auto args = m.ArgumentStream();
    auto path = m.AddressPattern();

    float x,y,z = 0;

    if (std::strcmp(path, "/acc") == 0 )
    {
        args >> x >> y >> z >> osc::EndMessage;
        //outfile << x << " " << y << " " << z << std::endl;
    }
    else if (std::strcmp(path, "/ori") == 0 )
    {
        osc::int32 nx,ny,nz;
        args >> nx >> ny >> nz >> osc::EndMessage;
        //outfile << nx << " " << ny << " " << nz << std::endl;

        x = nx;
        y = ny;
        z = nz;
    }
    else if (std::strcmp(path, "/touch") == 0)
    {
        args >> x >> y >> osc::EndMessage;
        //outfile << x << " " << y << std::endl;
    }

    //outfile.close();

    SensorEvent event(path, x, y, z);

    if (m_eventQueue)
    {
        m_eventQueue->push(event);
    }

}
