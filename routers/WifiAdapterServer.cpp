
#include "../rawSocket/rawSocket.cpp"


class WifiAdapterServer{
	RawSocket * wifi = new RawSocket("wlan0");
	RawSocket * ethernet = new RawSocket("eth0");
	
	public:
	void run();

};
