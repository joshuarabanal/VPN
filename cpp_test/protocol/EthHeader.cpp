#ifndef ETH__HEADER_t
#define ETH__HEADER_t

#include <net/ethernet.h> 
#include <iostream>
#include "../rawSocket/rawSocketMetaData.cpp"

namespace Eth{
	enum Type{
		ipv4 = 0x80
	};
	
	struct Header{
		unsigned char destinationMac[ETH_ALEN];
		unsigned char sourceMac[ETH_ALEN];
		int type:16;
	} __attribute__ ((__packed__));;
	
	void setType(Header *src, Type type){ src->type = 0x08; }
	void setSourceMac(Header *src, unsigned char mac[6]){
		unsigned char * us = src->sourceMac;
			us[0] = mac[0];
			us[1] = mac[1];
			us[2] = mac[2];
			us[3] = mac[3];
			us[4] = mac[4];
			us[5] = mac[5];
	}
	void setDestinationMac(Header *src, unsigned char mac[6]){
		unsigned char * us = src->destinationMac;
			us[0] = mac[0];
			us[1] = mac[1];
			us[2] = mac[2];
			us[3] = mac[3];
			us[4] = mac[4];
			us[5] = mac[5];
	}
	
	
	
	Eth::Header *create(char *src){
		return (Eth::Header *) (src);
	}
	Eth::Header *create(char *src, unsigned char sourceMac[6], unsigned char destMac[6], Type type ){
		Header *retu = create(src);
		Eth::setSourceMac(retu, sourceMac);
		Eth::setDestinationMac(retu, destMac);
		Eth::setType(retu,type);
		return retu;
	}
	void setDefaultSourceMac(Header *src, int socket, const char *interfaceName){
		unsigned char mac[6] = {0};
		Raw::getMacAddress(socket, interfaceName, mac);
		setSourceMac(src, mac);
		
	}
	void createResponseHeader(Eth::Header *destination, Eth::Header *source){
		unsigned char *source_source = source->sourceMac;
		setDestinationMac(destination, source_source);
			
		unsigned char *source_dest = source->destinationMac;
		setSourceMac(destination, source_dest);
		
		destination -> type = source->type;
	}
	int HeaderLength = sizeof(Eth::Header);
	char *getPayload(Eth::Header *src){
		return (
			((char*)src) + HeaderLength
		);
	}
	void logValues(Header *src){
		std::cout<<"logging the ethernet header:\n";
		std::cout<<"dest mac:";
		
		unsigned char *arry  = src->destinationMac;
		for(int i = 0; i<6; i++){ std::cout<<(int)arry[i]<<","; }
		std::cout<<"\n";
		
		std::cout<<"source mac:";
		arry  = src->sourceMac;
		for(int i = 0; i<6; i++){ std::cout<<(int)arry[i]<<","; }
		std::cout<<"\n";
		
		std::cout<<"payload type:"<<src->type<<"\n";
	}
}


#endif
