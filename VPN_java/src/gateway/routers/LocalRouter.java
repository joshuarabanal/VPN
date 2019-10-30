/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package gateway.routers;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.util.ArrayList;
import sockets.IpPacket;
import sockets.RawSocket;

/**
 *
 * @author root
 */
public class LocalRouter implements PortConnectionBuilder {
    private RawSocket clientOut;

    @Override
    public ConnectionForwarder bind(Client.UDP conn) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public ConnectionForwarder bind(Client.TCP conn) {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }
    
    /**
     * udp timeout 30 seconds
     */
    private class LocalUDP implements ConnectionForwarder, Runnable{
        pr
        private DatagramSocket socket;
        private Client.UDP conn;
        private ArrayList<byte[]> packets = new ArrayList<byte[]>();
        private Thread t = new Thread(this);
        private boolean running = false;
        
        private LocalUDP(Client.UDP conn) throws SocketException{
            this.conn = conn;
            this.socket = new DatagramSocket();
        }

        @Override
        public void newClientMessage(byte[] b) {
            packets.add(IpPacket.UDPPacket.getPayload(b));
            if(!running){
                running = true;
                t.start();
            }
        }

        private byte[] read = new byte[65535];
        @Override
        public void run() {
            running = true;
            byte[] buf = packets.get(0);
            DatagramPacket packet;
            packet = new DatagramPacket(buf, buf.length, conn.serverIp, conn.serverPort);
            socket.send(packet);
            packet = new DatagramPacket(buf, buf.length);
            socket.receive(packet);
            
            running = false;
        }
        
    }
    
}
