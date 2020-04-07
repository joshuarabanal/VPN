

#ifndef CrashReporter_t
#define CrashReporter_t

#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <iostream>

namespace CrashReporter{
	namespace{
		void handler(int sig) {
		  void *array[10];
		  size_t size;

		  // get void*'s for all entries on the stack
		  size = backtrace(array, 10);

		  // print out all the frames to stderr
		 std::cout<< "Error: signal %d:\n";
		  fprintf(stderr, "Error: signal %d:\n", sig);
		  backtrace_symbols_fd(array, size, STDERR_FILENO);
		  exit(1);
		}
	}
	void create(){
		signal(SIGSEGV, handler);
	}
	void logCrash(char *data, int length){
		std::ofstream file; 
		file.open("/home/pi/Documents/github/VPN/testData/Crash.txt");
		file.write(data, length);
		file.close();
	}

}

#endif
