#include "routers/WifiAdapterServer.cpp"
#include "util/file.cpp"
#include "rawSocket/rawSocket.cpp"
#include "server/udp/Connection.cpp"

void runfromFile(){
	char buffer[0xffff];
	FileIO::readLogFile("WifiAdapter/dnsLookup.txt", buffer, 0xffff);
	RawSocket *sock = new RawSocket("eth0");
	UDP::server::Connection *c = new UDP::server::Connection(Eth::create(buffer),sock);
	c->start();
}

int main () { 
	
	bool fromFile = false;
	
	
	if(fromFile){
		runfromFile();
		return 0;
	}
	
	
	FileIO::writeLogFile("text.txt", "aamama", strlen("aamama"));
	WifiAdapterServer * server = new WifiAdapterServer();
	server->start();
	return 0;
}
