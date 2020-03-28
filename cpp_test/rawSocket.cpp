#include <iostream>
#include<stdlib.h>
#include<string.h>
#include<netinet/ip.h>
#include<sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> 
#include<arpa/inet.h>
#include<errno.h>
#include <unistd.h>

#define RawSocket_type_TCP IPPROTO_TCP

class RawSocket{
	//private
	int sock = -1;
	
	
	//public
	public:
	RawSocket(int type, const char interface[]);
	int read( char buffer[65536]);
};

RawSocket::RawSocket(int type, const char interfaceName[]){
	this->sock = socket (PF_INET, SOCK_RAW, htons(ETH_P_ALL));
	if(this->sock == -1){
		puts("socket could not be created possibly due to not requesting super user");
		throw 1;
	}
	else{
		std::cout<<"initialized socket!\n";
	}
	setsockopt(this->sock, SOL_SOCKET, SO_BINDTODEVICE, interfaceName, strlen(interfaceName));
}
int RawSocket::read(char buffer[65536]){
	
	int howMany = recvfrom(this->sock , buffer , 65536 , 0 , NULL, NULL);
	for(int i = howMany; i<65536; i++){ buffer[i] = 0; }
	return howMany;
}
