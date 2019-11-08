/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package test;

import dhcp.DHCPServer;
import gateway.TCPServer;
import gateway.UDPServer;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.util.logging.Level;
import java.util.logging.Logger;
import sockets.RawSocket;
import udp.PrivateIpHandler;

/**
 *
 * @author joshuarabanal
 */
public class Test {
    public static final RawSocket rawTCP = RawSocket.initialize_TCP("eth0");
    public static final RawSocket rawUDP = RawSocket.initialize_UDP("eth0");
    public static DHCPServer dhcp;
    public static PrivateIpHandler priv;
    
    public static void main(String[] args){
        try{
            dhcp = new DHCPServer(rawUDP);
            priv = new PrivateIpHandler( rawTCP, rawUDP, dhcp);

            System.setErr(System.out);
            System.out.println(System.getProperty("user.dir"));
            TCPServer tcps = new TCPServer(dhcp,priv);
            UDPServer udps = new UDPServer(dhcp,priv);
                udps.start();
           tcps.run();
        }
        catch(Exception e){
            e.printStackTrace();
            return;
        }
        
       
    }
    
   
}
