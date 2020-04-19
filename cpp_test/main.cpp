#include "routers/WifiAdapterServer.cpp"
#include "util/file.cpp"


int main () { 
	FileIO::writeLogFile("text.txt", "aamama", strlen("aamama"));
	WifiAdapterServer * server = new WifiAdapterServer();
	server->start();
}
