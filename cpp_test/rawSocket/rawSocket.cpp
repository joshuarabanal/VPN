#ifndef RAWSOCKET_h
#define RAWSOCKET_h

#include <iostream>
#include<stdlib.h>
#include<string.h>
#include<netinet/ip.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include <sys/types.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> 
#include<arpa/inet.h>
#include<errno.h>
#include <unistd.h>
#include "../protocol/IpHeader.cpp"
#include "rawSocketMetaData.cpp"

#define RawSocket_type_TCP IPPROTO_TCP
#define RawSocket_type_UDP IPPROTO_UDP

class RawSocket{
	//private
	char interfaceName[100];
	int sock = -1;
	void createSendSockAddr(
		struct sockaddr_ll *retu, 
		const unsigned char destMacAddr[6]
	);
	
	
	//public
	public:
	RawSocket(const char interface[]);
	int read( char buffer[65536]);
	int write(char buffer[65536], int bufferLength, unsigned char destinationMacAddress[6]);
	void getMacAddress(unsigned char return_mac_address[6]);
};

RawSocket::RawSocket(const char interfaceName[]){
	strcpy(this->interfaceName, interfaceName);
	std::cout<<"interface mane set:"<<this->interfaceName<<"\n";
	
	this->sock =socket(AF_PACKET,SOCK_RAW,htons(ETH_P_IP));//when we call ETH_P_ALL we can get non IP packets
	if(this->sock == -1){
		puts("socket could not be created possibly due to not requesting super user");
		throw 1;
	}
	else{
		std::cout<<"initialized socket:"<<this->sock<<"\n";
	}
	int valueSet = setsockopt(this->sock, SOL_SOCKET, SO_BINDTODEVICE, interfaceName, strlen(interfaceName));
	if(valueSet != 0){
		std::cout<<"failed to set sockopt for "<<interfaceName<<" strlen="<<strlen(interfaceName)<<"\n";
		std::cout<<"error message:"<<strerror(errno)<<"\n";
		std::cout.flush();
		throw 78;
	}
	else{
		std::cout<<"bound socket to interface:"<<interfaceName<<" strlen="<<strlen(interfaceName)<<"\n";
	}
}

int RawSocket::read(char buffer[65536]){
	//unsigned char *buffer = (unsigned char *) malloc(65536); //to receive data
	memset(buffer,0,65536);
	struct sockaddr saddr;
	int saddr_len = sizeof (saddr);
	
	//Receive a network packet and copy in to buffer
	int howMany = recvfrom(this->sock,buffer,65536,0,&saddr,(socklen_t *)&saddr_len);
	/*
	 std::cout<<
	"sockaddr:\n"<<
	"addr family:"<<
	(saddr.sa_family)
	<<"\n";
	*/
     /*  
	int howMany = recvfrom(
			this->sock , buffer , 65536 , 0 ,
			&(this->addr), &(this->addrLen) 
	);*/
	
	if(howMany <0){
		std::cout<<"unknown error reading socket\n"; std::cout.flush();
		throw 87;
	}
	for(int i = howMany; i<65536; i++){ buffer[i] = 0; }
	
	return howMany;
}


void RawSocket::createSendSockAddr(
		struct sockaddr_ll *retu, 
		const unsigned char destMacAddr[6]
){
	//struct sockaddr_ll sadr_ll;
	int index = Raw::getInterfaceIndex(this->sock,this->interfaceName);
	std::cout<<"got interface index:"<<index<<"\n"; std::cout.flush();
	retu->sll_ifindex = index; // index of interface
	std::cout<<"set interface index:\n"; std::cout.flush();
	retu->sll_halen = 6; // length of destination mac address
	retu->sll_addr[0] = destMacAddr[0];
	retu->sll_addr[1] = destMacAddr[1];
	retu->sll_addr[2] = destMacAddr[2];
	retu->sll_addr[3] = destMacAddr[3];
	retu->sll_addr[4] = destMacAddr[4];
	retu->sll_addr[5] = destMacAddr[5];
}

int RawSocket::write(char buffer[0xffff],int bufferLength, unsigned char destMacAddr[6] ){
   if(
		destMacAddr[0] == 0 && destMacAddr[1] == 0 && destMacAddr[2] == 0 
		&& destMacAddr[3] == 0 && destMacAddr[4] == 0 && destMacAddr[5] == 0 
	){
	   std::cout<<"invalid destination mac 00.00.00.00.00.00\n";
	   std::cout.flush();
	   throw -87;
   }
   sockaddr_ll outMeta = {0}; 
   this->createSendSockAddr(&outMeta,destMacAddr);
	std::cout<<"attempting to send packet\n"; std::cout.flush();
   int send_len = sendto(
		this->sock,
		buffer,bufferLength,
		0,
		(const struct sockaddr*)&outMeta,
		sizeof(struct sockaddr_ll)
	);
	if(send_len<0)
	{
		std::cout<<"error sending the packet\n"; 
		std::cout<<"how many:"<<send_len<<"\n";
		std::cout<<"errno:"<<errno<<":"<<strerror(errno)<<"\n";
		switch(errno){
			
			case 90:
				std::cout<<"message length:"<<bufferLength<<"\n";
				break;
		}
		std::cout.flush();
		throw -5;
	}
   return send_len;
}
	
void RawSocket::getMacAddress(unsigned char retu[6]){
	Raw::getMacAddress(this->sock, this->interfaceName, retu);
}

#endif
