#include "ethernetHeader.cpp"
#include <iostream>

#ifndef UDPPacket_H
#define UDPPacket_H

struct UDPHeader{
		unsigned int sourcePort:16;
		unsigned int destPort:16;
		unsigned int length:16;
		unsigned int checksum:16;
		char *payLoad;
};
namespace UDPPacket{
	void copyVals( UDPHeader * to, UDPHeader * from){
		memcpy(to, from, sizeof(UDPHeader));
	}
	void copyToResponseHeader(UDPHeader * response , UDPHeader *message){
			copyVals(response, message);
			response->sourcePort = message->destPort;
			response->destPort = message->sourcePort;
	}
	void setPayload(UDPHeader *self,  char sourceIp[4], char destIp[4], char *body, int length){
		self->payLoad = body;
		self->length = 8+length;
		
		uint16_t sum = self->sourcePort + self->destPort + (self->length &0xff) + ((self->length>>8)&0xff);
		
		uint16_t *vals = (uint16_t *)destIp;
		sum+= vals[0] + vals[1];
		
		vals = (uint16_t *)sourceIp;
		sum+= vals[0] + vals[1];
		
		vals = body;
		for(int i = 0; i<length/2; i++){
			sum+= body[i];
		}
		self->checksum = ~sum;
	}
}
int UDPHeader_getPayloadIndex(IPHeader *tcp){
	int tcpIndex = IPHeader_getPayloadIndex(tcp);
	if(tcp->protocol == IPHeader_protocolUDP){
		return tcpIndex+ (16+16+16+16);
	}
	std::cout<<"error not a udp packet";
	throw -1;
}
void UDPHeader_fix( UDPHeader * udp){
	
	int temp = udp->sourcePort;
	udp->sourcePort = ((temp&0xff)<<8) | ((temp>>8)&0xff);
	
	temp = udp->destPort;
	udp->destPort = ((temp&0xff)<<8) | ((temp>>8)&0xff);
	
	temp = udp->length;
	udp->length = ((temp&0xff)<<8) | ((temp>>8)&0xff);
	
	temp = udp->checksum;
	udp->checksum = ((temp&0xff)<<8) | ((temp>>8)&0xff);
	
}

void UPDHeader_log(UDPHeader * udp){
	std::cout<<"udp header:\n"
		<<"sourcePort:"<< udp->sourcePort<<"\n"
		<<"destPort:"<< udp->destPort <<"\n"
		<<"length:"<< udp->length <<"\n"
		<<"checksum:"<< udp->checksum <<"\n";
}
#endif
