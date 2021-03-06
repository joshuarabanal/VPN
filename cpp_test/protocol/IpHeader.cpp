#ifndef IPHeader_H
#define IPHeader_H

#include <stdint.h>
#include <iostream>
#include <string.h> 
#include <bitset>
#include "./EthHeader.cpp"

//#define IPHeader_protocolUDP 17 replace with IP::protocol::UDP
//#define IPHeader_protocolTCP 6  replace with IP::protocol::TCP


//forward declarations
namespace IP{
	/**
 * wikipedia page of ipv4 explains this well 
 **/
	struct Header{
		//swapped due to byte order on raspberry pi
		unsigned int headerLengthIn32bit : 4; //number of 32bits in the header, if there are no options, then the length is 5
		unsigned int version :4;//ipv4 = 4
		
		unsigned int typeOfService:8;
		unsigned int totalLength:16;//length of payload and header
		unsigned int identification:16;
		unsigned int allFlags:16;
		unsigned int timeToLive:8;
		unsigned int protocol:8;//see define directives at top of page to see the protocol values
		unsigned int checksum:16;//calculated only using the ip header data
		unsigned long sourceIP:32;
		unsigned long destinationIP:32;
		
		char *optionsStart;
		//int32_t crc;
	};
	enum TypeOfService{
		ipv4 = 0x08
	};
	enum protocol{
		UDP = 17,
		TCP = 6
	};
	
};
bool IPHeader_checkChecksum(char *array, int length);
int IPHeader_calcChecksum(char * array, int length);

namespace NetworkEndian{
		int formatShort(int sht){
			return ((sht&0xff)<<8)  |  ((sht&0xff00)>>8); 
		}
		long formatLong(long lng){
			long one  = formatShort(lng & 0xffff);
			long two =  formatShort( (lng>>16)&0xffff );
			return (one<<16) | two; 
		}
}

namespace IP{
	
	void parseLongIpAddress(long ip, char retu[4]);
	int getPayloadIndex(IP::Header *h);
	void logValues(IP::Header * h);
	long IpAddressToLong(unsigned char ip[4]);
	
	/** @return header length + payloadLength **/
	int getLength(IP::Header *h){	return NetworkEndian::formatShort(h->totalLength);	}
	int getTotalLength(IP::Header *self){ return getLength(self); }
	int getHeaderSize(IP::Header *self){ return ((self->headerLengthIn32bit)*4); }
	int getPayloadLength(IP::Header *self){ return getLength(self) - getPayloadIndex(self);  }
	void setSourceIPAddress(IP::Header * self, long val){ self->sourceIP = val; }
	void setDestinationIPAddress(IP::Header *self, long val){	self->destinationIP = val;	}
	void setPayload(IP::Header * self, const char * payload, int payloadLength);
	
	
	
	IP::Header * createEmptyHeader(char *bytes){
		return (IP::Header *) bytes;
	}
	IP::Header * create(char * bytes, const char *log){
		IP::Header *retu =  createEmptyHeader(bytes);
		bool check = IPHeader_checkChecksum(bytes, getPayloadIndex(retu));
		if(retu->version != 4){
			std::cout<<"IP Header is invalid!\n helpful log:"<<log<<"\n";
			std::cout.flush();
			throw -69;
		}
		if(!check){
			logValues(retu);
			int sum = IPHeader_calcChecksum(bytes, getPayloadIndex(retu));
			std::bitset<16> bits(sum);
			std::cout<<"Ip checksum:"<<sum<<"\n";
			std::cout<<"IpHeader checksum failed at: "<<log<<"\nsum bits:"<<bits<<"\n";
			std::cout.flush();
			throw -2;
		}
		return retu;
	}
	
	Header * create(Eth::Header *src, long destIp, long srcIp, int packetId, protocol prot){
		IP::Header *self = createEmptyHeader(Eth::getPayload(src));
		IP::setDestinationIPAddress(self, destIp);
		IP::setSourceIPAddress(self, srcIp);
		self->typeOfService = TypeOfService::ipv4;
		self->version = 4;
		self->identification = packetId;
		self->allFlags = 0;
		self->timeToLive = 64;
		self->protocol= prot;
		self->headerLengthIn32bit = 5;
		
		IP::setPayload(self, "", 0);
		return self;
	}
	
	int getPayloadIndex(IP::Header *h){ return getHeaderSize(h); }
	
