/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package gateway;

import static sockets.editable.TcpEditable.getInt;
import static sockets.editable.TcpEditable.getShort;


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
        public static int getSourcePort(byte[]b){
            return getShort(getIPHeaderLength(b), b);
        }
        public static int getDestPort(byte[]b){
            return getShort(getIPHeaderLength(b)+2, b);
        }
        public static int getLength(byte[]b){
            return getShort(getIPHeaderLength(b)+4, b);
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
    
    
}
