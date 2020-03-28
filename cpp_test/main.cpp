#include <iostream>
#include "rawSocket.cpp"

int main () { 
	std::cout << "Output sentence 1\n";
	RawSocket* sock;
	try{
		sock = new RawSocket(RawSocket_type_TCP, "eth0");
	}
	catch(int err){
		std::cout<<"error initializing socket:"<<err<<"\n";
		return 0;
	}
	char read[65536];
	int howMany= sock->read(read);
	std::cout<<"read"<<howMany<<" bytes\n";
	std::cout << read;
	
	std::cout<< "end of the road\n";
}
