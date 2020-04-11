
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
		unsigned char *dest_dest = destination->destinationMac;
		unsigned char *source_source = source->sourceMac;
			dest_dest[0] = source_source[0];
			dest_dest[1] = source_source[1];
			dest_dest[2] = source_source[2];
			dest_dest[3] = source_source[3];
			dest_dest[4] = source_source[4];
			dest_dest[5] = source_source[5];
			
			
		unsigned char *des_source = destination->sourceMac;
		unsigned char *source_dest = source->destinationMac;
			des_source[0] = source_dest[0];
			des_source[1] = source_dest[1];
			des_source[2] = source_dest[2];
			des_source[3] = source_dest[3];
			des_source[4] = source_dest[4];
			des_source[5] = source_dest[5];
		
		
		destination -> type = source->type;
	}
	char *getPayload(Eth::Header *src){
		return (
			((char*)src) + sizeof(Eth::Header)
		);
	}
}
