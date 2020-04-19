
#include <cstdlib>
#include "../../protocol/TCPHeader.cpp"
#include "../../util/file.cpp"
#include "./Connection.cpp"

namespace TCP::Handshake{
	
	namespace {
		const long defaultWindowSize =2000;
		void respondToSYN(IP::Header *ip_in, TCP::Header *in, IP::Header *ip_out, TCP::Header *out){
			out->sourcePort = in->destPort;
			out->destPort = in->sourcePort;
			out->sequenceNumber = std::rand() % 0xffff;
			TCP::setACK(out,TCP::getSEQ(in)+1);
			out->dataoffset = 5;
			out->reserved = 0;
			out->NS = false;
			out->CWR = false;
			out->ECE = false;
			out->URG = false;
			out->ACK = false;
			out->PSH = false;
			out->RST = false;
			out->NS = false;
			out->SYN = false;
			out->FIN = false;
			TCP::setWindowSize(out, defaultWindowSize);
			out->urgentPointer = 0;
			TCP::setChecksum(ip_out,out);
			IP::setPayload( ip_out, (char *)out, TCP::getHeaderSize(out) );
		}

	}
	
	bool isHandshake(IP::Header *ip_in, TCP::Header *in, IP::Header *ip_out, TCP::Header *out, TCP::Connection *returnVal){
		
		if(in->SYN && !in->ACK ){
			respondToSYN(ip_in,in, ip_out, out);
			returnVal->initialize( in);
			FileIO::writeLogFile(
				"IP_tcp_handshake_syn.txt", 
				(char*)ip_in, 
				IP::getHeaderSize(ip_in)+TCP::getHeaderSize(in)
			);
			FileIO::writeLogFile(
				"IP_tcp_handshake_syn_ack.txt", 
				(char *)ip_out, 
				IP::getHeaderSize(ip_out)+TCP::getHeaderSize(out)
			);
			return true;
		}
		return false;
	}


}
