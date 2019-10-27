/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package gateway;

import dhcp.DHCPPacket;
import java.io.IOException;
import java.util.Arrays;
import java.util.logging.Level;
import java.util.logging.Logger;
import sockets.RawSocket;
import sockets.Socket;

/**
 *
 * @author root
 */
public class UDPServer {
    private RawSocket sock = RawSocket.initialize_UDP("eth0");
    private int clientIp = Socket.ipStringToInt("192.168.1.11");
    private int serverIp = Socket.ipStringToInt("192.168.1.12"), forwardingIp = Socket.ipStringToInt("72.188.192.147");
    
    public void run(){
                dhcp.DHCPServer dhcp = new dhcp.DHCPServer(sock);
        while(true){
            try {
                System.out.println("\n\n\n\nread loop:");
                
                byte[] b = sock.accept();
                if(dhcp.accept(b)){ continue; }//check if its a dhcp packet
                
                System.out.println("regular packetRecieved:"+
                        Socket.ipIntToString(IpPacket.getSourceIp(b))
                        +":"+
                        IpPacket.UDPPacket.getSourcePort(b)+
                        "=>"+
                        Socket.ipIntToString(IpPacket.getDestIp(b))
                        +":"+
                        IpPacket.UDPPacket.getDestPort(b)
                        
                );
                
                    try{
                        System.out.println("unknonwn ip packet:"+IpPacket.toString(b));
                        System.out.println("unknown UDP packet:"+IpPacket.UDPPacket.toString(b));
                        System.out.println(
                                "array"+Arrays.toString(
                                    Arrays.copyOfRange(b, IpPacket.UDPPacket.getPayloadStartIndex(b), b.length)
                                )
                        );
                    }catch (IndexOutOfBoundsException e){
                        throw e;
                    }
                
                
            } catch (IOException ex) {
                Logger.getLogger(UDPServer.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
}
