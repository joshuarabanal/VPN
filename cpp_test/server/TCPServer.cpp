#include "../util/LinkedList.cpp"
#include "../util/file.cpp"
#include "./tcp/ThreeWayHandshake.cpp"
#include "./tcp/Connection.cpp"
namespace TCP{
	class Server{
		LinkedList<TCP::Connection> *TCPConnections = new LinkedList<TCP::Connection>();
		
		public:
		bool handlePacket(IP::Header *ip_in, TCP::Header * in, IP::Header *ip_out, TCP::Header *out);
		
	};
	
	bool Server::handlePacket(IP::Header *ip_in, TCP::Header * in, IP::Header *ip_out, TCP::Header *out){
			TCP::Connection * newConn = new TCP::Connection();
			if(TCP::Handshake::isHandshake(ip_in, in, ip_out, out, newConn)){
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

