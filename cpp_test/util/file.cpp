
#ifndef FILEIO_h
#define FILEIO_h

#include <stdint.h>
#include <iostream>
#include <fstream>
#include <string.h>

namespace FileIO{
void readFile(const char name[],  char*buffer, int length){
	std::ifstream file; 
	file.open(name);
	file.read(buffer, length);
	file.close();
}

void writeFile(const char name[], const char *buffer, int length){
	std::ofstream file; 
	file.open(name);
	file.write(buffer, length);
	file.close();
}

const char logDir[] = "/home/pi/Documents/github/VPN/testData/";

void writeLogFile(const char name[], const char *buffer, int length){
	char temp[1000] = {0};
	memcpy(temp, logDir, strlen(logDir));
	memcpy(temp+strlen(temp), name, strlen(name) );
	writeFile(temp, buffer, length);
}
}
#endif
