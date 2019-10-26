/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package gateway;

import sockets.editable.TcpEditable;

/**
 *
 * @author root
 */
public class IpPacket {
    public static final int tcpStartIndex = 20;
    public static class TCPPacket{
        public static int getSourcePort(byte[] b){
            return TcpEditable.getShort(tcpStartIndex, b);
        }
        public static int getDestPort(byte[] b){
            return TcpEditable.getShort(tcpStartIndex+2, b);
        }
    }
    
    
    public static int getSourceIp(byte[] b){
        return TcpEditable.getInt(12, b);
    }
    public static int getDestIp(byte[] b){
        return TcpEditable.getInt(16, b);
    }
    public static int getProtocol(byte[] b){
        return b[9];
    }
    
}
