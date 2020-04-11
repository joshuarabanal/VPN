#include <netinet/in.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>

#ifndef RawSocketMetadata_h
#define RawSocketMetadata_h

namespace Raw{
  namespace {
    void getInterfaceData(const char *interfaceName, int socket, int requestType, struct ifreq *ifreq_ip){
      memset(ifreq_ip,0,sizeof(ifreq));//set the entire struct to null
      strcpy(ifreq_ip->ifr_name,interfaceName);//giving name of Interface
      if(ioctl(socket,requestType,ifreq_ip)<0) //getting IP Address
      {
        std::cout<<"error obtaining ip address for the specified interfave:"<<interfaceName<<"\n";
        throw -45;
      }
      
    }
  }
  char *getIpAddress(int socket, const char *interfaceName, char *retu){
    struct ifreq data;
    getInterfaceData(interfaceName,socket, SIOCGIFADDR, &data);
    memcpy(retu, data.ifr_addr.sa_data, sizeof(char)*4 );
    return retu;
  }
  
  char *getMacAddress(int socket, const char *interfaceName, char *retu){
    struct ifreq data;
    getInterfaceData(interfaceName,socket, SIOCGIFHWADDR, &data);
    memcpy(retu, data.ifr_addr.sa_data, sizeof(char)*6 );
    return retu;
  }
  
  int getInterfaceIndex(int socket, const char *interfaceName){
    struct ifreq data = {0};
    getInterfaceData(interfaceName,socket, SIOCGIFINDEX, &data);
    std::cout.flush();
    return data.ifr_ifindex;
  }
    

}
#endif
