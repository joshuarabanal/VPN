/**
 * use this file to debug format:
 * http://www.tcpipguide.com/free/t_DHCPMessageFormat.htm
 */
#ifndef DHCP_h
#define DHCP_h

#define DHCP_clientPort 68
#define DHCP_serverPort 67

//forward declarations
namespace DHCP{
	struct Header{
		char OPCode;
		char hardwareType;//1 for ethernet
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
		char magicCookie[4];//always the same value: 0x63, 0x82, 0x53, 0x63
	};
	
	enum OPCodeTypes{
		 request = 1,
		 reply = 2
	};
	
	int optionsIndex = sizeof(Header);
	
	void logValues(struct Header * self);
	
	void checkValidity(struct Header *self);
	
	int getTotalHeaderLength(DHCP::Header *self);
}

#include "IpHeader.cpp"
#include "UDPHeader.cpp"
#include <iostream>
#include <string.h>
#include "DHCP/Option.cpp"
#include <iostream>
#include <unistd.h>
#include <stdint.h>

/**
 * @param name = the name of the variable
 * @param dhcp = the dhcpPacket to fill the values with
 **/
//#define DHCP_new_OptionsArray(dhcp,name) name[10]; DHCP::fillOptionsValues(dhcp, name)



namespace DHCP{
	bool isMagicCookieValid(DHCP::Header * self){
		if(
				self->magicCookie[0] != 0x63 ||
				self->magicCookie[1] != 0x82 ||
				self->magicCookie[2] != 0x53 ||
				self->magicCookie[3] != 0x63
		){
			return false;
		}
		return true;
	}
	Header * createEmptyHeader(UDP::Header* src){
		Header *self =  (Header *) (((char *)src) + UDP::payloadIndex);
				self->magicCookie[0] = 0x63;
				self->magicCookie[1] = 0x82;
				self->magicCookie[2] = 0x53;
				self->magicCookie[3] = 0x63;
				return self;
	}
	Header * create(UDP::Header * src, const char *error){
		Header *retu =  (Header *) (((char *)src) + UDP::payloadIndex);
		if(!isMagicCookieValid(retu)){
			DHCP::logValues(retu);
			std::cout<<"invalid dhcp header\n";
			std::cout<<"stack hint:"<<error<<"\n";
			std::cout.flush();
			throw 15;
		}
		return retu;
	}
	
	void setOptions(Header * self, DHCP::Option * options, int optLength){
		//char magicCookie[4];//always the same value: 0x63, 0x82, 0x53, 0x63
		self->magicCookie[0] = 0x63;
		self->magicCookie[1] = 0x82;
		self->magicCookie[2] = 0x53;
		self->magicCookie[3] = 0x63;
		
		
		char * opt_out = ((char *)self) + DHCP::optionsIndex;
		bool has_endOption = false;
		for(int i = 0; i<optLength; i++){
			DHCP::Option opt_in = options[i];
			if(opt_in.type == DHCP::OPTIONS::types::end){
				has_endOption = true;
			}
			opt_out[0] = opt_in.type;
			opt_out[1] = opt_in.length; 
			opt_out+=2;
			memcpy(opt_out, opt_in.data, opt_in.length);
			opt_out+=opt_in.length;
		}
		if(!has_endOption){
			opt_out[0] = DHCP::OPTIONS::types::end;
			opt_out[1] = 0;
		}
		if(
				self->magicCookie[0] != 0x63 ||
				self->magicCookie[1] != 0x82 ||
				self->magicCookie[2] != 0x53 ||
				self->magicCookie[3] != 0x63
		){
			std::cout<<"FATAL ERROR:magic cookie not coorect:DHCP::HEADER::set options\n";
			throw 10;
		}
	}
	
	int getOptionsLengthInBytes(DHCP::Option * options, int optionsLength){
		int totalLength = 0;
		for(int i = 0; i<optionsLength; i++){
			totalLength += 2 + options[i].length;
		}
		return totalLength ;
	}
	
