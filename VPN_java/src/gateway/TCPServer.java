/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package gateway;

import gateway.server.ConnectedDevice;
import java.io.IOException;
import java.util.ArrayList;
import java.util.logging.Level;
import java.util.logging.Logger;
import sockets.RawSocket;
import sockets.IpPacket;
import sockets.IpPacket_deprecated;

/**
 *
 * @author root
 */
public class TCPServer implements Runnable {
    private RawSocket TCP_serverSocket = RawSocket.initialize_TCP("wlan0");
    private int clientIp = IpPacket.ipStringToInt("192.168.1.11");
    private int serverIp = IpPacket.ipStringToInt("192.168.1.12"), forwardingIp = IpPacket.ipStringToInt("72.188.192.147");
    
    private ArrayList<ConnectedDevice> clients = new ArrayList<ConnectedDevice>();
    
    public TCPServer(){
        clients.add( new ConnectedDevice(clientIp, serverIp, TCP_serverSocket));
    }
    
    /**
     * runs the server on the current thread
     */
    public void run(){
        while(true){
         
            byte[] s = null;
            try {
                s = TCP_serverSocket.accept();
                if(s == null){ throw new IOException("socket is null"); }
                /**
                System.out.println(
                        "ip:"+
                        Socket.ipIntToString(s.sourceIpAddress)+
                        "=>"+
                        Socket.ipIntToString(s.destinationIpAddress)
                );
                * **/
                
                //System.out.println("new packet:"+s.sourceIpAddress+"=>"+s.destinationIpAddress);
                
                //check if the packet belongs to a connected client
                    for(int i = 0;i<clients.size(); i++){
                        if(clients.get(i).accept(s)){
                            s = null;//this packet was used up
                        }
                    }
                    
                
                if(s == null){
                    continue;//packet was consumed by client
                }
                
                
                  
                    
                    
                    
                    
                
                if(IpPacket.getDestIp(s) == serverIp && IpPacket.getSourceIp(s) == clientIp){
                    System.out.println("\n\n\n");
                    System.out.println("found correct Ip: "+ s.toString());
                        new IpPacket_deprecated(s, TCP_serverSocket).bindPacket();
                }
            
            } catch (IOException ex) {
                        Logger.getLogger(TCPServer.class.getName()).log(Level.SEVERE, null, ex);
                        if(s !=null){
                            System.out.println(s.toString());
                        }
                        return;
            }
            
           // System.out.println("packet["+b.length+"]:"+new String(b,0,b.length));
            
           // System.out.println("packet["+b.length+"]:"+new String(b,0,b.length));
        }
    }
    
}
