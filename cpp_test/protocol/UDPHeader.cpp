#ifndef UDPPacket_H
#define UDPPacket_H

#include "IpHeader.cpp"
#include <iostream>
#include <bitset>
#include "../CrashReporter.cpp"


namespace UDP{
	struct Header{
		unsigned int sourcePort:16;//be carefull because the byte order might be off
		unsigned int destPort:16;//be carefull because the byte order might be off
		unsigned int length:16;//length of UDP header + payload.length
		unsigned int checksum:16; //the checksum of the entire header+ payload
	};
}

namespace UDP{
	
	namespace CommonPorts{
		const int DNS = 53; 
		const int Multicast = 5353;
		const int UPnP = 1900;
		const int MultimediaConferenceControl = 5050;
	}
	//forward declarations
	void logValues(UDP::Header *udp);
	int getTotalLength(UDP::Header *src);
	
	namespace{
		
		
		int calcChecksum(IP::Header *ip, UDP::Header *self){
			char temp[0xffff] = {0};
			
			unsigned long ipin = ip->sourceIP;
			temp[0] = (ipin)& 0xff;
			temp[1] = (ipin>>8)&0xff;
			temp[2] = (ipin>>16)&0xff;
			temp[3] = (ipin>>24)&0xff;;
			
			unsigned long ipout = ip->destinationIP;
			temp[4] = (ipout)&0xff;
			temp[5] = (ipout>>8)&0xff;
			temp[6] = (ipout>>16)&0xff;
			temp[7] = (ipout>>24)&0xff;
			
			
			temp[8] = 0;
			temp[9] = ip->protocol;
			
			
			int len = UDP::getTotalLength(self);
			
			temp[10] = (len>>8)&0xff;
			temp[11] = len&0xff;
			
			
			
			char *tempe = (char *)self;
			for(int i = 0; i<len;i++){
				temp[12+i] = tempe[i];
			}
			
			return IPHeader_calcChecksum(temp,13+len);
			
			
		}
		
	}
	bool checkChecksum(IP::Header *ip, UDP::Header *self){
			int sum = calcChecksum(ip, self);
			return  (sum == 0) || (sum == 0xffff);
	}
	int headerLength = sizeof(UDP::Header);
	int getTotalLength(UDP::Header *src){	return NetworkEndian::formatShort(src->length); }
	int getPayloadLength(UDP::Header *src){ return getTotalLength(src) - headerLength; }
	
	int getSourcePort(UDP::Header *src){ return NetworkEndian::formatShort(src->sourcePort); }
	
	int getDestPort(UDP::Header *src){ return NetworkEndian::formatShort(src->destPort); }
	
	UDP::Header *createEmptyHeader(IP::Header *src){
		return (UDP::Header *) 
		(
			((char *)src) + IP::getPayloadIndex(src)
		);
	}
	UDP::Header *create(IP::Header * src, const char *errLog){
		if(src->protocol != IP::protocol::UDP){
			std::cout<<"cant open udp connection on non udp packet:"<<src->protocol<<"\n";
			std::cout<<"UDPHeader:15\n";
			IP::logValues(src);
			throw -1;
		}
		UDP::Header * retu =  createEmptyHeader(src);
		bool check = checkChecksum(src,retu);
		if(!check){
			CrashReporter::logCrash((char *)src, 65536);
			int sum = calcChecksum(src, retu);
			IP::logValues(src);
			std::cout<<"\n\n\n";
			logValues(retu);
			std::bitset<16> bits(sum);
			std::cout<<"udp header checksum invalid at("<<errLog<<"):"<<NetworkEndian::formatShort(sum)<<":"<<bits<<"\n";
			std::cout<<"length swapped IP:"<<NetworkEndian::formatShort(src->totalLength)<<"\n";
			std::cout<<"length swapped UDP:"<<NetworkEndian::formatShort(retu->length)<<"\n";
			std::cout<<"test:"<<(int)(((char*)src)[0]&0x0f)<<"\n";
			char * read = (char *)src;
			for(int i = 0; i<IP::getLength(src); i+=16){
				
					std::cout<<(int)read[i]<<","<<(int)read[i+1]
						<<","<<(int)read[i+2]<<","<<(int)read[i+3];
						
					std::cout<<"\t\t"<<(int)read[i+4]<<","<<(int)read[i+5]
								<<","<<(int)read[i+6]<<","<<(int)read[i+7];
						
					std::cout<<"\t\t"<<(int)read[i+8]<<","<<(int)read[i+9]
								<<","<<(int)read[i+10]<<","<<(int)read[i+11];
								
					std::cout<<"\t\t"<<(int)read[i+12]<<","<<(int)read[i+13]
								<<","<<(int)read[i+14]<<","<<(int)read[i+15]<<"\n";
			}
			std::cout.flush();
			throw -2;
		}
		return retu;
	}
	
	int payloadIndex = sizeof(UDP::Header);
	char *getPayload(UDP::Header *self){
		return ((char *)self) + payloadIndex;
	}
	void copyVals( UDP::Header * to, UDP::Header * from){
		memcpy(to, from, payloadIndex);
	}
	
	void copyToResponseHeader(UDP::Header * response , UDP::Header *message){
			copyVals(response, message);
			response->sourcePort = message->destPort;
			response->destPort = message->sourcePort;
	}
	
	void setChecksum(IP::Header * ip, UDP::Header *self){
		self->checksum = 0;
		self->checksum = calcChecksum(ip,self);
		if(!UDP::checkChecksum(ip, self)){
			std::cout<<"failed to set own checksum\n"; std::cout.flush();
			throw -4;
		}
	}
	
	void setPayload(UDP::Header *self, IP::Header * ip, char *body, int length){
		char *udpChar = (char *)self;
		memcpy(udpChar+payloadIndex, body, length);
		self->length = NetworkEndian::formatShort(payloadIndex+length);
		setChecksum(ip,self);
		if(!checkChecksum(ip, self)){
			std::cout<<"UDP failed to set own checksum:"<<self->checksum<<"\n";
			throw -7;
		}
		
	}
	
	void logValues(UDP::Header *udp){
		std::cout<<"udp header:\n"
		<<"sourcePort:"<< NetworkEndian::formatShort(udp->sourcePort)<<"\n"
		<<"destPort:"<< NetworkEndian::formatShort(udp->destPort) <<"\n"
		<<"length:"<< NetworkEndian::formatShort(udp->length) <<"\n"
		<<"checksum:"<< udp->checksum <<"\n";
	}
}
int UDPHeader_getPayloadIndex(IP::Header *tcp){
	int tcpIndex = IP::getPayloadIndex(tcp);
	if( tcp->protocol == IP::protocol::UDP ){
		return tcpIndex+ (16+16+16+16);
	}
	std::cout<<"error not a udp packet";
	throw -1;
}



#endif