	int getTotalHeaderLength(DHCP::Header *self){
		
		int optionsLengths = 0;
		char * optchar = ((char *)self) +sizeof(DHCP::Header);
		std::cout<<"calculating options length:";
		while(true){
			DHCP::Option * opt = (DHCP::Option *)optchar;
			if(opt->type == 0){
				std::cout<<"FATAL ERROR: calculating the header length:DHCP::HEader::get total header length\n"; std::cout.flush();
				throw 19;
			}
			int len = (opt->length) +2;
			std::cout<<","<<opt->type; std::cout.flush();
			optchar += len;
			optionsLengths += len;
			if(opt->type == DHCP::OPTIONS::types::end){ break; }
		}
		std::cout<<"\n";
		return optionsLengths + sizeof(DHCP::Header);
	}
	
	void memCpy(Header *to, Header *from){
		memcpy(to, from,sizeof(Header));
	}
	
	void logValues(DHCP::Header * self){
		
		std::cout<<"DHCP Header:\nopcode:"<< (int)self->OPCode;
		std::cout<<"\nhardwareType:"<< (int)self->hardwareType;
		std::cout<<"\nhardwareAddressLength:"<< (int)self->hardwareAddressLength;
		std::cout<<"\nhops:"<< (int)self->hops;
		std::cout<<"\ntransactionId:"<< (long)self->transactionId;
		std::cout<<"\nseconds:"<< (int)self->seconds;
		std::cout<<"\nflags:"<< (int)self->flags;
		std::cout<<"\nclientIpAddress:"
			<<((self->clientIpAddress>>24)&0xff)<<","<<((self->clientIpAddress>>16)&0xff)
			<<","<<((self->clientIpAddress>>8)&0xff)<<","<<((self->clientIpAddress)&0xff);
		std::cout<<"\nyourIpAddress:"
			<<((self->yourIpAddress>>24)&0xff)<<","<<((self->yourIpAddress>>16)&0xff)
			<<","<<((self->yourIpAddress>>8)&0xff)<<","<<((self->yourIpAddress)&0xff);
		std::cout<<"\nserverIpAddress:"
			<<((self->serverIpAddress>>24)&0xff)<<","<<((self->serverIpAddress>>16)&0xff)
			<<","<<((self->serverIpAddress>>8)&0xff)<<","<<((self->serverIpAddress)&0xff);
		std::cout<<"\ngatewayIpAddress:"
			<<((self->gatewayIpAddress>>24)&0xff)<<","<<((self->gatewayIpAddress>>16)&0xff)
			<<","<<((self->gatewayIpAddress>>8)&0xff)<<","<<((self->gatewayIpAddress)&0xff);
		std::cout<<"\nclientHardwareAddress:"<<(self->clientHardwareAddress);
		std::cout<<"\nServerName:"<< (char *)self->ServerName;
		std::cout<<"\nbootFileName:"<< (char *)self->bootFileName;
		std::cout<<"\nmagicCookie:"<<(int)(self->magicCookie[0])<<","<<(int)(self->magicCookie[1])<<","<<(int)(self->magicCookie[2])<<","<<(int)(self->magicCookie[3]);
		std::cout<<"\noptions:\n";
		std::cout.flush();
		DHCP::OPTIONS::logValuesFromHeader(self);
		std::cout<<"\n";
		std::cout.flush();
		
	}
	
	void checkValidity(Header *self){
		if(
			self->OPCode == 0 ||
			self->hardwareType != 1 || 
			self->hardwareAddressLength == 0
		){
			std::cout<<"invalid DHCP header, all values possibly null\n logging raw values:\n"<<sizeof(Header);
			char *buffer = (char *)self;
			for(unsigned int i = 0; i<sizeof(Header)+1; i++){
				std::cout<<(int)buffer[i]<<",";
			}
			throw -1;
		}
	}
}

#endif
