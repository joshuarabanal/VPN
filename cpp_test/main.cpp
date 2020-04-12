#define DEBUG true

#include <stdint.h>
#include <iostream>
#include "protocol/IpHeader.cpp"
#include "protocol/UDPHeader.cpp"
#include "server/DHCPServer.cpp"
#include <fstream>
#include "CrashReporter.cpp"
#include "rawSocket/rawSocket.cpp"
#include "rawSocket/rawSocketMetaData.cpp"
#include "protocol/EthHeader.cpp"

//debugging
/*
 *use to debug output text files:
 *   	https://tomeko.net/online_tools/file_to_hex.php?lang=en 
 * use to view packets and debug them: 
 * 		https://hpd.gasmi.net/
 * 		http://packetor.com/
 */


//forward declarations
bool readEvent( char *in, char *out);
void writeFile(const char *name, char *buffer, int length);
void readFile(const char name[], char*buffer, int length);
void logPacket(char *pack);


int main () { 
	
	bool fromfile = true;
	bool shouldSocket = true;
	
	CrashReporter::create();
	
	std::cout << "Output sentence 1\n";
	RawSocket* sock;
	try{
		if(!fromfile || shouldSocket){
			sock = new RawSocket( "eth0");
			
		}
	}
	catch(int err){
		std::cout<<"error initializing socket:"<<err<<"\n";
		return 0;
	}
	char read[65536] = {0};
	char write[65536] = {0};
	
	unsigned char defaultMac[6] = {0};
	sock->getMacAddress(defaultMac);
	
	std::cout<<"starting main loop\n"; std::cout.flush();
	while(true){
		memset(read, 0x00, 65536);
		memset(write, 0x00, 65536);
		
		try{
			if(!fromfile)sock->read(read);
			else readFile("/home/pi/Documents/github/VPN/testData/packet_request.txt", read,65536);
			
			Eth::Header * eth_in = Eth::create(read);
			char * readData = Eth::getPayload(eth_in);
			Eth::Header * eth_out = Eth::create(write);
				Eth::createResponseHeader(eth_out, eth_in);
				Eth::setSourceMac(eth_out, defaultMac);
			char *writeData = Eth::getPayload(eth_out);
			
				
			
			if(readEvent(readData, writeData)){ 
				IP::Header *ip_out = IP::create(writeData, "main:about to send data");
				int length = IP::getLength(ip_out) + sizeof(Eth::Header);
				if(!fromfile){
					sock->write(write, length, eth_out->destinationMac);
				}
				
				//logging details
				logPacket(write); 
				
				IP::Header *ip_in = IP::create(readData,"main:about to log the final data");
				UDP::Header *udp_in = UDP::create(ip_in,"main:about to log the final data");
				DHCP::Header *dhcp_in = DHCP::create(udp_in,"main:about to log the final data");
				
				int length_in = IP::getLength(ip_in) + sizeof(Eth::Header);
				
				const char *readpath = "/home/pi/Documents/github/VPN/testData/lastFullPacket_sent.txt";
				const char *writepath = "/home/pi/Documents/github/VPN/testData/lastFullPacket_recieved.txt";
				
				switch(dhcp_in->OPCode){
					case DHCP::OPCodeTypes::discover:
						readpath = "/home/pi/Documents/github/VPN/testData/packet_discover.txt";
						writepath = "/home/pi/Documents/github/VPN/testData/packet_offer.txt";
						break;
					case DHCP::OPCodeTypes::request:
						readpath = "/home/pi/Documents/github/VPN/testData/packet_request.txt";
						writepath = "/home/pi/Documents/github/VPN/testData/packet_acknowledge.txt";
						break;
				}
				
				writeFile(readpath,write, length);
				writeFile(writepath,read, length_in);
				
				std::cout<<"\n\n\n\n\n";
			}
			else{
				//
			}
			if(fromfile)break;
		}
		catch(int err){
			std::cout<<"eth size:"<<sizeof(Eth::Header)<<"\n";
			std::cout<<"logging incoming data:\n";
			for(int i = 0; i<100;i+=8){
				std::cout<<(int)read[i]<<","<<(int)read[i+1]<<","<<(int)read[i+2]<<","<<(int)read[i+3]
					<<","<<(int)read[i+4]<<","<<(int)read[i+5]<<","<<(int)read[i+6]<<","<<(int)read[i+7]<<"\n";
			}
			std::cout.flush();
			throw err;
		}
	}
	
	return 0;
}

void logPacket(char * pack){
	Eth::Header * eth = Eth::create(pack);
		std::cout<<"eth packet returned:\n";
		Eth::logValues(eth);
		
	IP::Header * ip = IP::create(Eth::getPayload(eth), "main::logPacket,1");
		std::cout<<"Ip Packet returned:\n";
		IP::logValues(ip);
	UDP::Header *udp = UDP::create(ip, "main::logPacket,1");
		std::cout<<"udp Packet returned:\n";
		UDP::logValues(udp);
	DHCP::Header *dhcp = DHCP::create(udp,"main log acket 2");
		std::cout<<"dhcp Packet returned:\n";
		DHCP::logValues(dhcp);
}

bool readEvent(char *in, char *out){
	
	IP::Header * ip_in = IP::create(in, "main::readEvent,1");
	
	if(ip_in->protocol == IPHeader_protocolUDP ){
		UDP::Header *udp_in = NULL;
		try{
		udp_in = UDP::create(ip_in, "Main::readEvent,2");
		}catch(int err){
			std::cout<<"\n\n\n"<<"failed to verify packet throwing it away"<<"\n\n\n\n";
			return false;
		}
		int sourcePort = UDP::getSourcePort(udp_in);
		int destPort = UDP::getDestPort(udp_in);
		
		if(sourcePort == DHCP_clientPort && destPort == DHCP_serverPort){
			
			DHCP::Header *dhcp_in =  DHCP::create(udp_in,"meain:main");
			
			if(DHCP::Server::handleMessage(in, out)){
				return true;
			}
			std::cout<<"dhcp failed to reply to request\n";
			DHCP::logValues(dhcp_in);std::cout<<"\n\n";
		
		}
		else if(sourcePort == UDP_Port_Multicast && destPort == UDP_Port_Multicast){
			std::cout<<"\n\nignoring multicast requests\n\n\n";
			return false;//ignore multicast requests
		}
		else if( destPort == 1900){
			std::cout<<"\n\nignoring UPnP requests\n\n\n";
			return false;//ignore multicast requests
		}
		
		
		std::cout<<"failed to read udp packet request\n";
		UDP::logValues(udp_in);
		
	}
	else if(ip_in->protocol == IPHeader_protocolTCP){
		return false;
	}
	else{
		std::cout<<"skipped packet\n";
		IP::logValues(ip_in);
		return false;
	}
	
	IP::logValues(ip_in);
	std::cout<<"failed to read the packet\n";
	throw 5;
	
	
	
	
}

void readFile(const char name[], char*buffer, int length){
	std::ifstream file; 
	file.open(name);
	file.read(buffer, length);
	file.close();
}

void writeFile(const char name[], char *buffer, int length){
	std::ofstream file; 
	file.open(name);
	file.write(buffer, length);
	file.close();
}
