/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package sockets.editable;

import java.io.IOException;
import java.util.Arrays;
import sockets.Socket;

/**
 *
 * @author root
 */
public class SocketEditable {
    private byte[] b;
    //public byte[] payload;
    
    public SocketEditable(byte[] b) throws IOException{
        this.b = Arrays.copyOfRange(
                b, 0, Socket.payloadStartIndex
        );
        //this.payload = Arrays.copyOfRange(
        //        b, Socket.payloadStartIndex, b.length
        //);
        if(
                b.length != getShort(2) //packet length
                && 
                b[0] != ((4<<4) + 5)//4 = version & 5 = ihl
                &&
                b[9] != Socket.TCP_protocol //tcp protocol
                & 
                getShort(10) != Socket.checksum(b, 0, Socket.payloadStartIndex)
        ){
            throw new IOException("failed to parse packet correctly");
        }
        
    }
    
    private int getShort(int index){
        return  ((b[index]&0xff)<<8) | ((b[index+1]&0xff)); 
    }
    private  void setShort(int value,int index){
        b[index] = (byte) ((value>>8) & 0xff);
        b[index+1] = (byte) (value&0xff);
    }
    
    private int getInt(int index){
        return (getShort(index)<<16) + getShort(index+2);
    }
    private void setInt(int val, int index){
        setShort(val>>16, index);
        setShort(val&0xFFFF, index+2);
    }
    
    
    
    public int getTypeOfService() { return b[1]; }
    public void setTypeOfService(int type){ b[1] = (byte)( type & 0xff ); } 
    
    public int setLength(byte[] payload){ 
        setShort(b.length+payload.length, 2);
        return b.length+payload.length;
    }
    
    public int getId(){ return getShort(4); }
    public void setId(int id){  setShort(id, 4); }
    
    public int getFragOffset(){ return getShort(6); }
    public void setFragOffset(int offset){ setShort(offset, 6); }
    
    public int getTTL(){ return b[8];}
    public void setTTL(int ttl){ b[8] = (byte) ttl;}
    
    public int setCheckSum(){
        b[10] = b[11] = 0;
        int check = Socket.checksum(b, 0, Socket.payloadStartIndex);
        setShort(check, 10);
        return check;
    }
    
    public int getSourceIp(){ return getInt(12); }
    public void setSourceIp(int ip){ setInt(ip, 12); }
    
    public int getDestIp(){ return getInt(16); }
    public void setDestIp(int ip){ setInt(ip, 16); }
    
    
    public byte[] getPacket(byte[] payload){
        setLength(payload);
        setCheckSum();
        byte[] retu = new byte[b.length+payload.length];
        
        for(int i =0; i<b.length; i++){ retu[i] = b[i]; }
        
        for(int i = 0; i<payload.length; i++){ retu[i+b.length] = payload[i]; }
        
        return retu;
    }
}
