#ifndef TCP__SERVER__CONNECTION_T
#define TCP__SERVER__CONNECTION_T

#include "../../protocol/TCPHeader.cpp"
#include "../../protocol/IpHeader.cpp"
#include "../../rawSocket/rawSocket.cpp"
#include "../../util/file.cpp"
#include "../../server/tcp/ThreeWayHandshake.cpp"

namespace TCP{
class Connection{
		char tcpHeader[0xffff] = {0};
		long tcpTotalLength = 0;
		
		long packetId;
		long serverIP;
			int serverPort;
		long clientIP;
			int clientPort;
		unsigned char clientMacAddress[6];
		unsigned char ourMac[6];
		
		//tcp stuff
		long sequenceNumber = 0;
	
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
namespace {
	void writeLogFile(const char *type, Eth::Header *data){
		char temp[0xff] = {0};
		const char *logDir = "Connection/tcp/";
		memcpy(temp, logDir, strlen(logDir));
		memcpy(temp+strlen(temp), type, strlen(type) );
		IP::Header * ip = IP::create(Eth::getPayload(data),"tcp::connection::writelog file");
		FileIO::writeLogFile(temp, (char *) data, IP::getTotalLength(ip)+Eth::HeaderLength);
	
	}
}
	Connection::Connection(Eth::Header *source, RawSocket *responseSocket){
		this->responseSocket = responseSocket;
		memcpy(this->ourMac, source->destinationMac, 6);
		memcpy(this->clientMacAddress, source->sourceMac, 6);
		
		IP::Header *ip = IP::create(Eth::getPayload(source),"Connection:initialize" );
			this->serverIP = ip->destinationIP;
			this->clientIP = ip->sourceIP;
			this->packetId = ip->identification;
			
		TCP::Header *tcp = TCP::create(ip, "Connection:initialize");
			this->serverPort = TCP::getDestPort(tcp);
			this->clientPort = TCP::getSourcePort(tcp);
		this->tcpTotalLength = TCP::getTotalLength(ip, tcp);
		memcpy(this->tcpHeader, tcp, this->tcpTotalLength );
		
		this->sequenceNumber = TCP::getSEQ(tcp);
		
		writeLogFile("syn.txt", source);
	}

	void Connection::start(){
		char buffer[0xffff] = {0};
		
		Eth::Header *eth_out = Eth::create(buffer, this->ourMac, this->clientMacAddress, Eth::Type::ipv4);
				
		IP::Header * ip_out = IP::create(eth_out, this->clientIP, this->serverIP, this->packetId, IP::protocol::TCP);
		
		TCP::Header *tcp_out = TCP::Handshake::create_SYN_ACK(ip_out,(TCP::Header *)this->tcpHeader);
		
		IP::setPayload(ip_out, (char *)tcp_out, TCP::getHeaderSize(tcp_out) );
		
		writeLogFile("syn_ack.txt", eth_out);
		
		std::cout<<"sent syn ack\n";
		std::cout<<"check log files to verify the valididty\n";
		std::cout<<"tcp::connection::start\n";
		std::cout.flush();
		throw (-53);
		
	}
}

#endif
