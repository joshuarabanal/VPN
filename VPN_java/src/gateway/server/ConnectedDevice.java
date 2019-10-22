/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package gateway.server;

import java.io.IOException;
import java.util.ArrayList;
import sockets.RawSocket;
import sockets.Socket;
import sockets.TcpPacket;

/**
 *
 * @author root
 */
public class ConnectedDevice {
    private final int clientIp, gatewayIp;
    private ArrayList<IpPacket> requests = new ArrayList<IpPacket>();
    private RawSocket rawOut;
    
    public ConnectedDevice(int ClientIp, int gatewayIp, RawSocket outputStream){
        this.clientIp = ClientIp;
        this.gatewayIp = gatewayIp;
        this.rawOut = outputStream;
    }
    
    public boolean accept(Socket s) throws IOException{
        
        for(IpPacket p : requests){
            if(p.accept(s)){
                return true;
            }
        }
        if(s.sourceIpAddress == clientIp){
            System.out.println("making new packet");
            IpPacket req = new IpPacket(s, rawOut, gatewayIp);
            requests.add(req);
            
            if(!req.accept(s)){ throw new IOException(); }
            return true;
        }
            return false;
    }
    
}
