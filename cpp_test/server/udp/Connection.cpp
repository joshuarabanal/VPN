#include "../../protocol/EthHeader.cpp"
#include "../../protocol/UDPHeader.cpp"
#include "../../rawSocket/rawSocket.cpp"

namespace UDP::server{
	class Connection{
		char udpPayload[0xffff] = {0};
		long udpPayloadLength = 0;
		
		long serverIP;
			int serverPort;
		long clientIP;
			int clientPort;
		char clientMacAddress[6];
		RawSocket *responseSocket;
		RawSocket *forwardSocket = new RawSocket("wlan0", rawSocket::protocols::udp);
		
		public:
		Connection(Eth::Header *in, RawSocket *responseSocket);
		void start();
	};
	
	Connection::Connection(Eth::Header *in, RawSocket *responseSocket){
		this->responseSocket = responseSocket;
		
		//memcpy((char *)in, this->ethPacket, 0xffff);
		memcpy(this->clientMacAddress, in->sourceMac, 6);
		
		IP::Header *ip = IP::create(Eth::getPayload(in),"Connection:initialize" );
			this->serverIP = ip->destinationIP;
			this->clientIP = ip->sourceIP;
			
		UDP::Header *udp = UDP::create(ip, "Connection:initialize");
			this->serverPort = UDP::getDestPort(udp);
			this->clientPort = UDP::getSourcePort(udp);
		this->udpPayloadLength = UDP::getPayloadLength(udp);
		memcpy(this->udpPayload, UDP::getPayload(udp), this->udpPayloadLength );
	}
	
	void Connection::start(){
		this->forwardSocket->sendTo(
			this->udpPayload, this->udpPayloadLength, 
			this->serverPort, this->serverIP
		);
		char buffer[0xffff] = {0};
		int howmany = this->forwardSocket->recieveFrom(buffer, 0xffff,this->serverPort, this->serverIP );
		
		if(howmany >0){}
	}
}