	unsigned long createIpAddress(char one, char two, char three, char four){
		return 
			((four&0xff)<<24) 
			| 
			((three&0xff)<<16) 
			| 
			((two&0xff)<<8) 
			| 
			(one&0xff);
	}
	
	
	void setDestinationIp(IP::Header *self, unsigned char ip[4]){
		unsigned long ipv4 = IP::IpAddressToLong(ip);
		self->destinationIP = ipv4;
	}
	
	bool isSrcIp(IP::Header *self, int ip0, int ip1, int ip2, int ip3){
		unsigned long given = createIpAddress(ip0, ip1, ip2, ip3);
		unsigned long sip = self->sourceIP;
		return sip == given;
	}
	
	bool isDestIp(IP::Header *self, int ip0, int ip1, int ip2, int ip3){
		unsigned long given = createIpAddress(ip0, ip1, ip2, ip3);
		unsigned long dip = self->destinationIP;
		return dip == given;
	}
	
	void parseLongIpAddress(long ip,  char retu[4]){
		retu[0] = (ip>>24)&0xff;
		retu[1] = (ip>>16)&0xff;
		retu[2] = (ip>>8)&0xff;
		retu[3] = (ip&0xff);
	}
	
	long IpAddressToLong(unsigned char ip[4]){	return createIpAddress(ip[0], ip[1], ip[2], ip[3]);	}
	
	void copyVals( IP::Header * to, IP::Header * from){
		memcpy(to, from, IP::getPayloadIndex(from));
	}
	
	void copyToResponseHeader(IP::Header * response , IP::Header *message){
			copyVals(response, message);
			response->sourceIP = message->destinationIP;
			response->destinationIP = message->sourceIP;
	}
	
	bool checkChecksum(IP::Header *self){
		return IPHeader_checkChecksum((char *)self, getPayloadIndex(self));
	}
	
	void setChecksum(IP::Header * self){
		char *self_payload = (char *)(self);
		int headerLength = getPayloadIndex(self);
		self->checksum = 0;
		self->checksum = IPHeader_calcChecksum(self_payload, headerLength);
		
		
	}
	
	void setPayload(IP::Header * self, const char * payload, int payloadLength){
		char *self_payload = ((char *)self)+ getPayloadIndex(self);
		memcpy(self_payload, payload, payloadLength);
		
		int len = payloadLength + IP::getPayloadIndex(self);
		self->totalLength = NetworkEndian::formatShort(len);
		setChecksum(self);
	}
	
	void logValues(IP::Header * h){
		std::cout<<"Ip header:\n"
		<<"version:"<<h->version<<"\n"
		<<"headerLengthIn32bit:"<<h->headerLengthIn32bit<<"\n"
		<<"typeOfService:"<<h->typeOfService<<"\n"
		<<"totalLength:"<<getLength(h)<<"\n"
		<<"identification:"<<h->identification<<"\n"
		<<"flags:"<<h->allFlags<<"\n"
		<<"timeToLive:"<<h->timeToLive<<"\n"
		<<"protocol:"<<h->protocol<<"\n"
		<<"checksum:"<<h->checksum<<"\n"
		
		
		<<"sourceIP:"<<((int)(h->sourceIP&0xff))
				<<","<<((int)((h->sourceIP>>8)&0xff))
				<<","<<((int) ((h->sourceIP>>16)&0xff))
				<<","<<((int)((h->sourceIP>>24)&0xff))<<"\n"
		
		<<"destinationIP:"<<((int)(h->destinationIP&0xff))
				<<","<<((int)((h->destinationIP>>8)&0xff))
				<<","<<((int) ((h->destinationIP>>16)&0xff))
				<<","<<((int)((h->destinationIP>>24)&0xff))<<"\n";
	}
	
	
}

namespace IP::Util{
	void logIpAddress( long ip ){
		std::cout
				<<((int)(ip&0xff))
				<<","<<((int)((ip>>8)&0xff))
				<<","<<((int) ((ip>>16)&0xff))
				<<","<<((int)((ip>>24)&0xff));
	}
}

int IPHeader_calcChecksum(char * array, int length){
	uint32_t sum = 0;
	for(int i = 0; i+1<length; i+=2){
		uint32_t temp =  
			(  (array[i]<<8) &  0xff00  );
		temp +=
			(   array[i+1]   &  0xff    );
		sum+=temp;
	}
	unsigned long retu = (sum&0xffff) + ((sum>>16)&0xffff);
	return NetworkEndian::formatShort((~retu)&0xffff);
}
bool IPHeader_checkChecksum(char *array, int length){
	int sum = IPHeader_calcChecksum(array, length);
	return (sum == 0) || (sum == 0xffff);
}


#endif
