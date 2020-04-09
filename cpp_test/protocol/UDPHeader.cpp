#ifndef UDPPacket_H
#define UDPPacket_H

#include "ethernetHeader.cpp"
#include <iostream>
#include <bitset>
#include "../CrashReporter.cpp"

#define UDP_Port_Multicast 5353

namespace UDP{
	struct Header{
		unsigned int sourcePort:16;//be carefull because the byte order might be off
		unsigned int destPort:16;//be carefull because the byte order might be off
		unsigned int length:16;//length of UDP header + payload.length
		unsigned int checksum:16; //the checksum of the entire header+ payload
	};
}

namespace UDP{
	//forward declarations
	void logValues(UDP::Header *udp);
	int getLength(UDP::Header *src);
	
	namespace{
		
		int formatShort(int sht){
			return ((sht&0xff)<<8)  |  ((sht&0xff00)>>8); 
		}
		int calcChecksum(IP::Header *ip, UDP::Header *self){
			char temp[65536] = {0};
			
			unsigned char *ipin = ip->sourceIP;
			temp[0] = ipin[1];
			temp[1] = ipin[0];
			temp[2] = ipin[3];
			temp[3] = ipin[2];
			
			unsigned char *ipout = ip->destinationIP;
			temp[4] = ipout[1];
			temp[5] = ipout[0];
			temp[6] = ipout[3];
			temp[7] = ipout[2];
			
			
			temp[8] = 0;
			temp[9] = ip->protocol;
			
			
			int len = UDP::getLength(self);
			
			temp[10] = (len>>8)&0xff;
			temp[11] = len&0xff;
			
			
			
			char *tempe = (char *)self;
			for(int i = 0; i<len;i++){
				temp[12+i] = tempe[i];
			}
			
			return IPHeader_calcChecksum(temp, 65536 );
			
			
		}
		
	}
	bool checkChecksum(IP::Header *ip, UDP::Header *self){
			int sum = calcChecksum(ip, self);
			return  (sum == 0) || (sum == 0xffff);
	}
	int getLength(UDP::Header *src){
		return formatShort(src->length);
	}
	int getSourcePort(UDP::Header *src){ return formatShort(src->sourcePort); }
	
	int getDestPort(UDP::Header *src){ return formatShort(src->destPort); }
	
	UDP::Header *createEmptyHeader(IP::Header *src){
		return (UDP::Header *) 
		(
			((char *)src) + IP::getPayloadIndex(src)
		);
	}
	UDP::Header *create(IP::Header * src, const char *errLog){
		if(src->protocol != IPHeader_protocolUDP){
			std::cout<<"UDPHeader:15";
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
			std::cout<<"udp header checksum invalid at("<<errLog<<"):"<<formatShort(sum)<<":"<<bits<<"\n";
			std::cout<<"length swapped IP:"<<formatShort(src->totalLength)<<"\n";
			std::cout<<"length swapped UDP:"<<formatShort(retu->length)<<"\n";
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
		memcpy(self+payloadIndex, body, length);
		self->length = formatShort(payloadIndex+length);
		setChecksum(ip,self);
		if(!checkChecksum(ip, self)){
			std::cout<<"UDP failed to set own checksum:"<<self->checksum<<"\n";
			throw -7;
		}
		
	}
	
	void logValues(UDP::Header *udp){
		std::cout<<"udp header:\n"
		<<"sourcePort:"<< UDP::formatShort(udp->sourcePort)<<"\n"
		<<"destPort:"<< UDP::formatShort(udp->destPort) <<"\n"
		<<"length:"<< formatShort(udp->length) <<"\n"
		<<"checksum:"<< udp->checksum <<"\n";
	}
}
int UDPHeader_getPayloadIndex(IP::Header *tcp){
	int tcpIndex = IPHeader_getPayloadIndex(tcp);
	if(tcp->protocol == IPHeader_protocolUDP){
		return tcpIndex+ (16+16+16+16);
	}
	std::cout<<"error not a udp packet";
	throw -1;
}



#endif
