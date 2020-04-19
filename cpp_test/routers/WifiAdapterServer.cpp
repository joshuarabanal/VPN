
#include "../rawSocket/rawSocket.cpp"
#include "../util/file.cpp"
#include "../protocol/EthHeader.cpp"
#include "../protocol/IpHeader.cpp"
#include "../protocol/UDPHeader.cpp"
#include "../server/DHCPServer.cpp"
#include "../server/TCPServer.cpp"
#include "../util/LinkedList.cpp"
#include "../server/udp/Connection.cpp"

class WifiAdapterServer{
	//RawSocket * wifi = new RawSocket("wlan0");
	TCP::Server *tcpConnections = new TCP::Server();
	RawSocket * ethernet = new RawSocket("eth0");
	
	bool dhcpMessage(IP::Header *in, IP::Header *out);
	bool routerMessage(
			Eth::Header *eth_in, IP::Header *ip_in,
			Eth::Header *eth_out,IP::Header *ip_out
	);
	void logPackets(char *in, char* out);
	void logPacket(Eth::Header *out);
	
	public:
	void start();

};

void WifiAdapterServer::start(){
	
	bool fromfile = false;
	
	char read[65536] = {0};
	char write[65536] = {0};
	
	unsigned char defaultMac[6] = {0};
	this->ethernet->getMacAddress(defaultMac);
	
	std::cout<<"starting wifi adapter server\n";
	while(true){
		memset(read, 0x00, 65536);
		memset(write, 0x00, 65536);
		
		try{
			if(!fromfile)this->ethernet->read(read);
			else FileIO::readFile("/home/pi/Documents/github/VPN/testData/packet_discover.txt", read,65536);
			
			Eth::Header * eth_in = Eth::create(read);
			IP::Header * ip_in = IP::create(Eth::getPayload(eth_in), "wifi adapter:run");
			if( IP::isSrcIp(ip_in, 192,168,1,13) ){
				continue;
			}
			else if( IP::isSrcIp(ip_in, 192,168,1,12) ){
				continue;
			}
			
			Eth::Header * eth_out = Eth::create(write);
				Eth::createResponseHeader(eth_out, eth_in);
				Eth::setSourceMac(eth_out, defaultMac);
			IP::Header *ip_out = IP::createEmptyHeader( Eth::getPayload(eth_out) );
			
			if(this->dhcpMessage(ip_in, ip_out)){
				int length_out = IP::getLength(ip_out) + sizeof(Eth::Header);
				this->ethernet->write(write, length_out, eth_out->destinationMac);
			}	
			else if(this->routerMessage(eth_in,ip_in, eth_out, ip_out)){
				this->logPackets(read,write);
			}
			else {
				continue;
			}
		}
		catch(int err){
			this->logPackets(read, write);
			throw err;
		}
	}
	
	return;
	
}

void WifiAdapterServer::logPacket(Eth::Header * data){
	Eth::logValues(data);
	IP::Header * ip = IP::create(Eth::getPayload(data), "wifi adapter:logPacket");
	IP::logValues(ip);
	if(ip->protocol == IP::protocol::UDP ){
		UDP::Header *udp = UDP::create(ip, "wifi adapter : log packet");
		UDP::logValues(udp);
	}
	if(ip->protocol == IP::protocol::TCP ){
		TCP::Header *tcp = TCP::create(ip, "wifi adapter : log packet");
		TCP::logValues(tcp);
	}
}

void WifiAdapterServer::logPackets(char *in, char *out){
	std::cout<<"loggining in packet:\n";
	logPacket(Eth::create(in));
	std::cout<<"loggining out packet:\n";
	logPacket(Eth::create(out));
	
}

bool WifiAdapterServer::routerMessage(
			Eth::Header *eth_in, IP::Header *ip_in,
			Eth::Header *eth_out,IP::Header *ip_out
){
	if(ip_in->sourceIP != DHCP::Server::clientIP){
		return false;
	}
	
	if(ip_in->protocol == IP::protocol::UDP ){
		UDP::Header *udp = UDP::create(ip_in, "WifiAdapterServer::routerMessage1");
		if(UDP::getDestPort(udp) == UDP::CommonPorts::DNS){//handle a dns packet
			if( IP::isDestIp(ip_in,8,8,8,8) || IP::isDestIp(ip_in, 8,8,4,4) ){
				FileIO::writeLogFile(
						"WifiAdapter/dnsLookup.txt", 
						(char*)eth_in, 
						IP::getTotalLength(ip_in)+Eth::HeaderLength
				);
				UDP::server::Connection * conn = new UDP::server::Connection(eth_in, this->ethernet);
				conn->start();
				std::cout<<"got desired log files\n";
				std::cout.flush();
				throw -96;
			}
			std::cout<<"dns packet found\n";
		}
		std::cout<<"currently unable to handle UDP Packets\n";
		std::cout<<"not dns port:"<<UDP::CommonPorts::DNS<<"\n";
		IP::logValues(ip_in);
		UDP::logValues(udp);
		FileIO::writeLogFile(
				"WifiAdapter/error_udp.txt", 
				(char*)eth_in, 
				IP::getTotalLength(ip_in)+Eth::HeaderLength
		);
		return false;
	}
	else if(ip_in->protocol == IP::protocol::TCP ){
		IP::logValues(ip_in);
		TCP::Header *tcp_in = TCP::create(ip_in, "WifiAdapterServer::routerMessage1");
		TCP::Header *tcp_out = TCP::createEmptyHeader(ip_out);
		(this->tcpConnections)->handlePacket(ip_in, tcp_in, ip_out, tcp_out);
		std::cout.flush();
		 throw -24;
	}
	return false;
}


bool WifiAdapterServer::dhcpMessage(IP::Header *ip_in, IP::Header *ip_out){
	if(ip_in->protocol != IP::protocol::UDP ){ return false; }
	
	UDP::Header *udp_in = UDP::create(ip_in, "wifi adapter:dhcp message");
	
	if(IP::isDestIp(ip_in, 224,0,0,252)){ return false; }//multicast packet
	
		int sourcePort = UDP::getSourcePort(udp_in);
		int destPort = UDP::getDestPort(udp_in);
		
		if(sourcePort == DHCP_clientPort && destPort == DHCP_serverPort){
			
			DHCP::Header *dhcp_in =  DHCP::create(udp_in,"meain:main");
			
			if(DHCP::Server::handleMessage(ip_in, ip_out)){
				return true;
			}
			std::cout<<"dhcp failed to reply to request\n";
			DHCP::logValues(dhcp_in);std::cout<<"\n\n";
			return false;
		
		}
		else if(sourcePort == UDP::CommonPorts::Multicast && destPort == UDP::CommonPorts::Multicast){
			return false;//ignore multicast requests
		}
		else if( destPort == UDP::CommonPorts::UPnP){
			std::cout<<"\n\nignoring UPnP requests\n\n\n";
			return false;//ignore multicast requests
		}
		else if(destPort == UDP::CommonPorts::MultimediaConferenceControl){
			std::cout<<"ignoring multimedia conference control tool\n";
			return false;// ignore multimedia requests
		}
		
	
		return false;
	
	
	
}

