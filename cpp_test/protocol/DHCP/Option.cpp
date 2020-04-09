#ifndef DHCP_OPTIONS_h
#define DHCP_OPTIONS_h
/**
 * to see more info go to rfc2132
 * */
//forward declarations
namespace DHCP{
	
	struct Header;
	struct Option{
		unsigned int type:8;
		int length:8;
		char data[256] = {0};
	};
	namespace OPTIONS{
		
		enum types{
			subnet_mask = 1,time_offset = 2,router =3,time_server = 4,name_server = 5,
			dns = 6,log_server = 7,cookie_server = 8,lpr_server = 9,impress_server = 10,
			resource_location_server = 11,host_name = 12,boot_file_size = 13,merit_dump_file = 14,
			domain_name = 15,interface_mtu = 26, broadcast_address = 28,static_route = 33,	
			requested_ip_address = 50,ip_address_lease_time = 51,message_type = 53,
			server_identifier = 54, renewal_time_val = 58,rebinding_time_value = 59, 
			client_identifier = 61,param_request_list = 55,max_message_size = 57, 
			class_identifier = 60, domain_search = 119,classless_static_route = 121, 
			WPAD = 252, end = 255
		};
		void logValues(Option * option);
		void logValuesFromHeader(DHCP::Header * src);
	}
	
}


#include "../ethernetHeader.cpp"
#include "../DHCPHeader.cpp"
#include <string>


namespace DHCP::OPTIONS{
	
	namespace Message{
		enum types{
			DISCOVER = 1, OFFER = 2, REQUEST = 3, 
				 DECLINE = 4, ACK = 5, NAK = 6, RELEASE = 7
		};
		
		void create(DHCP::Option *val, types type){
			val->type = DHCP::OPTIONS::types::message_type;
			val->length = 1;
			val->data[0] = type;
		}
		
		void logValues(Option * val){
			if(val->type != DHCP::OPTIONS::types::message_type){ throw -1; }
			std::cout<<"MESSAGE:";
			switch(val->data[0]){
				case DISCOVER:
					std::cout<<"DISCOVER\n";
					break;
				case OFFER:
					std::cout<<"OFFER\n";
					break;
				case DECLINE:
					std::cout<<"DECLINE\n";
					break;
				case ACK:
					std::cout<<"ACK\n";
					break;
				case NAK:
					std::cout<<"NAK\n";
					break;
				case RELEASE:
					std::cout<<"RELEASE\n";
					break;
				default:
					std::cout<<"unknown\n";
			}
		}
	
	}
	
	namespace SubnetMask{
		
		void create(DHCP::Option *val, long mask){
			val->type = DHCP::OPTIONS::types::subnet_mask;
			val->length = 4;
			IP::parseLongIpAddress( mask, val->data);
		}
		void logValues(Option * val){
			std::cout<<"SubnetMask:";
			std::cout<< (int)(val->data[0]) << ", "<< (int)(val->data[1]) << ", "
				 << (int)(val->data[2]) << ", "<< (int)(val->data[3]) <<"\n";
		}
	
	}
	
	namespace Router{
		void create(DHCP::Option * val, unsigned long values[], int length){
			val->type = DHCP::OPTIONS::types::router;
			val->length = length*4;
			
			char *arry = val->data;
			for(int i = 0; i+3<length*4; i+=4){
				char ip[4] = {0};
				IP::parseLongIpAddress(values[i/4], ip);
				arry[0] = ip[0];
				arry[1] = ip[1];
				arry[2] = ip[2];
				arry[3] = ip[3];
				arry += 4;
			}
			//memcpy(val->data, values, length*4);
		}
		void logValues(Option * val){
			std::cout<<"Router:\n";
			for(int i =0; i<val->length/4; i+=4){
				std::cout<<"IP:"<< (int)(val->data[i]) << ", "<< (int)(val->data[i+1]) << ", "
				 << (int)(val->data[i+2]) << ", "<< (int)(val->data[i+3]) <<"\n";
			}
		}
	}
	
	namespace LeaseTime{
		void create(DHCP::Option * val, long timeSeconds){
			val->type = DHCP::OPTIONS::types::ip_address_lease_time;
			val->length = 4;
			IP::parseLongIpAddress(timeSeconds, val->data);
		}
		void logValues(Option * val){
			std::cout<<"Router:\n"<<((long) val->data)<<"\n";
		}
	}
	
	namespace ServerId{
		void create(DHCP::Option *val, long ipAddress){
			val->type = DHCP::OPTIONS::types::server_identifier;
			val->length = 4;
			IP::parseLongIpAddress( ipAddress, val->data);
		}
		void logValues(Option * val){
			std::cout<<"ServerId:\n";
			std::cout<< (int)(val->data[0]) << ", "<< (int)(val->data[1]) << ", "
				 << (int)(val->data[2]) << ", "<< (int)(val->data[3]) <<"\n";
		}
	}
	
