#ifndef DHCP_SERVR_h
#define DHCP_SERVER_h

#include "../protocol/IpHeader.cpp"
#include "../protocol/UDPHeader.cpp"
#include "../protocol/DHCPHeader.cpp"
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <fstream>//delete after debugging

#define DHCP_Server_clientIP  IP::createIpAddress(192, 168, 1,100)
#define DHCP_Server_SubnetMask  IP::createIpAddress(255, 255, 255,0)
#define DHCP_Server_serverIP  IP::createIpAddress(192, 168, 1,12)
#define DHCP_Server_broadcastIp  IP::createIpAddress(255, 255, 255, 255)
#define DHCP_SERVER_AddrLeaseTime  (60*60*24)



namespace DHCP::Server{
	
	namespace{//private functions
		void checkReturnVals(char *pac){
			IP::Header *ip = NULL;
			UDP::Header * udp = NULL;
			DHCP::Header *dhcp = NULL;
			
			try{
				
				ip =  IP::create(pac,"DHCP::Server::check return vals");
				udp = UDP::create(ip,"DHCP::Server::check return vals,2");
				dhcp = DHCP::create(udp,"DHCP::SERVER::check return vals,3");
				
			}catch( int err){
				IP::logValues(ip);
				UDP::logValues(udp);
				DHCP::logValues(dhcp);
				std::cout<<"failed to create properly formatted packet\nDHCP::server::check return vals\n";
				std::cout.flush();
				throw err;
				
			}
				
		}
	}
void replyToRequest(char *read, char *write){
			std::cout<<"DHCPServer:replyToRequest\n";std::cout.flush();
			//create the input headers
			IP::Header * ip_in = IP::create(read, "DHCP::SERVER::replyToRequest,2");
			UDP::Header * udp_in = UDP::create(ip_in, "DHCP::SERVER::replyToRequest,1");
			DHCP::Header *dhcp_in =  DHCP::create(udp_in,"DHCP::SERVER::replyToRequest,3" );
			
			
			
			std::cout<<"DHCPSERVER::replyToRequest:25\n";std::cout.flush();
			//create the out put headers
			IP::Header * ip_out = IP::createEmptyHeader(write);
				IP::copyToResponseHeader(ip_out, ip_in);
				IP::setSourceIPAddress(ip_out, DHCP_Server_serverIP);
				IP::setDestinationIPAddress(ip_out, DHCP_Server_broadcastIp);
			UDP::Header *udp_out = UDP::createEmptyHeader(ip_out);
				UDP::copyToResponseHeader(udp_out, udp_in);
				
			DHCP::Header *dhcp_out =  DHCP::createEmptyHeader(udp_out);
				DHCP::memCpy(dhcp_out, dhcp_in);
			std::cout<<"DHCPSERVER:replyToRequest:28\n";std::cout.flush();
		
			
			
			
			//DHCP::Header *returnDHCP = {0};
			//DHCP::memCpy(returnDHCP, dhcp_in);
			dhcp_out->OPCode = DHCP::OPCodeTypes::acknowledge;
			dhcp_out->yourIpAddress = DHCP_Server_clientIP;
			dhcp_out->serverIpAddress = DHCP_Server_serverIP;
			memcpy(
					dhcp_out->clientHardwareAddress,
					dhcp_in->clientHardwareAddress,
					sizeof(DHCP::Header::clientHardwareAddress)
			);
			
			memcpy(
					dhcp_out->ServerName,
					dhcp_in->ServerName,
					sizeof(DHCP::Header::ServerName)
			);
			
			memcpy(
					dhcp_out->bootFileName,
					dhcp_in->bootFileName,
					sizeof(DHCP::Header::bootFileName)
			);
			
			DHCP::Option options_out[6];
			
			//configure message type
			DHCP::OPTIONS::Message::create(options_out, DHCP::OPTIONS::Message::types::ACK);
			
			//options[1] : subnetMask
			DHCP::OPTIONS::SubnetMask::create(options_out+1, DHCP_Server_SubnetMask);
			
			//options[2] = Router
			unsigned long vals[1] = {DHCP_Server_serverIP};
			DHCP::OPTIONS::Router::create(options_out+2, vals, 1);
			
			//options[3] = LeaseTime
			DHCP::OPTIONS::LeaseTime::create(options_out+3, DHCP_SERVER_AddrLeaseTime);
			
			//options[4] = ServerIdentifier
			DHCP::OPTIONS::ServerId::create(options_out+4, DHCP_Server_serverIP);
			
			//options[5] = DNSServer
			unsigned long dns[2] = {
			      IP::createIpAddress(8,8,8,8),
			      IP::createIpAddress(8,8,4,4)
				};
			DHCP::OPTIONS::DNS::create(options_out+5, dns, 2);
			
			DHCP::setOptions(dhcp_out, options_out, 6);
			
			
			
			UDP::setPayload(
				udp_out, 
				ip_out,  
				(char * )dhcp_out, 
				DHCP::getTotalHeaderLength(dhcp_out)
			) ;
			IP::setPayload(ip_out, (char *)udp_out, UDP::getLength(udp_out));
			
			
			
			if(!IP::checkChecksum(ip_out)){
				std::cout<<"checksum not setting correct:DHCPServer:115\n";
				std::cout<<"checksum:"<<ip_out->checksum<<"\n";
				ip_out->checksum = 0;
				std::cout<<"zeroes checksum:"<<ip_out->checksum<<"\n";
				IP::setChecksum(ip_out);
				std::cout<<"checksum after 2nd try:"<<ip_out->checksum<<"\n";
				throw -6;
			}
			if(!UDP::checkChecksum(ip_out, udp_out)){
				std::cout<<"checksum not setting correct:DHCPServer:122\n";
				std::cout<<"checksum:"<<udp_out->checksum<<"\n";
				ip_out->checksum = 0;
				std::cout<<"zeroes checksum:"<<ip_out->checksum<<"\n";
				UDP::setChecksum(ip_out, udp_out);
				std::cout<<"checksum after 2nd try:"<<ip_out->checksum<<"\n";
				std::cout.flush();
				IP::logValues(ip_out);
				UDP::logValues(udp_out);
				throw -8;
			}
			
			
			
		
		}
		
