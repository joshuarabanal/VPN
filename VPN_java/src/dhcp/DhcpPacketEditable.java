/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package dhcp;

import dhcp.dhcpPacket.Options;

/**
 *
 * @author root
 */
public class DhcpPacketEditable {
    public Options o;
    
    public DhcpPacketEditable(byte[] b){
        o = DHCPPacket.getOptions(b);
        int end = DHCPPacket.getOptionsStartIndex(b);
    }
    
}
