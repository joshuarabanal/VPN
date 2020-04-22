#include "../util/LinkedList.cpp"
#include "../util/file.cpp"
#include "./tcp/ThreeWayHandshake.cpp"
#include "./tcp/Connection.cpp"
namespace TCP{
	class Server{
		LinkedList<TCP::Connection> *TCPConnections = new LinkedList<TCP::Connection>();
		
		public:
		bool handlePacket(Eth::Header *ip_in, TCP::Header * in);
		
	};
	
	bool Server::handlePacket(Eth::Header *eth_in, TCP::Header * in){
			TCP::Connection * newConn = new TCP::Connection(eth_in, NULL);
			if(in->SYN){
				this->TCPConnections->add(newConn, 0, "TCP::SEerver::handle packet");
				
				return true;
			}
			else{
				delete newConn;
				std::cout<<"cannot handle non handshake packets\n";
				return false;
			}
		}
	
	
}

