
#include "../rawSocket/rawSocket.cpp"
#include "../util/file.cpp"
#include "../protocol/EthHeader.cpp"
#include "../protocol/IpHeader.cpp"
#include "../protocol/UDPHeader.cpp"
#include "../server/DHCPServer.cpp"
#include "../server/TCPServer.cpp"
#include "../util/LinkedList.cpp"

class WifiAdapterServer{
	//RawSocket * wifi = new RawSocket("wlan0");
	TCP::Server *tcpConnections = new TCP::Server();
	RawSocket * ethernet = new RawSocket("eth0");
	
	bool dhcpMessage(IP::Header *in, IP::Header *out);
	bool routerMessage(IP::Header *readData, IP::Header *writeData);
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
			else if(this->routerMessage(ip_in, ip_out)){
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
	if(ip->protocol == IPHeader_protocolUDP ){
		UDP::Header *udp = UDP::create(ip, "wifi adapter : log packet");
		UDP::logValues(udp);
	}
	if(ip->protocol == IPHeader_protocolTCP ){
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

bool WifiAdapterServer::routerMessage(IP::Header *in, IP::Header *out){
	if(in->sourceIP != DHCP::Server::clientIP){
		return false;
	}
	
	if(in->protocol == IPHeader_protocolUDP ){
		std::cout<<"currently unable to handle UDP Packets\n";
		IP::logValues(in);
		UDP::Header *udp = UDP::create(in, "WifiAdapterServer::routerMessage1");
		UDP::logValues(udp);
		FileIO::writeLogFile(
				"WifiAdapter_error_udp.txt", 
				(char*)in, 
				IP::getTotalLength(in)
		);
		return false;
	}
	else if(in->protocol == IPHeader_protocolTCP ){
		IP::logValues(in);
		TCP::Header *tcp_in = TCP::create(in, "WifiAdapterServer::routerMessage1");
		TCP::Header *tcp_out = TCP::createEmptyHeader(out);
		(this->tcpConnections)->handlePacket(in, tcp_in, out, tcp_out);
		std::cout.flush();
		 throw -24;
	}
	return false;
}


bool WifiAdapterServer::dhcpMessage(IP::Header *ip_in, IP::Header *ip_out){
	if(ip_in->protocol != IPHeader_protocolUDP ){ return false; }
	
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
		else if(sourcePort == UDP_Port_Multicast && destPort == UDP_Port_Multicast){
			return false;//ignore multicast requests
		}
		else if( destPort == 1900){
			std::cout<<"\n\nignoring UPnP requests\n\n\n";
			return false;//ignore multicast requests
		}
		else if(destPort == 5050){
			std::cout<<"ignoring multimedia conference control tool\n";
			return false;// ignore multimedia requests
		}
		
	
		return false;
	
	
	
}

