/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package gateway;

import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import sockets.RawSocket;
import sockets.Socket;

/**
 *
 * @author root
 */
public class UDPServer {
    private RawSocket sock = RawSocket.initialize_UDP();
    private int clientIp = Socket.ipStringToInt("192.168.1.11");
    private int serverIp = Socket.ipStringToInt("192.168.1.12"), forwardingIp = Socket.ipStringToInt("72.188.192.147");
    
    public void run(){
        while(true){
            try {
                byte[] b = sock.accept();
                System.out.println("packetRecieved:"+
                        Socket.ipIntToString(IpPacket.getSourceIp(b))
                        +":"+
                        IpPacket.UDPPacket.getSourcePort(b)+
                        "=>"+
                        Socket.ipIntToString(IpPacket.getDestIp(b))
                        +":"+
                        IpPacket.UDPPacket.getSourcePort(b)
                        
                );
                
            } catch (IOException ex) {
                Logger.getLogger(UDPServer.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
}
