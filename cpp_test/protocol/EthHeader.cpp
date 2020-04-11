
#include <net/ethernet.h> 
#include <iostream>
#include "../rawSocket/rawSocketMetaData.cpp"

namespace Eth{
	struct Header{
		unsigned char destinationMac[ETH_ALEN];
		unsigned char sourceMac[ETH_ALEN];
		__be16 type;
	} __attribute__ ((__packed__));;
	Eth::Header *create(char *src){
		return (Eth::Header *) (src);
	}
	void setSourceMac(Header *src, unsigned char mac[6]){
		unsigned char * us = src->sourceMac;
			us[0] = mac[0];
			us[1] = mac[1];
			us[2] = mac[2];
			us[3] = mac[3];
			us[4] = mac[4];
			us[5] = mac[5];
	}
	void setDefaultSourceMac(Header *src, int socket, const char *interfaceName){
		unsigned char mac[6] = {0};
		Raw::getMacAddress(socket, interfaceName, mac);
		setSourceMac(src, mac);
		
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
	
	void createResponseHeader(Eth::Header *destination, Eth::Header *source){
		unsigned char *source_source = source->sourceMac;
		setDestinationMac(destination, source_source);
			
		unsigned char *source_dest = source->destinationMac;
		setSourceMac(destination, source_dest);
		
		destination -> type = source->type;
	}
	char *getPayload(Eth::Header *src){
		return (
			((char*)src) + sizeof(Eth::Header)
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
