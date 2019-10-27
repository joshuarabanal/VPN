/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package dhcp;

import dhcp.dhcpPacket.Options;
import static sockets.editable.TcpEditable.setInt;
import static sockets.editable.TcpEditable.setShort;

/**
 *
 * @author root
 */
public class DhcpPacketBuilder {
    private byte[] b = new byte[236];
    public Options options = new Options();
    
    public void initializeFromCopy(byte[] b){
            this.setOP(DHCPPacket.getOP(b))
                .setHTYPE(DHCPPacket.getHTYPE(b))
                .setHLEN(DHCPPacket.getHLEN(b))
                .setHOPS(DHCPPacket.getHOPS(b))
                .setXID(DHCPPacket.getXID(b))
                .setSECS(DHCPPacket.getSECS(b))
                .setFLAGS(DHCPPacket.getFLAGS(b))
                .setClient_IP_address(DHCPPacket.getClient_IP_address(b))
                .setYour_IP_address(DHCPPacket.getYour_IP_address(b))
                .setServer_IP_address(DHCPPacket.getServer_IP_address(b))
                .setGateway_IP_address(DHCPPacket.getGateway_IP_address(b))
                .setClient_hardware_address(DHCPPacket.getClient_hardware_address(b))
                .setServer_host_name(DHCPPacket.getServer_host_name(b))
                .setBoot_file_name(DHCPPacket.getBoot_file_name(b));
        this.options = DHCPPacket.getOptions(b);
    }
    
    public DhcpPacketBuilder setOP(byte op){   b[0] = op;  return this; }
    
    public DhcpPacketBuilder setHTYPE(byte htype){   b[1] = htype;  return this; }
    
    public DhcpPacketBuilder setHLEN(byte hlen){ b[2] = hlen;  return this; }
    
    public DhcpPacketBuilder setHOPS(byte hops){ b[3] = hops;  return this; }
    
    public DhcpPacketBuilder setXID(int xid){  setInt(xid,4,b);  return this; }
    
    public DhcpPacketBuilder setSECS(int secs){ setShort(secs,8,b);  return this; }
    
    public DhcpPacketBuilder setFLAGS(int flags){ setShort(flags,10,b);  return this; }
    
    
    public DhcpPacketBuilder setClient_IP_address(int val){ setInt(val,12,b);  return this; }
    
    public DhcpPacketBuilder setYour_IP_address(int val){ setInt(val,16,b);  return this; }
    
    public DhcpPacketBuilder setServer_IP_address(int val){ setInt(val,20,b);  return this; }
    
    public DhcpPacketBuilder setGateway_IP_address(int val){ setInt(val,24,b);  return this; }
    
    public DhcpPacketBuilder setClient_hardware_address(int[] val){ 
        for(int i = 0; i<val.length; i++){
            setInt(val[i],28+(i*4),b);
        }
     return this; 
    }
    public DhcpPacketBuilder setServer_host_name(String val){ 
        byte[] retu = val.getBytes();
        if(retu.length>64){ throw new IndexOutOfBoundsException("string too long"); }
        int length = 0;
        for(int i = 0; i<retu.length; i++){
            b[44+i] = retu[i];
        }
        for(int i = retu.length; i<64; i++){
            b[44+1] = 0;
        }
        return this;
    }
    public DhcpPacketBuilder setBoot_file_name(String val){ 
        byte[] retu = val.getBytes();
        int length = 0;
        for(int i = 0; i<(retu.length); i++){
             b[108+i] = retu[i];
        }
        for(int i = retu.length; i<64; i++){
            b[108+1] = 0;
        }
        return this;
    }
    public byte[] build(){
        byte[] options = this.options.toByteArray();
        byte[] retu = new byte[this.b.length+options.length];
        for(int i = 0; i<this.b.length; i++){
            retu[i] = this.b[i];
        }
        for(int i = 0; i<options.length; i++){
            retu[i+this.b.length] = options[i];
        }
        
        return retu;
    }
}
