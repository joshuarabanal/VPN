/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package sockets.editable;

import java.io.IOException;
import java.util.Arrays;
import sockets.Socket;
import sockets.TcpPacket;
import sockets.tcp.Options;

/**
 *
 * @author root
 */
public class TcpEditable {
    private byte[] b;
    private Options options;
    public byte[] payload;
    public TcpEditable(byte[] buffer, int sourceIp, int destIp) throws IOException{
        this.b = buffer;
            options = new Options(b, 20, getOptionsLength());
        payload =Arrays.copyOfRange(b, TcpPacket.optionsStartIndex+getOptionsLength(), b.length);
        b = Arrays.copyOfRange(b, 0, TcpPacket.optionsStartIndex);
        
        int oldChecksum = getShort(16);
        int checksum = getChecksum(sourceIp, destIp);
        if(checksum!= 0){
            throw new IOException("checksu invalid:"+checksum+"!="+oldChecksum+":"+0xFFFF);
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
        setShort(val&0xff, index+2);
    }
    
    
    public int getSourcePort(){ return getShort(0); }
    public void setSourcePort(int val){ setShort(val,0); }
    
    public int getDestPort(){ return getShort(2); }
    public void setDestPort(int val){ setShort(val,2); }
    
    public int getSequenceNumber(){ return getInt(4); }
    public void setSequenceNumber(int val){ setInt(val,4); }
    
    public int getAckNumber(){ return getInt(8); }
    public void setAckNumber(int val){ setInt(val,8); }
    
    public int getOptionsLength() { 
        return (((b[12]&0xff)>>4)-5)*4; }
    
    public void setOptionsLength() { 
       int length = options.toByteArray().length;
       length/=4;
       length += 5;
       length = length<<4;
       b[12] = (byte)length;
    }
    
    public int getFlags(){ return b[13]; }
    public void setFlags(int val){ b[13] = (byte) val; }
    
    private void setFlag(boolean on, int flag){
        int flags = getFlags() & (~flag);
        if(on){
            flags |= flag;
        }
        setFlags(flags);
    }
    public boolean getACK(){   return (getFlags() & TcpPacket.ACK_flag) != 0;  }
    public void setACK(boolean ACK){ setFlag(ACK,TcpPacket.ACK_flag); }
    
    public boolean getPSH(){   return (getFlags() & TcpPacket.PSH_flag) != 0;  }
    public void setPSH(boolean ACK){ setFlag(ACK,TcpPacket.PSH_flag); }
    
    public boolean getRST(){   return (getFlags() & TcpPacket.RST_flag) != 0;  }
    public void setRST(boolean ACK){ setFlag(ACK,TcpPacket.RST_flag); }
    
    public boolean getSYN(){   return (getFlags() & TcpPacket.SYN_flag) != 0;  }
    public void setSYN(boolean ACK){ setFlag(ACK,TcpPacket.SYN_flag); }
    
    public boolean getFIN(){   return (getFlags() & TcpPacket.FIN_flag) != 0;  }
    public void setFIN(boolean ACK){ setFlag(ACK,TcpPacket.FIN_flag); }
    
    public boolean getURG(){   return (getFlags() & TcpPacket.URG_flag) != 0;  }
    public void setURG(boolean ACK){ setFlag(ACK,TcpPacket.URG_flag); }
    
    
    public int getWindowSize(){ return getShort(14); }
    public void setWindowSize(int val){ setShort(val,14); }
    
    //checksum = 16(short)
    private int getChecksum(int sourceIp,int destIp) throws IOException{
        long sum = 0;
        //source ip
        sum+= (sourceIp>>16 )& 0xFFFF;
        sum+= sourceIp& 0xffff;
              
        //dest ip
        sum+= (destIp>>16)& 0xFFFF;
        sum+= destIp & 0xffff;
        
        // 8 bit 0
        
        //protocol
        sum+=  (Socket.TCP_protocol);
        
        //buffer checksums
        sum+=(~Socket.checksum(b, 0, b.length))&0xffff;
            byte[] options = getOptions().toByteArray();
        sum+=(~Socket.checksum(options, 0, options.length)) & 0xffff;
        sum+=(~Socket.checksum(payload, 0, payload.length)) & 0xffff;
        
        //tcp length
        sum+= b.length+options.length+payload.length;
        
        if(sum<0){ throw new IOException("failed to add checksum"); }
        while(sum>0xFFFF){
            sum = (sum&0xffff) + (sum>>16);
        }
        long checksum = (~sum)&0xffff;
        return (int) checksum;
        
    }
    public int setChecksum(int sourceIp, int destIp) throws IOException{
        b[16] = b[17] = 0;
        int sum = getChecksum(sourceIp, destIp);
        setShort(sum,16);
        return sum;
    }
    
    public int getUrgentPointer(){ return getShort(18); }
    public void set(int val){ setShort(val,18); }
    
    public Options getOptions() throws IOException{ 
        setOptionsLength(); 
        return options;
    }
    public void setOptions( Options val) throws IOException{
        if(val == null){ throw new NullPointerException(); }
        setOptionsLength(); 
        this.options = options; 
    }
    
    //public int get(){ return getShort(); }
    //public void set(int val){ setShort(val,); }
    public byte[] getPacket(int sourceIp, int destIp) throws IOException{
        setChecksum(sourceIp, destIp);
        byte[] options = getOptions().toByteArray();
        
        byte[] retu = new byte[ b.length + options.length + payload.length ];
        for(int i = 0; i<b.length; i++){ retu[i] = b[i]; }
        
        for(int i = 0; i<options.length; i++){ retu[i+b.length] = options[i]; }
        
        for(int i = 0; i<payload.length; i++){ retu[i+b.length+options.length] = payload[i]; }
        return retu;
    }
    
}
