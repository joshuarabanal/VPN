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
import sockets.Socket;
import sockets.TcpPacket;

/**
 *
 * @author root
 */
public class Server implements Runnable {
    private RawSocket serverSocket = new RawSocket();
    private int clientIp = Socket.ipStringToInt("192.168.1.11");
    private int serverIp = Socket.ipStringToInt("192.168.1.12"), forwardingIp = Socket.ipStringToInt("72.188.192.147");
    
    private ArrayList<ConnectedDevice> clients = new ArrayList<ConnectedDevice>();
    
    public Server(){
        clients.add( new ConnectedDevice(clientIp, serverIp, serverSocket));
    }
    
    /**
     * runs the server on the current thread
     */
    public void run(){
        while(true){
         
            Socket s = null;
            try {
                s = serverSocket.accept();
                if(s == null){ throw new IOException("socket is null"); }
                
                
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
                
                
                    if(s.destinationIpAddress == forwardingIp){
                        System.out.println("response:"+s.toString());
                    }
                    else if(s.sourceIpAddress == serverIp){
                        System.out.println("response:"+s.toString());
                    }
                    
                    
                    
                    
                
                if(s.destinationIpAddress == serverIp && s.sourceIpAddress == clientIp){
            System.out.println();  System.out.println(); System.out.println(); System.out.println();
                    System.out.println("found correct Ip: "+ s.toString());
                        s.bindPacket();
                }
            
            } catch (IOException ex) {
                        Logger.getLogger(Server.class.getName()).log(Level.SEVERE, null, ex);
                        if(s !=null){
                            System.out.println(s.toString());
                        }
                        return;
            }
            
           // System.out.println("packet["+b.length+"]:"+new String(b,0,b.length));
        }
    }
    
}
