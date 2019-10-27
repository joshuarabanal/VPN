/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package gateway;

import static sockets.editable.TcpPacketBuilder.getInt;
import static sockets.editable.TcpPacketBuilder.getShort;


/**
 *
 * @author root
 */
public class IpPacket {
    public static class TCPPacket{
        public static int getSourcePort(byte[] b){
            return getShort(getIPHeaderLength(b), b);
        }
        public static int getDestPort(byte[] b){
            return getShort(getIPHeaderLength(b)+2, b);
        }
    }
    
    public static class UDPPacket{
        private static final int UDP_header_length  = 8;
        public static int getSourcePort(byte[]b){
            return getShort(getIPHeaderLength(b), b);
        }
        public static int getDestPort(byte[]b){
            return getShort(getIPHeaderLength(b)+2, b);
        }
        public static int getLength(byte[]b){
            return getShort(getIPHeaderLength(b)+4, b);
        }
        public static int getChecksum(byte[]b){
            return getShort(getIPHeaderLength(b)+6, b);
        }
        public static int getPayloadStartIndex(byte[]b){
            return getIPHeaderLength(b)+UDP_header_length;
        }
        public static String toString(byte[] b){
            StringBuilder sb = new StringBuilder("UDP packet:");
            sb.append("\nsource port:").append(getSourcePort(b));
            sb.append("\ndest port:").append(getDestPort(b));
            sb.append("\nlength:").append(getLength(b));
            sb.append("\ncheck sum:").append(getChecksum(b));
            return sb.toString();
        }
    }
    
    public static int getVersion(byte[]b){
        return (b[0]&0xf0)>>4;
    }
    /**@return  length of IP header in bytes**/
    public static int getIPHeaderLength(byte[] b){
        return ((b[0]&0x0f))*4;
    }
    public static int getTypeOfService(byte[] b){ return b[1]; }
    
    public static int getTotalLength(byte[] b){
        return getShort(2,b);
    }
     public static int getId(byte[] b){ return getShort(4,b); }
     
    public static int getFragOffset(byte[]b){ return getShort(6,b); }
    
    public static int getTTL(byte[]b){ return b[8];}
    
    public static int getProtocol(byte[] b){
        return b[9];
    }
    
    public static int getCheckSum(byte[]b){ return getShort(10,b); }
    
    public static int getSourceIp(byte[] b){
        return getInt(12, b);
    }
    public static int getDestIp(byte[] b){
        return getInt(16, b);
    }
    
    public static  String toString(byte[] b){
        StringBuilder sb = new StringBuilder("IP packet:");
        sb.append("\nversion:").append(getVersion(b));
        sb.append("\nip header length:").append(getIPHeaderLength(b));
        sb.append("\ntype of service:").append(getTypeOfService(b));
        sb.append("\n total length:").append(getTotalLength(b));
        sb.append("\nID:").append(getId(b));
        sb.append("\nFrag ofset:").append(getFragOffset(b));
        sb.append("\nTTL:").append(getTTL(b));
        sb.append("\nprotocol:").append(getProtocol(b));
        sb.append("\ncheck sum:").append(getCheckSum(b));
        sb.append("\nsource Ip:").append(getSourceIp(b));
        sb.append("\ndest ip:").append(getDestIp(b));
        return sb.toString();
    }
    
    
}
