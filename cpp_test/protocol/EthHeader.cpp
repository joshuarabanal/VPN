
#include <net/ethernet.h> 

namespace Eth{
	struct Header{
		unsigned char destinationMac[ETH_ALEN];
		unsigned char sourceMac[ETH_ALEN];
		__be16 type;
	} __attribute__ ((__packed__));;
	Eth::Header *create(char *src){
		return (Eth::Header *) (src);
	}
	void createResponseHeader(Eth::Header *destination, Eth::Header *source){
		memcpy(
			destination->destinationMac, 
			destination->sourceMac, 
			sizeof(Eth::Header::destinationMac)
		);
		memcpy(
			destination->sourceMac, 
			destination->destinationMac, 
			sizeof(Eth::Header::destinationMac)
		);
		destination -> type = source->type;
	}
	char *getPayload(Eth::Header *src){
		return (
			((char*)src) + sizeof(Eth::Header)
		);
	}
}
