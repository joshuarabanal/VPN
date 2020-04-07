#define DEBUG true

#include <stdint.h>
#include <iostream>
#include "rawSocket.cpp"
#include "protocol/ethernetHeader.cpp"
#include "protocol/UDPHeader.cpp"
#include "server/DHCPServer.cpp"
#include <fstream>
#include "CrashReporter.cpp"

//debugging



//forward declarations
void readEvent( char *in, char *out);
void writeFile(const char *name, char *buffer, int length);
void readFile(const char name[], char*buffer, int length);
void logPacket(char *pack);


int main () { 
	
	CrashReporter::create();
	
	std::cout << "Output sentence 1\n";
	//RawSocket* sock;
	try{
		//sock = new RawSocket(RawSocket_type_UDP, "eth0");
	}
	catch(int err){
		std::cout<<"error initializing socket:"<<err<<"\n";
		return 0;
	}
	char read[65536] = {0};
	char write[65536] = {0};
	
	
	
	
	
	while(true){
		memset(read, 0x00, 65536);
		memset(write, 0x00, 65536);
		
		//sock->read(read);
		readFile("/home/pi/Documents/github/VPN/testData/Crash.txt", read,65536);
		
		
			
		
		readEvent(read, write); 
		//sock->write(write);
		logPacket(write); break;
	}
	
	return 0;
}
void logPacket(char * pack){
	IP::Header * ip = IP::create(pack, "main::logPacket,1");
		std::cout<<"Ip Packet returned:\n";
		IP::logValues(ip);
	UDP::Header *udp = UDP::create(ip, "main::logPacket,1");
		std::cout<<"udp Packet returned:\n";
		UDP::logValues(udp);
	DHCP::Header *dhcp = DHCP::create(udp);
		std::cout<<"dhcp Packet returned:\n";
		DHCP::logValues(dhcp);
}
void readEvent(char *in, char *out){
	std::cout<<"\n\n\n\n\n";
	
	IP::Header * ip_in = IP::create(in, "main::readEvent,1");
	
	if(ip_in->protocol == IPHeader_protocolUDP){
		
		UDP::Header *udp_in = UDP::create(ip_in, "Main::readEvent,2");
		int sourcePort = UDP::getSourcePort(udp_in);
		int destPort = UDP::getDestPort(udp_in);
		
		if(sourcePort == DHCP_clientPort && destPort == DHCP_serverPort){
			
			DHCP::Header *dhcp_in =  DHCP::create(udp_in);
			
			if(DHCP::Server::handleMessage(in, out)){
				return;
			}
			std::cout<<"dhcp failed to reply to request\n";
			DHCP::logValues(dhcp_in);std::cout<<"\n\n";
		
		}
		else if(sourcePort == UDP_Port_Multicast && destPort == UDP_Port_Multicast){
			std::cout<<"\n\nignoring multicast requests\n\n\n";
			return;//ignore multicast requests
		}
		
		std::cout<<"failed to read udp packet request\n";
		UDP::logValues(udp_in);
		
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
