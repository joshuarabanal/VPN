#include "routers/WifiAdapterServer.cpp"
#include "util/file.cpp"
#include "rawSocket/rawSocket.cpp"
#include "server/udp/Connection.cpp"
#include "server/tcp/Connection.cpp"

void runfromFile(){
	char buffer[0xffff];
	FileIO::readLogFile("WifiAdapter/error_unknown.txt", buffer, 0xffff);
	//RawSocket *sock = new RawSocket("eth0");
	TCP::Connection *c = new TCP::Connection(Eth::create(buffer),NULL);
	c->start();
}

int main () { 
	
	bool fromFile = true;
	
	
	if(fromFile){
		runfromFile();
		return 0;
	}
	
	
	FileIO::writeLogFile("text.txt", "aamama", strlen("aamama"));
	WifiAdapterServer * server = new WifiAdapterServer();
	server->start();
	return 0;
}
