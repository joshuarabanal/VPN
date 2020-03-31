#include "../protocol/ethernetHeader.cpp"
#include "../protocol/UDPHeader.cpp"
#include "../protocol/DHCPHeader.cpp"
#include <iostream>
#include <string.h>

#define DHCP_Server_clientIP  IPPacket::createIpAddress(192, 168, 2,2)
#define DHCP_Server_SubnetMask  IPPacket::createIpAddress(192, 168, 2,0)
#define DHCP_Server_serverIP  IPPacket::createIpAddress(192, 168, 1,34)
#define DHCP_SERVER_AddrLeaseTime  (60*60*24)

namespace Server{
	
	void replyToDiscover(char *read, char *write){
		IPHeader * ip_in = (IPHeader *)read;
		UDPHeader * udp_in = (UDPHeader *)(read + UDPHeader_getPayloadIndex(ip_in));
		DHCP::Header *dhcp_in = (DHCP::Header *)udp_in->payLoad;
		DHCP::Option DHCP_new_OptionsArray(dhcp_in, options_in);//macro allocated new options array named options
		
		
		
		
		DHCP::Header *returnDHCP = {0};
		DHCP::memCpy(returnDHCP, dhcp_in);
        returnDHCP->OPCode = DHCP::OPCodeTypes::offer;
        //returnDHCP->hardwareType = dhcp_in->hardwareType;
        //returnDHCP->hardwareAddressLength = dhcp_in->hardwareAddressLength;
        //returnDHCP->hops = dhcp_in->hops;
        //returnDHCP->transactionId = dhcp_in->transactionId;
        //returnDHCP->seconds = dhcp_in->seconds;
        //returnDHCP->flags = dhcp_in->flags;
        //returnDHCP->clientIpAddress = dhcp_in->clientIpAddress;
        returnDHCP->yourIpAddress = DHCP_Server_clientIP;
        returnDHCP->serverIpAddress = DHCP_Server_serverIP;
        //returnDHCP->gatewayIpAddress = dhcp_in->gatewayIpAddress;
        memcpy(
			returnDHCP->clientHardwareAddress,
			dhcp_in->clientHardwareAddress,
			16*sizeof(char)
        );
        
        memcpy(
			returnDHCP->ServerName,
			dhcp_in->ServerName,
			64*sizeof(char)
        );
        
        memcpy(
			returnDHCP->bootFileName,
			dhcp_in->bootFileName,
			128*sizeof(char)
        );
        //returnDHCP->magicCookie = dhcp_in->magicCookie;
        //returnDHCP->gatewayIpAddress = dhcp_in->gatewayIpAddress;
        
        DHCP::Option options_out[6];
        
        //configure message type
        DHCP::OPTIONS::Message::create(options_out, DHCP::OPTIONS::Message::types::OFFER);
        
        //options[1] : subnetMask
        DHCP::OPTIONS::SubnetMask::create(options_out+1, DHCP_Server_SubnetMask);
        
        //options[2] = Router
        long vals[1] = {DHCP_Server_serverIP};
        DHCP::OPTIONS::Router::create(options_out+2, vals, 1);
        
        //options[3] = LeaseTime
        DHCP::OPTIONS::LeaseTime::create(options_out+3, DHCP_SERVER_AddrLeaseTime);
        
        //options[4] = ServerIdentifier
        DHCP::OPTIONS::ServerId::create(options_out+4, DHCP_Server_serverIP);
        
        //options[5] = DNSServer
        long dns[2] = {
              IPPacket::createIpAddress(8,8,8,8),
              IPPacket::createIpAddress(8,8,4,4)
		};
        DHCP::OPTIONS::DNS::create(options_out+5, dns, 2);
        
        
        
        IPHeader * ip_out = (IPHeader *)write;
        IPPacket::copyToResponseHeader(ip_out, ip_in);
        
        UDPHeader *udp_out = (UDPHeader *)(ip_out + IPHeader_getPayloadIndex(ip_out));
        UDPPacket::copyToResponseHeader(udp_out, udp_in);
		UDPPacket::setChecksum(ip_out->sourceIP, ip_out->destIp, returnDHCP, DHCP::getLengthInBytes(options_out, 6)) ;
        
        
        IPPacket::setPayload(udp_out, udp_out->length);
        
        /**
            
        byte[] retu = builder.build();//dhcp packet
        retu = udpe.build(ipe.getSourceIp(), ipe.getDestIp(), IpPacket_deprecated.payloadStartIndex+UdpPacketBuilder.payload_start_index+retu.length,retu );
        retu = ipe.build(retu);
        out.write(retu, udpe.getDestPort(), ipe.getDestIp());
        
        
        **/
	}
	void replyToRequest(char *bytes){}

	bool handleMessage(char *packet, char *responsePacket){
		IPHeader * ip = (IPHeader *)packet;
		UDPHeader * udp = (UDPHeader *)(packet + UDPHeader_getPayloadIndex(ip));
		DHCP::Header *dhcp = (DHCP::Header *)udp->payLoad;
		DHCP::Option DHCP_new_OptionsArray(dhcp, options);//macro allocated new options array named options
		
		switch(options[DHCP::getOptionIndexByType(options,DHCP::OPTIONS::types::message_type)].type){
			case DHCP::OPCodeTypes::discover:
				replyToDiscover(packet,responsePacket );
				return true;
			case DHCP::OPCodeTypes::request:
				replyToRequest(packet);
				return true;
			default:
				std::cout<<"unknown option type for dhcp:"
						<< (options[getOptionIndexByType(options,DHCP::OPTIONS::types::message_type)].type);
				throw -1;
		}
		return false;
		
	}
}
