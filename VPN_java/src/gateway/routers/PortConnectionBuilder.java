/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package gateway.routers;

import java.util.ArrayList;
import sockets.IpPacket;

/**
 *
 * @author root
 */
public interface PortConnectionBuilder{
    public ConnectionForwarder bind( Client.UDP conn);
    public ConnectionForwarder bind(  Client.TCP conn);
    public interface ConnectionForwarder{
        public void newClientMessage(byte[] b);
    } 
}