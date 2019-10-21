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
import sockets.TcpPacket;

/**
 *
 * @author root
 */
public class Server implements Runnable {
    private RawSocket serverSocket = new RawSocket();
    private int clientIp = Socket.ipStringToInt("192.168.1.11");
    private int serverIp = Socket.ipStringToInt("192.168.1.12");
    
    /**
     * runs the server on the current thread
     */
    public void run(){
        while(true){
         
            Socket s = null;
            try {
                //logPacket(b)
            
                s = serverSocket.accept();
                if(s.destinationIpAddress == serverIp && s.sourceIpAddress == clientIp){
            System.out.println();  System.out.println(); System.out.println(); System.out.println();
                    System.out.println("found correct Ip: "+ s.toString());
                        s.bindPacket();
                }
                else{
                    /*System.out.println("not correct Ip: "+ 
                            Socket.ipIntToString(s.sourceIpAddress));*/
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
