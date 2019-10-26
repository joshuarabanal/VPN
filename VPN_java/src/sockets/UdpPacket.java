/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package sockets;

import sockets.editable.TcpEditable;

/**
 *
 * @author root
 */
public class UdpPacket {
    public static final int UDP_protocol = 17, payload_start_index= 8;
    
    private int offset;
    private byte[]b ;
    
    public UdpPacket(byte[] b, int offset){
        this.b = b; this.offset = offset;
    }
    public int getSourcePort(){
        return TcpEditable.getShort(0, b);
    }
    public int getDestPort(){
        return TcpEditable.getShort(2, b);
    }
    public int getLength(){
        return TcpEditable.getShort(4, b);
    }
    /**
     * 
     * @return true if the checksum passes, or false if the checksum fails
     */
    public boolean checksum(int sourceIp, int destIp, int totalLength){
        int checksum = TcpEditable.getShort(6,b);
        if(checksum == 0){
            return true;
        }
        
        long sum = 0;
        //source ip
        sum+= (sourceIp>>16 )& 0xFFFF;
        sum+= sourceIp& 0xffff;
              
        //dest ip
        sum+= (destIp>>16)& 0xFFFF;
        sum+= destIp & 0xffff;
        
        // 8 bit 0
        
        //protocol
        sum+=  (Socket.TCP_protocol);
        
        sum+=(~Socket.checksum(b, 0, b.length))&0xffff;
        
        while(sum>0xFFFF){
            sum = (sum&0xffff) + (sum>>16);
        }
        
        return ((~sum)&0xffff) == 0;
    }
    
}
