#ifndef TCP__SERVER__CONNECTION_T
#define TCP__SERVER__CONNECTION_T

#include "../../protocol/TCPHeader.cpp"
#include "../../rawSocket/rawSocket.cpp"

namespace TCP{
class Connection{
		char tcpPayload[0xffff] = {0};
		long tcpPayloadLength = 0;
		
		long packetId;
		long serverIP;
			int serverPort;
		long clientIP;
			int clientPort;
		unsigned char clientMacAddress[6];
	
		RawSocket *responseSocket;
		RawSocket *forwardSocket = new RawSocket("wlan0", rawSocket::protocols::tcp);
		
		
	public:
	 Connection(Eth::Header *source, RawSocket *responseSocket);
	 void start();
	 void operator delete (void *p){
			Connection * self = (Connection *)p;
			delete self->forwardSocket;
			free(p);
	}
};
	Connection::Connection(Eth::Header *source, RawSocket *responseSocket){
		this->responseSocket = responseSocket;
		memcpy(this->clientMacAddress, source->sourceMac, 6);
		
		IP::Header *ip = IP::create(Eth::getPayload(source),"Connection:initialize" );
			this->serverIP = ip->destinationIP;
			this->clientIP = ip->sourceIP;
			this->packetId = ip->identification;
			
		TCP::Header *udp = TCP::create(ip, "Connection:initialize");
			this->serverPort = TCP::getDestPort(udp);
			this->clientPort = TCP::getSourcePort(udp);
		this->tcpPayloadLength = TCP::getPayloadLength(ip, udp);
		memcpy(this->tcpPayload, TCP::getPayload(udp), this->tcpPayloadLength );
	}

}

#endif
