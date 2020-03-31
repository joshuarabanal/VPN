
struct TcpHeader{
	char preamble[7];
	char startDelimiter;
	char destinationMACAddress[6];
	char sourceMACAddress[6];
	uint16_t dataLength;
	char *dataStart;
	//int32_t crc;
};