	void replyToDiscover(char *read, char *write){
		std::cout<<"DHCPServer:reply to discover16\n";std::cout.flush();
		//create the input headers
		IP::Header * ip_in = IP::create(read, "DHCP::SERVER::replyToDiscover,2");
		UDP::Header * udp_in = UDP::create(ip_in, "DHCP::SERVER::replyToDiscover,1");
		DHCP::Header *dhcp_in =  DHCP::create(udp_in,"DHCP::SERVER::replyToDiscover,3" );
		
		std::cout<<"DHCPSERVER:25\n";std::cout.flush();
		//create the out put headers
		IP::Header * ip_out = IP::createEmptyHeader(write);
			IP::copyToResponseHeader(ip_out, ip_in);
			IP::setDestinationIPAddress(ip_out, ip_in->destinationIP);
			IP::setSourceIPAddress(ip_out, DHCP_Server_serverIP);
		UDP::Header *udp_out = UDP::createEmptyHeader(ip_out);
			UDP::copyToResponseHeader(udp_out, udp_in);
			
		DHCP::Header *dhcp_out =  DHCP::createEmptyHeader(udp_out);
			DHCP::memCpy(dhcp_out, dhcp_in);
		std::cout<<"DHCPSERVER:28\n";std::cout.flush();

		
		
		
		//DHCP::Header *returnDHCP = {0};
		//DHCP::memCpy(returnDHCP, dhcp_in);
		dhcp_out->OPCode = DHCP::OPCodeTypes::offer;
		dhcp_out->yourIpAddress = DHCP_Server_clientIP;
		dhcp_out->serverIpAddress = DHCP_Server_serverIP;
		memcpy(
				dhcp_out->clientHardwareAddress,
				dhcp_in->clientHardwareAddress,
				sizeof(DHCP::Header::clientHardwareAddress)
		);
		
		memcpy(
				dhcp_out->ServerName,
				dhcp_in->ServerName,
				sizeof(DHCP::Header::ServerName)
		);
		
		memcpy(
				dhcp_out->bootFileName,
				dhcp_in->bootFileName,
				sizeof(DHCP::Header::bootFileName)
		);
		
		DHCP::Option options_out[6];
		
		//configure message type
		DHCP::OPTIONS::Message::create(options_out, DHCP::OPTIONS::Message::types::OFFER);
		
		//options[1] : subnetMask
		DHCP::OPTIONS::SubnetMask::create(options_out+1, DHCP_Server_SubnetMask);
		
		//options[2] = Router
		unsigned long vals[1] = {DHCP_Server_serverIP};
		DHCP::OPTIONS::Router::create(options_out+2, vals, 1);
		
		//options[3] = LeaseTime
		DHCP::OPTIONS::LeaseTime::create(options_out+3, DHCP_SERVER_AddrLeaseTime);
		
		//options[4] = ServerIdentifier
		DHCP::OPTIONS::ServerId::create(options_out+4, DHCP_Server_serverIP);
		
		//options[5] = DNSServer
		unsigned long dns[2] = {
			  IP::createIpAddress(8,8,8,8),
			  IP::createIpAddress(8,8,4,4)
			};
		DHCP::OPTIONS::DNS::create(options_out+5, dns, 2);
		
		DHCP::setOptions(dhcp_out, options_out, 6);
		
		
		
		UDP::setPayload(
			udp_out, 
			ip_out,  
			(char * )dhcp_out, 
			DHCP::getTotalHeaderLength(dhcp_out)
		) ;
		IP::setPayload(ip_out, (char *)udp_out, UDP::getLength(udp_out));
		
		
		
		if(!IP::checkChecksum(ip_out)){
			std::cout<<"checksum not setting correct:DHCPServer:115\n";
			std::cout<<"checksum:"<<ip_out->checksum<<"\n";
			ip_out->checksum = 0;
			std::cout<<"zeroes checksum:"<<ip_out->checksum<<"\n";
			IP::setChecksum(ip_out);
			std::cout<<"checksum after 2nd try:"<<ip_out->checksum<<"\n";
			throw -6;
		}
		if(!UDP::checkChecksum(ip_out, udp_out)){
			std::cout<<"checksum not setting correct:DHCPServer:122\n";
			std::cout<<"checksum:"<<udp_out->checksum<<"\n";
			ip_out->checksum = 0;
			std::cout<<"zeroes checksum:"<<ip_out->checksum<<"\n";
			UDP::setChecksum(ip_out, udp_out);
			std::cout<<"checksum after 2nd try:"<<ip_out->checksum<<"\n";
			std::cout.flush();
			IP::logValues(ip_out);
			UDP::logValues(udp_out);
			throw -8;
		}
		if(DEBUG == true){
			std::ofstream file; 
			file.open("/home/pi/Documents/github/VPN/testData/packet_discover.txt");
			file.write(read,IP::getLength(ip_in) );
			file.close();
			
			file.open("/home/pi/Documents/github/VPN/testData/packet_offer.txt");
			file.write(write, IP::getLength(ip_out));
			file.close();
		}

	}
	
	
	bool handleMessage(char *packet, char *responsePacket){
		std::cout<<"DHCPServer:104\n";std::cout.flush();
		IP::Header * ip = IP::create(packet, "DHCP::SERVER::handleMessage,1");
		if(ip->protocol != IPHeader_protocolUDP){
			std::cout<<"not a udp request"<<ip->protocol;
			return false;
		}
		UDP::Header * udp = UDP::create(ip, "DHCP::SERVER::handle message,1");
		if(UDP::getSourcePort(udp) != DHCP_clientPort || UDP::getDestPort(udp) != DHCP_serverPort){
			std::cout<<"not a DHCP request\n"<<"sourcePort:"<<UDP::getSourcePort(udp)<<"\n";
			return false;
		}
		DHCP::Header *dhcp = DHCP::create(udp, "DHCP::SERVER::handle message,2");
		
		try{ DHCP::checkValidity(dhcp); }
		catch(int err){
			std::cout<<"\n\n\n";
			IP::logValues(ip);
			std::cout<<"\n\n\n";
			UDP::logValues(udp);
			std::cout<<"\n\n\n";
			std::cout<<"handle message:106\n";
			std::cout.flush();
			throw err;
		}
		
		//DHCP::Option DHCP_new_OptionsArray(dhcp, options);//macro allocated new options array named options
		
		std::cout<<"DHCPServer:149\n";std::cout.flush();
		
		DHCP::Option * message_type = DHCP::OPTIONS::getByType(dhcp, DHCP::OPTIONS::types::message_type);
		if(message_type == NULL){
			std::cout<<"null message type\n"<<"DHCPServer:153\n"; std::cout.flush();
			DHCP::logValues(dhcp);
			throw -1;
		}
		
		
		std::cout<<"DHCPServer:158\n";std::cout.flush();
		switch(message_type->data[0]){
			case DHCP::OPCodeTypes::discover:
				replyToDiscover(packet,responsePacket );
				checkReturnVals(responsePacket);
				return true;
			case DHCP::OPCodeTypes::request:
				replyToRequest(packet, responsePacket);
				checkReturnVals(responsePacket);
				return true;
			default:
				std::cout<<"unknown option type for dhcp:"
						<< message_type->type;
					DHCP::logValues(dhcp);
						std::cout.flush();
				throw -1;
		}
		return false;
		
	}
}
#endif
