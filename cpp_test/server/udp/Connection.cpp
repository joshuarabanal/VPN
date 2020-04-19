#ifndef UDP__server__Connection_t
#define UDP__server__Connection_t


#include "../../protocol/EthHeader.cpp"
#include "../../protocol/IpHeader.cpp"
#include "../../protocol/UDPHeader.cpp"
#include "../../server/DHCPServer.cpp"
#include "../../rawSocket/rawSocket.cpp"
#include "../../util/file.cpp"

namespace UDP::server{
	class Connection{
		char udpPayload[0xffff] = {0};
		long udpPayloadLength = 0;
		long packetId;
		long serverIP;
			int serverPort;
		long clientIP;
			int clientPort;
		unsigned char clientMacAddress[6];
		
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
			this->packetId = ip->identification;
			
		UDP::Header *udp = UDP::create(ip, "Connection:initialize");
			this->serverPort = UDP::getDestPort(udp);
			this->clientPort = UDP::getSourcePort(udp);
		this->udpPayloadLength = UDP::getPayloadLength(udp);
		memcpy(this->udpPayload, UDP::getPayload(udp), this->udpPayloadLength );
	}
	
	void Connection::start(){
		std::cout<<"connsection : start\n";
		this->forwardSocket->sendTo(
			this->udpPayload, this->udpPayloadLength, 
			this->serverPort, this->serverIP
		);
		
		
		
		std::cout<<"connsection : start = sent packet\n";
		char buffer[0xffff] = {0};
		int howMany = this->forwardSocket->recieveFrom(buffer, 0xffff,this->serverPort, this->serverIP );
		
		
		std::cout<<"connsection : start = recieved packet\n";
		char retuChar[0xffff] = {0};
		
		
				unsigned char ourMac[6] = {0};
				this->responseSocket->getMacAddress(ourMac);
		Eth::Header *eth_out = Eth::create(retuChar, ourMac, this->clientMacAddress, Eth::Type::ipv4);
				
		IP::Header * ip_out = IP::create(eth_out, this->clientIP, DHCP::Server::serverIP, this->packetId);
		
		UDP::Header *udp_out = UDP::create(ip_out, this->serverPort, this->clientPort, buffer, howMany);
		IP::setPayload(ip_out, (char *)udp_out, UDP::getTotalLength(udp_out));
		
		FileIO::writeLogFile(
						"Connection/responseCheck.txt", 
						(char*)eth_out, 
						IP::getTotalLength(ip_out)+Eth::HeaderLength
				);
			std::cout<<"sent test packet";
			std::cout.flush();
		throw -49;
		
		
	}
}



#endif
