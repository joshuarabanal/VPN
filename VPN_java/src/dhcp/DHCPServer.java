/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package dhcp;

import gateway.IpPacket;
import dhcp.dhcpPacket.Options;
import dhcp.dhcpPacket.Option;
import java.util.ArrayList;
import sockets.Socket;
import sockets.editable.SocketEditable;

/**
 *
 * @author root
 */
public class DHCPServer {
    private static final int subnetMask = Socket.ipStringToInt("255.255.255.0"), 
            serverIpAddress = Socket.ipStringToInt("192.168.1.0"),
            serverIdentifier = Socket.ipStringToInt("192.168.1.1"),
            broadcastAddress = Socket.ipStringToInt("192.168.1.101"),
            addressLeaseTime = 60*60*24,
            renewalLeaseTime = 60*60*12,
            rebindingLeaseTime = 60*60*21;
    
    ArrayList<Integer> usedSubnets = new ArrayList<Integer>();
    
    public boolean accept(byte[] b){
        if(IpPacket.UDPPacket.getSourcePort(b) != 68 || IpPacket.UDPPacket.getDestPort(b) != 67){
            return false;
        }
        Options opts = DHCPPacket.getOptions(b);
        switch(opts.getMessageType() ){
            case Option.MessageType.DHCPDISCOVER:
                discover(b, opts);
                return true;
            default: 
                System.out.println("unable to procss message type:"+opts.getMessageType());
                return true;
        }
    }
    private int getAvailableIp(){
        for(int i = 2; i<100; i++){
            boolean used = false;
            for(int clientSubnet: usedSubnets){
                if(clientSubnet == i){
                    used = true;
                    break;
                }
            }
            if(!used){ return i; }
        }
        throw new IndexOutOfBoundsException("no available clients");
    }
    private void discover(byte[] b, Options opts){
        DhcpPacketBuilder builder = new DhcpPacketBuilder();
        
        builder.initializeFromCopy(b);
        builder.setOP((byte)2);
        builder.setServer_IP_address(serverIpAddress);
        builder.setClient_IP_address(Socket.ipStringToInt("192.168.1."+getAvailableIp()));
        builder.options = buildResponseOptions(opts);
        
        //add the default options
        
        //configure message type 
        Option o = builder.options.findOptionByType(Option.type_message_type);
        if(o == null){
            o= new Option.MessageType(Option.MessageType.DHCPOFFER);
            builder.options.add(o);
        }
        
        //subnetMask
        o = builder.options.findOptionByType(Option.type_subnet_mask);
        if(o == null){
            o= new Option.SubnetMask(subnetMask);
            builder.options.add(o);
        }
        
        //router
        o = builder.options.findOptionByType(Option.type_router);
        if(o == null){
            o = buildResponseByType((byte)Option.type_router);
            builder.options.add(o);
        }
        
        //lease time
        o = builder.options.findOptionByType(Option.type_ip_address_lease_time);
        if(o == null){
            o = buildResponseByType((byte)Option.type_ip_address_lease_time);
            builder.options.add(o);
        }
        
        //server identifier
        o = builder.options.findOptionByType(Option.type_server_identifier);
        if(o == null){
            o = buildResponseByType((byte)Option.type_server_identifier);
            builder.options.add(o);
        }
        
        //dns servers
        o = builder.options.findOptionByType(Option.type_dns);
        if(o == null){
            o = buildResponseByType((byte)Option.type_dns);
            builder.options.add(o);
        }
        SocketEditable se = new SocketEditable(b);
        
        byte[] dhcp = builder.build();
        
        
    }
    private Options buildResponseOptions(Options request){
        Options retu = new Options();
        System.out.println("build response options:\n"+request.toString());
        byte[] list = request.getParamRequestList();
        for(byte i : list){
            Option o = buildResponseByType(i);
            if(o != null){ retu.add(o); }
        }
        return retu;
    }
    private Option buildResponseByType(byte type){
        switch(type&0xff){
            case Option.type_subnet_mask:
                return new Option.SubnetMask( subnetMask);
                
            case Option.type_classless_static_route:
                System.out.println("skipping classless static route because its confusing");
                return null;
                
            case Option.type_static_route:
                return new Option.StaticRoute( 
                        new int[]{
                            Socket.ipStringToInt("0.0.0.0"), serverIpAddress,
                            Socket.ipStringToInt("192.168.1.0"), serverIpAddress
                        } 
                );
                
            case Option.type_router:
                return new Option.Router(serverIpAddress);
                
            case Option.type_dns:
                return new Option.DNS(
                        Socket.ipStringToInt("8.8.8.8"),
                        Socket.ipStringToInt("8.8.4.4")
                );
                
            case Option.type_host_name:
                return new Option.HostName("192.168.0.1");
                
            case Option.type_domain_name:
                System.out.println("skipping domain name because its confusing, and appears to be unneccessary");
                return null;
                
            case Option.type_interface_mtu:
                return new Option.InterfaceMTU(1500);
                
            case Option.type_ip_address_lease_time:
                return new Option.IpAddressLeaseTime(addressLeaseTime);
                
            case Option.type_server_identifier:
                return new Option.ServerID(this.serverIdentifier);
            
            case Option.type_broadcast_address:
                return new Option.BroadcastAddress(broadcastAddress);
                
            case Option.type_renewal_time_val:
                return new Option.RenewalTimeValue(renewalLeaseTime);
            
            case Option.type_rebinding_time_value:
                return new Option.RebindingTimeValue(rebindingLeaseTime);
                
            case Option.type_domain_search:
                System.out.println("DNS search options were removed since it requires domain_name to be used");
                return null;
                
            case Option.type_WPAD:
                System.out.println("WPAD(web proxy auto discovery) is removed since ther is no proxy");
                return null;
                
            default: 
                
                throw new IndexOutOfBoundsException("cannot respond to:"+type+"="+Option.typeToString(type));
        }
    }
    
}
