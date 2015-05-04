/* 
    Example of two different ways to process received OSC messages using oscpack.
    Receives the messages from the SimpleSend.cpp example.
*/

#include "ReceiveOSC.h"
#include <fstream>


ReceiveOSC::ReceiveOSC():
	Thread("OscListener Thread"),
	incomingPort(PORT),
	s(IpEndpointName(IpEndpointName::ANY_ADDRESS, incomingPort),
	this)
{

	DBG("Now called the Constructor");
}

void ReceiveOSC::ProcessMessage(const osc::ReceivedMessage& m,
	const IpEndpointName& remoteEndpoint)
{
	(void)remoteEndpoint; // suppress unused parameter warning

	std::cout << m.AddressPattern();
	
	std::ofstream outfile;
	outfile.open("output.txt");
	outfile << m.AddressPattern();
	outfile.close();

}