#ifndef TCP__HANDSHAKE_t
#define TCP__HANDSHAKE_t

#include <cstdlib>
#include "../../protocol/TCPHeader.cpp"
#include "../../util/file.cpp"  
#include <stdlib.h> 

namespace TCP::Handshake{
	
	namespace {
		const long defaultWindowSize =2000;

	}

	TCP::Header *create_SYN_ACK(IP::Header *ip_out,TCP::Header *synRequest){
		Header * retu = TCP::createEmptyHeader(ip_out);
		
		retu->sourcePort = synRequest -> destPort;
		retu -> destPort = synRequest ->sourcePort;
		TCP::setSEQ(retu, rand() % 1000 );
		TCP::setACK(retu, TCP::getSEQ(synRequest)+1);
		retu -> dataoffset = 5;
		retu -> reserved = 0;
		
		retu -> NS = false;//ECN-nonce - concealment protection
		retu -> CWR = false;//Congestion window reduced
		retu -> ECE = false;//ECN-Ech
		retu -> URG = false;//Indicates that the Urgent pointer field is significant
		retu -> ACK = true;//Indicates that the Acknowledgment field is significant
		retu -> PSH = false;//Push function. Asks to push the buffered data to the receiving application.
		retu -> RST = false;//Reset the connection
		retu -> SYN = true;//Synchronize sequence numbers.
		retu -> FIN = false;//indicates this is the last packet and close connection
		retu -> windowSize = 0;//specifies the number of window size units[c] that the sender of this segment is currently willing to receive.
		
		retu -> urgentPointer = 0;
		
		TCP::setChecksum(ip_out, retu);
		return retu;
		
	}

}
#endif
