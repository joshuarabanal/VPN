
#include "../rawSocket/rawSocket.cpp"
#include "../util/file.cpp"
#include "../protocol/EthHeader.cpp"
#include "../protocol/IpHeader.cpp"
#include "../protocol/UDPHeader.cpp"
#include "../server/DHCPServer.cpp"

class WifiAdapterServer{
	RawSocket * wifi = new RawSocket("wlan0");
	RawSocket * ethernet = new RawSocket("eth0");
	
	bool dhcpMessage(IP::Header *in, IP::Header *out);
	bool routerMessage(IP::Header *readData, IP::Header *writeData);
	void logPackets(char *in, char* out);
	
	public:
	void start();

};

void WifiAdapterServer::start(){
	
	bool fromfile = false;
	
	char read[65536] = {0};
	char write[65536] = {0};
	
	unsigned char defaultMac[6] = {0};
	this->ethernet->getMacAddress(defaultMac);
	
	while(true){
		memset(read, 0x00, 65536);
		memset(write, 0x00, 65536);
		
		try{
			if(!fromfile)this->ethernet->read(read);
			else FileIO::readFile("/home/pi/Documents/github/VPN/testData/packet_discover.txt", read,65536);
			
			Eth::Header * eth_in = Eth::create(read);
			IP::Header * ip_in = IP::create(Eth::getPayload(eth_in), "wifi adapter:run");
			
			Eth::Header * eth_out = Eth::create(write);
				Eth::createResponseHeader(eth_out, eth_in);
				Eth::setSourceMac(eth_out, defaultMac);
			IP::Header *ip_out = IP::create(Eth::getPayload(eth_out), "wfi adapter:run 2");
			
			if(this->dhcpMessage(ip_in, ip_out)){
				int length_out = IP::getLength(ip_out) + sizeof(Eth::Header);
				this->ethernet->write(write, length_out, eth_out->destinationMac);
			}	
			else if(this->routerMessage(ip_in, ip_out)){
				this->logPackets(read,write);
			}
			
			
			else break;
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
	IP::Header * ip = IP::create(Eth::getPayload(eth_in), "wifi adapter:logPacket");
	IP::logValues(ip);
	if(ip_in->protocol == IPHeader_protocolUDP ){
		UDP::Header *udp = UDP::create(ip, "wifi adapter : log packet");
		UDP::logValues(udp);
	}
	if(ip_in->protocol == IPHeader_protocolTCP ){
		TCP::Header *udp = TCP::create(ip, "wifi adapter : log packet");
		TCP::logValues(udp);
	}
}

void WifiAdapterServer::logPackets(char *in, char *out){
	std::cout<<"loggining in packet:\n";
	logPacket(Eth::create(in));
	std::cout<<"loggining out packet:\n";
	logPacket(Eth::create(out));
	
}

bool WifiAdapterServer::routerMessage(IP::Header *in, IP::Header *out){
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
		
		
		std::cout<<"failed to read udp packet request\n";
		UDP::logValues(udp_in);
		
	
	
		return false;
	
	
	
}

