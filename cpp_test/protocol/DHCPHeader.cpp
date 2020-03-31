#include "ethernetHeader.cpp"
#include "UDPHeader.cpp"
#include <iostream>
#include <string.h>
#include "DHCP/Option.cpp"


/**
 * @param name = the name of the variable
 * @param dhcp = the dhcpPacket to fill the values with
 **/
#define DHCP_new_OptionsArray(dhcp,name) name[10]; DHCP::fillOptionsValues(dhcp, name)



namespace DHCP{

	enum OPCodeTypes{
		 discover = 1,
		offer = 2,
		request = 3,
		decline = 4,
		pack = 5,
		npack = 6,
		release = 7,
	};

	struct Header{
		char OPCode;
		char hardwareType;
		char hardwareAddressLength;
		char hops;
		int transactionId:32;
		int seconds:16;
		int flags:16;
		int clientIpAddress:32;
		int yourIpAddress:32;
		int serverIpAddress:32;
		int gatewayIpAddress:32;
		char clientHardwareAddress[16];
		char ServerName[64];
		char bootFileName[128];
		char magicCookie;
		char *optionsStartHere;
	};
/**
 * fills up the allocated Options field with the values from the DHCPHeader
 **/
void fillOptionsValues(Header *dhcp, DHCP::Option *opt){
	char * options = dhcp->optionsStartHere;
		int index = 0;
		while(options[0] != DHCP::OPTIONS::types::end){
			Option  *option = (Option *)options;
			opt[index] = *option;
			options++;//increment to skip over options type
			char length = options[0];
			options+= 1 + length;//1 for the length field and then the length itself
			index++;
		}
}
int getOptionIndexByType(DHCP::Option *opt, DHCP::OPTIONS::types type){
	int index = 0;
	while(opt[index].type != DHCP::OPTIONS::types::end){
		if(opt[index].type == type){
			return index;
		}
		index++;
		opt++;
	}
	return -1;
}
	int getLengthInBytes(DHCP::Option * options, int optionsLength){
		int totalLength = 0;
		for(int i = 0; i<optionsLength; i++){
			totalLength += 2 + options[i].length;
		}
		return totalLength + sizeOf(DHCP::Option);
	}
	void memCpy(Header *to, Header *from){
		memcpy(to, from,sizeof(Header));
	}
}
