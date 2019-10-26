#include <jni.h>
#include <stdio.h>
#include "sockets_RawSocket.h"
#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/ip.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<errno.h>
#include <unistd.h>




void socket_RawSocket_setLong(JNIEnv *environment, jobject self, char* variable_name, long value);
jlong socket_RawSocket_getLong(JNIEnv *environment, jobject self, char* variable_name);






int makeSocket(int type){
  //struct sockaddr_in source_socket_address, dest_socket_address;

    // Open the raw socket
    int sock = socket (PF_INET, SOCK_RAW, type);
    //IPPROTO_TCP;
    if(sock == -1)
    {
        //socket creation failed, may be because of non-root privileges
        puts("you may need to request root using: sudo java -jar run.jar \n");
        perror("Failed to create sockets.RawSocket");
        exit(1);
    }
    return sock;
  
}





/**
 * I = int
 * D = double
 * Z = boolean
 * C = char
 * String = Ljava/lang/String;
 * java field signature: https://docs.oracle.com/javase/7/docs/technotes/guides/jni/spec/types.html
 * setInt(): https://www3.ntu.edu.sg/home/ehchua/programming/java/JavaNativeInterface.html
 */
void socket_RawSocket_setLong(JNIEnv *environment, jobject self, char* variable_name, long value){
  
    //get the class definition
  jclass thisClass = (*environment)->GetObjectClass(environment, self );
  
  //find where the field is in the class
  jfieldID fieldId = (*environment)->GetFieldID(environment, thisClass, variable_name, "J");
  if(NULL == fieldId){
      printf("field id does not exist: %s for class\n", variable_name);
  }

  (*environment)->SetLongField(environment, self , fieldId, value);
    
}
jlong socket_RawSocket_getLong(JNIEnv *environment, jobject self, char* variable_name){
   //get the class definition
  jclass thisClass = (*environment)->GetObjectClass(environment, self );
  
  //find where the field is in the class
  jfieldID fieldId = (*environment)->GetFieldID(environment, thisClass, variable_name, "J");
  if(NULL == fieldId){
      printf("field id does not exist: %s for class\n", variable_name);
  }

  return (*environment)->GetLongField(environment, self , fieldId);
}



JNIEXPORT jbyteArray JNICALL Java_sockets_RawSocket_readNextPacket
  (JNIEnv * environment, jobject self){
    unsigned char *buffer = (unsigned char *)malloc(65536);
  int sock = (int)socket_RawSocket_getLong(environment, self, "socketPointer");

  // recvfrom is used to read data from a socket
      jint packet_size = recvfrom(sock , buffer , 65536 , 0 , NULL, NULL);
      if (packet_size == -1) {
          puts("error in reading packet");
        perror("Failed to read from sockets.RawSocket");
      }
     
      
      
      //create return socket
      jbyteArray retu = (*environment)->NewByteArray( environment,(jsize)packet_size );
      if(retu == NULL){
          printf("failed to make array of length %d", packet_size);
        perror("Failed to read from sockets.RawSocket");
      }
      //printf("new packet size %d", packet_size);
      (*environment)->SetByteArrayRegion(environment, retu, (jsize)0, (jsize)packet_size, (jbyte *) buffer);
      free(buffer);
      return retu;
}

JNIEXPORT void JNICALL Java_sockets_RawSocket_initialize
  (JNIEnv * environment, jobject self, jint type){
   
    char * variable_name = "socketPointer";
    int sock = 0;
    if(type == 6){//
        sock = makeSocket(IPPROTO_TCP);
    }
    if(type == 17){//
        sock = makeSocket(IPPROTO_UDP);
    }
    
    socket_RawSocket_setLong(environment, self, variable_name, sock);
    
  }
/**
 * https://pdfs.semanticscholar.org/7182/0336978814dd41369794d79cb8a519aeb2bc.pdf
 * @param environment
 * @param self
 * @param array
 * @param port
 * @param ipAddress
 * @return 
 */
JNIEXPORT jint JNICALL Java_sockets_RawSocket_writePacket
  (JNIEnv * environment, jobject self, jbyteArray array, jint port, jint ipAddress){
    int sock = (int)socket_RawSocket_getLong(environment, self, "socketPointer");
    int swappedIp = 
            ( (ipAddress>>24) & 0x000000ff ) +//first becomes last
            ( (ipAddress>> 8) & 0x0000ff00 )+//second becomes 3rd
            ( (ipAddress<< 8) & 0x00FF0000 )+//3rd becomes 2nd
            ( (ipAddress<<24) & 0xFF000000);//lasrt becomes first
    //address for socket
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = swappedIp;//ipAddress;
    
    
    int one = 1;
	const int *val = &one;
    if (setsockopt (sock, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
	{
		perror("Error setting IP_HDRINCL");
		exit(0);
        }

    
    
    //create char array
    int length = (*environment)->GetArrayLength (environment,array);
    jbyte * b =   (jbyte *)malloc(length);
    (*environment)->GetByteArrayRegion (environment,array, 0, length, b);
    
    //write the array
    //return write(sock, b, length);
    //return send(sock, b, length, 0);
    int retu =  sendto (sock, b, length,0, (struct sockaddr *)&sin, sizeof(sin));
    if(retu < 0){ 
        //EAGAIN = EWOULDBLOCK = EBADF = 11
        //ECONNRESET = 104
        //EDESTADDRREQ = 89
        return errno;
    }
    return retu;
}
  
  
