#include <stdint.h>
#include <iostream>
#include <string.h>

#define IPHeader_protocolUDP 17
#define IPHeader_protocolTCP 6

#ifndef IPHeader_H
#define IPHeader_H


//forward declarations


/**
 * wikipedia page of ipv4 explains this well 
 **/
struct IPHeader{
	unsigned int headerLengthIn32bit : 4;
	unsigned int version :4;
	unsigned int typeOfService:8;
	unsigned int totalLength:16;
	unsigned int identification:16;
	unsigned int flags:3;
	unsigned int fragOffset:13;
	unsigned int timeToLive:8;
	unsigned int protocol:8;
	unsigned int checksum:16;
	unsigned char sourceIP[4];
	unsigned char destinationIP[4];
	
	char *optionsStart;
	//int32_t crc;
};

int IPHeader_getPayloadIndex(IPHeader *h);

namespace IPPacket{
	long createIpAddress(char one, char two, char three, char four){
		return ((one&0xff)<<24) | ((two&0xff)<<16) | ((three&0xff)<<8) || (four&0xff);
	}
	void parseLongIpAddress(long ip, char retu[4]){
		retu[0] = (ip>>24)&0xff;
		retu[1] = (ip>>16)&0xff;
		retu[2] = (ip>>8)&0xff;
		retu[3] = (ip&0xff);
	}
	void copyVals( IPHeader * to, IPHeader * from){
		memcpy(to, from, IPHeader_getPayloadIndex(from));
	}
	void copyToResponseHeader(IPHeader * response , IPHeader *message){
			copyVals(response, message);
			memcpy(response->sourceIP,message->destinationIP, 4);
			memcpy(response->destinationIP,message->sourceIP, 4);
	}
	
}


void IPHeader_log(IPHeader * h){
	std::cout<<"Ip header:\n"
		<<"version:"<<h->version<<"\n"
		<<"headerLengthIn32bit:"<<h->headerLengthIn32bit<<"\n"
		<<"typeOfService:"<<h->typeOfService<<"\n"
		<<"totalLength:"<<h->totalLength<<"\n"
		<<"identification:"<<h->identification<<"\n"
		<<"flags:"<<h->flags<<"\n"
		<<"fragOffset:"<<h->fragOffset<<"\n"
		<<"timeToLive:"<<h->timeToLive<<"\n"
		<<"protocol:"<<h->protocol<<"\n"
		<<"checksum:"<<h->checksum<<"\n"
		
		
		<<"sourceIP:"<<((int)h->sourceIP[0])<<","<<((int)h->sourceIP[1])<<","<<((int)h->sourceIP[2])<<","<<((int)h->sourceIP[3])<<"\n"
		
		<<"destinationIP:"<<((int)h->destinationIP[0])<<","<<((int)h->destinationIP[1])<<","<<((int)h->destinationIP[2])<<","<<((int)h->destinationIP[3])<<"\n"
		;
}

bool IpHeader_validate(IPHeader * h){
	
	char * buffer = (char*) h;
	uint16_t sum = 0;
	for(int i = 0; i+1<IPHeader_getPayloadIndex(h); i++){
		int s = 
			(buffer[i] &0xff)<<8
			|
			(buffer[i+1] & 0xff)
		;
		sum += s;
	}
	std::cout<<"checksum:"<<sum;
	
	
	return true;
}

int IPHeader_getPayloadIndex(IPHeader *h){
	return ((h->headerLengthIn32bit)*4);
}

#endif
