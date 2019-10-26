/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package dhcp.dhcpPacket;

import java.util.ArrayList;

/**
 *
 * @author root
 */
public class Options extends ArrayList<Option> {
    
    
    
    public int getMessageType(){
        for(Option o : this){
            if(o.type == Option.type_message_type){
                return ((Option.MessageType)o).getMessageType();
            }
        }
        return -1;
    }
}
