
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package gateway.server;

import gateway.IpPacket;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import sockets.RawSocket;
import sockets.Socket;
import sockets.editable.SocketEditable;
import sockets.editable.TcpEditable;

/**
 *
 * @author root
 */
public class IpPacketHolder {
    public final int clientIp, clientPort, serverIp, serverPort, gatewayIp, 
            forwardingIp = Socket.ipStringToInt("72.188.192.147");
    private Socket firstSocket;
    private RawSocket outStream;
    private ArrayList<Socket> requests = new ArrayList<Socket>();

    
    public IpPacketHolder(Socket s, RawSocket out, int gatewayIp) throws IOException{
        firstSocket = s;
        outStream = out;
        this.gatewayIp = gatewayIp;
        clientIp = s.sourceIpAddress;
        serverIp = s.destinationIpAddress;
        clientPort = s.getTcp().sourcePort;
        serverPort = s.getTCP().destinationPort;
    }
    
    public boolean accept(byte[] s) throws IOException{
        
        
        
        if(
                IpPacket.getSourceIp(s) == clientIp && //comes from our client
                IpPacket.TCPPacket.getSourcePort(s) == clientPort &&//from our process
                IpPacket.getDestIp(s) == serverIp &&//goes to our server
                IpPacket.TCPPacket.getDestPort(s)== serverPort//on the correct server port
        ){
            forwardToServer(new Socket(s,outStream) );
            return true;
        }
        else if(
                IpPacket.getSourceIp(s)== forwardingIp &&//came from our forwarding server
                    IpPacket.TCPPacket.getSourcePort(s) == serverPort &&//on server port
                IpPacket.getDestIp(s) == gatewayIp &&//
                    IpPacket.TCPPacket.getDestPort(s)== clientPort
                
        ){
            forwardtoClient(new Socket(s,outStream) );
            return true;
        }
        return false;
    }

    private void forwardToServer(Socket s) throws IOException {
        SocketEditable se = new SocketEditable(s.buffer);
        System.out.println("\n\n\n");
        System.out.println("forwarding to server");
        System.out.println(s.toString()+"\n\n\n");
        TcpEditable tcp = new TcpEditable(
                Arrays.copyOfRange(s.buffer, Socket.payloadStartIndex, s.buffer.length),
                s.sourceIpAddress, 
                s.destinationIpAddress
        );
        
        
        //check for errors
        byte[] fin = se.getPacket(tcp.getPacket(s.sourceIpAddress, s.destinationIpAddress));
        if(fin.length!= s.buffer.length){
            System.out.println("fin:"+Arrays.toString(fin));
            System.out.println("s:"+Arrays.toString(s.buffer));
            throw new IOException("the 2 buffers do not coincide:length");
        }
        for(int i = 0;i<s.buffer.length; i++){
            if(s.buffer[i] != fin[i]){
            System.out.println("fin:"+Arrays.toString(fin));
            System.out.println("s:"+Arrays.toString(s.buffer));
            throw new IOException("the 2 buffers do not coincide:index="+i);
            }
        }
        
        
        se.setDestIp(forwardingIp);
        se.setSourceIp(gatewayIp);
        outStream.write(
                se.getPacket(tcp.getPacket(gatewayIp, forwardingIp)), 
                tcp.getDestPort(), 
                se.getDestIp()
        );
    }
 
    private void forwardtoClient(Socket s) throws IOException {
        System.out.println("\n\n\n");
        System.out.println("forward to client");
        System.out.println(s.toString()+"\n\n\n");
        
        SocketEditable se = new SocketEditable(s.buffer);
        TcpEditable tcp = new TcpEditable(Arrays.copyOfRange(s.buffer, Socket.payloadStartIndex, s.buffer.length), s.sourceIpAddress, s.destinationIpAddress);
        
        
        //check for errors
        byte[] fin = se.getPacket(tcp.getPacket(s.sourceIpAddress, s.destinationIpAddress));
        if(fin.length!= s.buffer.length){
            System.out.println("fin:"+Arrays.toString(fin));
            System.out.println("s:"+Arrays.toString(s.buffer));
            throw new IOException("the 2 buffers do not coincide:length");
        }
        for(int i = 0;i<s.buffer.length; i++){
            if(s.buffer[i] != fin[i]){
            System.out.println("fin:"+Arrays.toString(fin));
            System.out.println("s:"+Arrays.toString(s.buffer));
            throw new IOException("the 2 buffers do not coincide:index="+i);
            }
        }
        
        
        se.setSourceIp(serverIp);
        se.setDestIp(clientIp);
        outStream.write(
                se.getPacket(tcp.getPacket(serverIp,clientIp)), 
                s.getTCP().sourcePort, 
                s.sourceIpAddress
        );
    }
    
}
