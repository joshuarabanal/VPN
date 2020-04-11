#include <netinet/in.h>


namespace RawSocket{
  namespace {
    void getInterfaceData(char *interfaceName, int socket, int requestType, struct ifreq *ifreq_ip){
      memset(&ifreq_ip,0,sizeof(ifreq_ip));//set the entire struct to null
      strncpy(ifreq_ip.ifr_name,interfaceName,IFNAMSIZ-1);//giving name of Interface
      if(ioctl(socket,requestType,&ifreq_ip)<0) //getting IP Address
      {
        std::cout<<"error obtaining ip address for the specified interfave:"<<interfaceName<<"\n";
        throw -45;
      }
    }
  }
  char *getIpAddress(int socket, char *interfaceName, char *retu){
    struct ifreq data;
    getInterfaceData(interfaceName,socket, SIOCGIFADDR, &data);
    memcpy(retu data.ifr_addr, sizeof(char)*4 );
    return retu;
  }
  
  char *getMacAddress(int socket, char *interfaceName, char *retu){
    struct ifreq data;
    getInterfaceData(interfaceName,socket, SIOCGIFHWADDR &data);
    memcpy(retu data.ifr_addr, sizeof(char)*4 );
    return retu;
  }
    

}
