

namespace TCP{
	struct Header{
		int sourcePort:16;
		int destPort:16;
		long sequenceNumber:32;
		long acknowlegementNumber:32;//value of this field is the next sequence number that the sender of the ACK is expecting
		int dataoffset:4;//size of the TCP header in 32-bit words, minimum of 5
		int reserved:3;
		bool NS:1;//ECN-nonce - concealment protection
		bool CWR:1;//Congestion window reduced
		bool ECE:1;//ECN-Ech
		bool URG:1;//Indicates that the Urgent pointer field is significant
		bool ACK:1;//Indicates that the Acknowledgment field is significant
		bool PSH:1;//Push function. Asks to push the buffered data to the receiving application.
		bool RST:1;//Reset the connection
		bool SYN:1;//Synchronize sequence numbers.
		bool FIN:1;//indicates this is the last packet and close connection
		unsigned int windowSize:16;//specifies the number of window size units[c] that the sender of this segment is currently willing to receive.
		int checksum :16;
		int urgentPointer:16;
		char *optionsStartHere;
		
	}
}
