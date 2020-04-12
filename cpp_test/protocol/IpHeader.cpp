#ifndef IPHeader_H
#define IPHeader_H

#include <stdint.h>
#include <iostream>
#include <string.h> 
#include <bitset>

#define IPHeader_protocolUDP 17
#define IPHeader_protocolTCP 6


//forward declarations
namespace IP{
	/**
 * wikipedia page of ipv4 explains this well 
 **/
	struct Header{
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
	
};
bool IPHeader_checkChecksum(char *array, int length);
int IPHeader_calcChecksum(char * array, int length);

namespace IP{
	
	void parseLongIpAddress(long ip, char retu[4]);
	int getPayloadIndex(IP::Header *h);
	int getLength(IP::Header *h);
	void logValues(IP::Header * h);
	long IpAddressToLong(unsigned char ip[4]);
	
	
	
		namespace {
		
		int formatShort(int sht){
			return ((sht&0xff)<<8)  |  ((sht&0xff00)>>8); 
		}
		
	}
	
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
	
	int getPayloadIndex(IP::Header *h){ return ((h->headerLengthIn32bit)*4); }
	
	unsigned long createIpAddress(char one, char two, char three, char four){
		return 
			((one&0xff)<<24) 
			| 
			((two&0xff)<<16) 
			| 
			((three&0xff)<<8) 
			| 
			(four&0xff);
	}
	
	void setSourceIPAddress(IP::Header * self, long val){
		self->sourceIP = val;
		/*
		char ip[4];
		parseLongIpAddress(val, ip);
		self->sourceIP[0] = ip[0];
		self->sourceIP[1] = ip[1];
		self->sourceIP[2] = ip[2];
		self->sourceIP[3] = ip[3];
		* */
	}
	
	void setDestinationIPAddress(IP::Header *self, long val){
		self->destinationIP = val;
		/*
		char ip[4];
		parseLongIpAddress(val, ip);
		self->destinationIP[0] = ip[0];
		self->destinationIP[1] = ip[1];
		self->destinationIP[2] = ip[2];
		self->destinationIP[3] = ip[3];
		*/
	}
	
	void setDestinationIp(IP::Header *self, unsigned char ip[4]){
	
		unsigned long ipv4 = IP::IpAddressToLong(ip);
		/*
		self->destinationIP[0] = ip[0];
		self->destinationIP[1] = ip[1];
		self->destinationIP[2] = ip[2];
		self->destinationIP[3] = ip[3];
		* */
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
	
	
	long IpAddressToLong(unsigned char ip[4]){
		return createIpAddress(ip[0], ip[1], ip[2], ip[3]);
	}
	
	void copyVals( IP::Header * to, IP::Header * from){
		memcpy(to, from, IP::getPayloadIndex(from));
	}
	
	void copyToResponseHeader(IP::Header * response , IP::Header *message){
			copyVals(response, message);
			response->sourceIP = message->destinationIP;
			//memcpy(response->sourceIP,message->destinationIP, 4);
			response->destinationIP = message->sourceIP;
			//memcpy(response->destinationIP,message->sourceIP, 4);
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
	
	void setPayload(IP::Header * self, char * payload, int payloadLength){
		char *self_payload = ((char *)self)+ getPayloadIndex(self);
		memcpy(self_payload, payload, payloadLength);
		
		int len = payloadLength + IP::getPayloadIndex(self);
		self->totalLength = formatShort(len);
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
	
	int getLength(IP::Header *h){
		return formatShort(h->totalLength);
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
	return IP::formatShort((~retu)&0xffff);
}
bool IPHeader_checkChecksum(char *array, int length){
	int sum = IPHeader_calcChecksum(array, length);
	return (sum == 0) || (sum == 0xffff);
}


#endif
