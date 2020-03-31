#include <stdint.h>
#include <iostream>
#include "rawSocket.cpp"
#include "protocol/ethernetHeader.cpp"
#include "protocol/UDPHeader.cpp"

int main () { 
	std::cout << "Output sentence 1\n";
	RawSocket* sock;
	try{
		sock = new RawSocket(RawSocket_type_UDP, "eth0");
	}
	catch(int err){
		std::cout<<"error initializing socket:"<<err<<"\n";
		return 0;
	}
	char read[65536];
	sock->read(read);
	
	std::cout<<"\n\n\n\n";
	
	IPHeader *ip = (IPHeader *)read;
	IPHeader_log(ip);
	IpHeader_validate(ip);
	
	std::cout<<"\n\n\n\n";
	
	
	//log udp header
	int startUDP = IPHeader_getPayloadIndex(ip);
	std::cout<<"startUDP:"<<startUDP;
	
	
	UDPHeader *udp = (UDPHeader *) ( read+ startUDP );
	UDPHeader_fix(udp);
	UPDHeader_log(udp);
		
		std::cout<<"logging array:\n";
	//char * shortBuff = (uint16_t *) read;
	
	for(int i = 0; i<40; i+=4){
		std::cout << ((int) read[i] )<<","<<  ((int) read[i+1] ) <<","<< ((int) read[i+2] )<<","<< ((int) read[i+3] )<<"\n";
	}
	
	return 0;
}
