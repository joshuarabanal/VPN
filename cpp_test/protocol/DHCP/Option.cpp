#include "../ethernetHeader.cpp"

namespace DHCP{
	struct Option{
		char type = -1;
		char length = 0;
		char data[256] = {0};
	};
	
}

namespace DHCP::OPTIONS{
	
		int sizeOf(DHCP::Option *val){
			return (2+ val->length)*sizeof(char);
		}
		enum types{
			subnet_mask = 1,time_offset = 2,router =3,time_server = 4,name_server = 5,
			dns = 6,log_server = 7,cookie_server = 8,lpr_server = 9,impress_server = 10,
			resource_location_server = 11,host_name = 12,boot_file_size = 13,merit_dump_file = 14,domain_name = 15,
            interface_mtu = 26,
            broadcast_address = 28,
            static_route = 33,
            requested_ip_address = 50,
            ip_address_lease_time = 51,
            message_type = 53,
            server_identifier = 54,
            renewal_time_val = 58,
            rebinding_time_value = 59,
            client_identifier = 61,
            param_request_list = 55,
            max_message_size = 57,
            class_identifier = 60,
            domain_search = 119,
            classless_static_route = 121,
            WPAD = 252,
            end = 255
		};

}

namespace DHCP::OPTIONS::Message{
	enum types{
		DISCOVER = 1, OFFER = 2, REQUEST = 3, 
			 DECLINE = 4, ACK = 5, NAK = 6, RELEASE = 7
	};
	void create(DHCP::Option *val, types type){
		val->type = DHCP::OPTIONS::types::message_type;
		val->length = 1;
		val->data[0] = type;
	}
}

namespace DHCP::OPTIONS::SubnetMask{
	void create(DHCP::Option *val, long mask){
		val->type = DHCP::OPTIONS::types::message_type;
		val->length = 4;
		IPPacket::parseLongIpAddress( mask, val->data);
	}
}

namespace DHCP::OPTIONS::Router{
	void create(DHCP::Option * val, long values[], int length){
		val->type = DHCP::OPTIONS::types::router;
		val->length = length*4;
		memcpy(val->data, values, length*4);
	}
}

namespace DHCP::OPTIONS::LeaseTime{
	void create(DHCP::Option * val, long timeSeconds){
		val->type = DHCP::OPTIONS::types::ip_address_lease_time;
		val->length = 4;
		IPPacket::parseLongIpAddress(timeSeconds, val->data);
	}
}
namespace DHCP::OPTIONS::ServerId{
	void create(DHCP::Option *val, long ipAddress){
		val->type = DHCP::OPTIONS::types::server_identifier;
		val->length = 4;
		IPPacket::parseLongIpAddress( ipAddress, val->data);
	}
}

namespace DHCP::OPTIONS::DNS{
	void create(DHCP::Option *val, long serverIps[], int length){
		val->type = DHCP::OPTIONS::types::dns;
		val->length = length*4;
		memcpy(val->data, serverIps, length*4);
		
	}
}

	

