

#ifndef TCP__HEADER_T
#define TCP__HEADER_T

#include "IpHeader.cpp"

namespace TCP{
	struct Header{
		int sourcePort:16;
		int destPort:16;
		long sequenceNumber:32;
		long acknowlegementNumber:32;//value of this field is the next sequence number that the sender of the ACK is expecting
		
		//swapped due to byte order on raspberry pi
		int reserved:3;
		bool NS:1;//UNKNOWN
		int dataoffset:4;//size of the TCP header in 32-bit words, minimum of 5
		
		bool FIN:1;//UNKNOWN
		bool SYN:1;//Congestion window reduced = true
		bool RST:1;//UNKNOWN
		bool PSH:1;//UNKNOWN
		bool ACK:1;//acknowlegement fields significant 
		bool URG:1;//UNKNOWN
		bool ECE:1;//UNKNOWN
		bool CWR:1;//UNKNOWN
		
		/**
		 * 
		bool NS:1;//ECN-nonce - concealment protection
		
		bool CWR:1;//Congestion window reduced
		bool ECE:1;//ECN-Ech
		bool URG:1;//Indicates that the Urgent pointer field is significant
		bool ACK:1;//Indicates that the Acknowledgment field is significant
		bool PSH:1;//Push function. Asks to push the buffered data to the receiving application.
		bool RST:1;//Reset the connection
		bool SYN:1;//Synchronize sequence numbers.
		bool FIN:1;//indicates this is the last packet and close connection
		**/
		unsigned int windowSize:16;//specifies the number of window size units[c] that the sender of this segment is currently willing to receive.
		int checksum :16;
		int urgentPointer:16;
		
	};
	
	int getHeaderSize(Header *self){ return self->dataoffset * 4; }
	char *getPayload(Header *self){ return ((char *)self) + getHeaderSize(self); } 	
	long getTotalLength(IP::Header *parent, Header *self){	return IP::getPayloadLength(parent);	}
	int getPayloadLength(IP::Header *parent, Header *self){ return getTotalLength(parent, self) - getHeaderSize(self); }
	
	void setSourcePort(Header * self, int port){ self->sourcePort = NetworkEndian::formatShort(port); }
	int getSourcePort(Header * self){ return NetworkEndian::formatShort(self->sourcePort); }
	
	
	void setDestPort(Header * self, int port){ self->destPort = NetworkEndian::formatShort(port); }
	int getDestPort(Header * self){ return NetworkEndian::formatShort(self->destPort); }
	
	namespace {
		uint16_t calcChecksum(IP::Header *ip, Header *self){
			char temp[0xffff] = {0};
			
			unsigned long ipin = ip->sourceIP;
			temp[0] = (ipin)& 0xff;
			temp[1] = (ipin>>8)&0xff;
			temp[2] = (ipin>>16)&0xff;
			temp[3] = (ipin>>24)&0xff;;
			
			unsigned long ipout = ip->destinationIP;
			temp[4] = (ipout)&0xff;
			temp[5] = (ipout>>8)&0xff;
			temp[6] = (ipout>>16)&0xff;
			temp[7] = (ipout>>24)&0xff;
			
			
			temp[8] = 0;
			temp[9] = ip->protocol;
			
			
			int len = getTotalLength(ip, self);
			
			temp[10] = (len>>8)&0xff;
			temp[11] = len&0xff;
			
			
			
			char *tempe = (char *)self;
			for(int i = 0; i<len;i++){
				temp[12+i] = tempe[i];
			}
			
			return IPHeader_calcChecksum(temp,13+len);
		}
	}
	
	void logValues(TCP::Header *src){
		std::cout << "sourcPort:"<<NetworkEndian::formatShort(src->sourcePort);
		std::cout << "\ndestPort:"<<NetworkEndian::formatShort(src->destPort);
		std::cout << "\n\tsequence:"<<NetworkEndian::formatLong(src->sequenceNumber);
		std::cout << "\n\t acknowlegementNumber:"<<NetworkEndian::formatLong(src->acknowlegementNumber);
		std::cout << "\n\t header size in bytes:"<<(4*src->dataoffset);
		std::cout << "\n\t concealment:"<<src->NS;
		std::cout << "\n\t congestion window reduced:"<<src->CWR;
		std::cout << "\n\t ECE:"<<src->ECE;
		std::cout << "\n\t URGent field significant:"<<src->URG;
		std::cout << "\n\t acknowledgement field significant:"<<src->ACK;
		std::cout << "\n\t PUSH:"<<src->PSH;
		std::cout << "\n\t reset:"<<src->RST;
		std::cout << "\n\t SYNC:"<<src->SYN;
		std::cout << "\n\t Close Connection:"<<src->FIN;
		std::cout << "\nwindowSize:"<<NetworkEndian::formatShort(src->windowSize);
		std::cout << "\nchecksum:"<<NetworkEndian::formatShort(src->checksum);
		std::cout << "\nurgentPointer:"<<NetworkEndian::formatShort(src->urgentPointer);
		std::cout<<"\n";
	}
	
	long getACK(Header *self){ return NetworkEndian::formatLong(self->acknowlegementNumber); }
	void setACK(Header *self, long ack){ self->acknowlegementNumber = NetworkEndian::formatLong(ack); }
	
	long getSEQ(Header *self){ return NetworkEndian::formatLong(self->sequenceNumber); }
	void setSEQ(Header *self, long seq){ self->sequenceNumber = NetworkEndian::formatLong(seq); }
	
	long getWindowSize(Header *self){ return NetworkEndian::formatShort(self->windowSize); }
	void setWindowSize(Header *self, long val){ self->windowSize = NetworkEndian::formatShort(val); }
	
	
	
	void setChecksum(IP::Header *parent, Header *self){
		self->checksum = 0;
		self->checksum = ~calcChecksum(parent, self);
	}

	TCP::Header *createEmptyHeader(IP::Header *src){
		return (TCP::Header *) 
		(
			((char *)src) + IP::getPayloadIndex(src)
		);
	}
	TCP::Header *create(IP::Header *src, const char *error){
		TCP::Header *retu = createEmptyHeader(src);
		long sum = calcChecksum(src, retu);
		if(sum != 0 && sum != 0xffff){
			std::cout<<"checksum invalid:"<<sum<<"\n logging packet:\n";
			logValues(retu);
			throw 76;
		}
		return retu;
	}
	

}


#endif
