/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package dhcp;

import gateway.IpPacket;
import dhcp.dhcpPacket.Options;
import dhcp.dhcpPacket.Option;

/**
 *
 * @author root
 */
public class DHCPServer {
    
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
    private void discover(byte[] b, Options opts){
        
    }
    
}