	namespace DNS{
		void create(DHCP::Option *val, unsigned long serverIps[], int length){
			val->type = DHCP::OPTIONS::types::dns;
			val->length = length*4;
			memcpy(val->data, serverIps, length*4);
			
		}
		void logValues(Option * val){
			std::cout<<"Router:\n";
			for(int i =0; i<val->length/4; i+=4){
				std::cout<<"IP:"<< (int)(val->data[i]) << ", "<< (int)(val->data[i+1]) << ", "
				 << (int)(val->data[i+2]) << ", "<< (int)(val->data[i+3]) <<"\n";
			}
		}
	}
	
	namespace ClientId{
		void logValues(Option * val){
			char data[256] = {0};
			memcpy(data, val->data, val->length);
			std::cout<<"ClientId:"<<data<<"\n"; 
		}
	}
	
	namespace MaxMessageSize{
		void logValues(Option * val){
			std::cout<<"MaxMessageSize:"<<((uint16_t *)val->data)[0]<<"\n"; 
		}
	}
	
	namespace VendorClassId{
		void logValues(Option * val){
			char data[256] = {0};
			memcpy(data, val->data, val->length);
			std::cout<<"VendorClassId:"<<data<<"\n"; 
		}
	}
	
	namespace ParamRequestList{
		void logValues(Option * val){
			std::cout<<"ParamRequestList:";
			for(int i = 0; i<val->length; i++){
				std::cout<<","<<(int)val->data[i];
			}
			std::cout<<"\n";
		}
	}
}




namespace DHCP::OPTIONS{
	
		int sizeOf(DHCP::Option *val){
			return (2+ val->length)*sizeof(char);
		}
		
		void memCpy(Option *dest, Option * src){
			memcpy(dest, src, sizeOf(src));
		}
		
		
		void logValues(Option * option){
			switch(option->type){
				case types::message_type:
					Message::logValues(option);
					return;
				case types::subnet_mask: 
					SubnetMask::logValues(option);
					return;
				case types::router:
					Router::logValues(option);
					return;
				case types::ip_address_lease_time: 
					LeaseTime::logValues(option);
					return;
				case types::server_identifier: 
					ServerId::logValues(option);
					return;
				case types::dns: 
					DNS::logValues(option);
					return ;
				case types::client_identifier: 
					ClientId::logValues(option);
					return ;
				case types::max_message_size: 
					MaxMessageSize::logValues(option);
					return ;
				case types::class_identifier: 
					VendorClassId::logValues(option);
					return ;
				case types::param_request_list: 
					ParamRequestList::logValues(option);
					return ;
					
					
					
				case types::end: 
					std::cout<<"end Option\n";
					return;
				
				default: 
					std::cout<<"unknown type:"<<option->type<<"\n";std::cout.flush();
					char data[256] = {0};
					memcpy(data, option->data, option->length);
					std::cout<<"type:"<<option->type; std::cout.flush();
					std::cout<<", length:"<<(int)option->length;  std::cout.flush();
					std::cout<<", data-string:"<<data; std::cout.flush();
					std::cout<<", data-int:"<<(int)option->data[0]; std::cout.flush();
					std::cout<<"\n"; std::cout.flush();
			}
			
		}
		
		void logValuesFromHeader(DHCP::Header * src){
			//std::cout<<"log values from header\n"; std::cout.flush();
			char *bytes = ((char *)src) +DHCP::optionsIndex;
			int ptr = (int)bytes;
			//std::cout<<"log values from header\n"; std::cout.flush();
			DHCP::Option * retu = (DHCP::Option *) bytes;
			//std::cout<<"log values from header\n"; std::cout.flush();
			
			for(int i = 0; i<100; i++){
				
				std::cout<<"type:"<<retu->type; std::cout.flush();
				logValues(retu);
				bytes++;//one for type field
				bytes++;//one for length field
				
				if(retu->type == types::end || retu->type == 0){
					int ptr2 = (int)bytes;
					if(ptr2-ptr != DHCP::getTotalHeaderLength(src)-DHCP::optionsIndex){
						std::cout<<"incorect length:"<<(ptr2-ptr)<<","<<( DHCP::getTotalHeaderLength(src)-DHCP::optionsIndex)<<"\n";
						std::cout.flush();
						throw -8;
					}
					return;
				}
				
				bytes+= retu->length; //one for length
				retu = (DHCP::Option *) bytes;
			}
		}
		
		DHCP::Option *getByType(DHCP::Header *dhcp, DHCP::OPTIONS::types type){
			char *bytes = ((char *)dhcp) +sizeof(Header);
			DHCP::Option * retu = (DHCP::Option *) bytes;
			
			while(true){
				if(retu->type == type){
					return retu; 
				}
				else if(retu->type == DHCP::OPTIONS::types::end){
					return NULL;
				}
				
				bytes++;//once for the type field
				bytes++;//once for the length field
				bytes+= retu->length; //once for the length itself
				retu = (DHCP::Option *) bytes;
			}
			std::cout<<"\n\nfailed to find option of name:"<<type<<"\n\n";
			logValuesFromHeader(dhcp);
			
			return NULL;
		}
		
}

#endif

	

