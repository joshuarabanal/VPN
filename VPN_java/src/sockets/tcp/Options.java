/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package sockets.tcp;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import sockets.tcp.Options.Option;

/**
 *
 * @author root
 */
public class Options extends ArrayList<Option> {
    
    public static class Option{
        static final int 
                type_padding_flag = 1,
                type_max_seg_size= 2,//"maximum segment size",
                type_wind_scale = 3,//"Window scale",
                type_selec_ack_permit = 4,//"Selective Acknowledgement permitted",
                type_time = 8;//"timestamp";
        int type;
        byte[] data;
        public Option(int type, byte[] b , int start, int length){
            this.type = type;
            this.data = Arrays.copyOfRange(b, start, start+length);
        }
        public String toString(){
            String retu = type+"=";
            switch(type){
                case type_max_seg_size:
                    retu="="+( (data[0]<<8) + (data[1]&0xff) );
                    break;
                case type_wind_scale:
                    retu = "="+ data[0];
                    break;
                case type_selec_ack_permit:
                    retu ="="+"true";
                    break;
                case type_time:
                    break;
                        
            }
            return retu;
        }
        public byte[] toArray(){
            if(type == type_padding_flag){
                byte[]retu=new byte[1];
                retu[0] = (byte) type;
                return retu;
            }
            byte[] retu = new byte[2+data.length];
            retu[0] = (byte) type;
            retu[1] = (byte) retu.length;
            for(int i = 0; i<data.length; i++){
                retu[2+i] = data[i];
            }
            return retu;
        }
    }
    
    public static String arrayToString(byte[] b, int start, int length){
        StringBuilder sb = new StringBuilder();
        for(int i = 0; i<length; i++){
            if(i>0){ sb.append(", "); }
            sb.append(b[i+start]);
        }
        return "["+sb.toString()+"]";
        
    }
    public Options(byte[] b, int start, int length) throws IOException{
        int offset = 0 ;
        while(offset < length){
            switch(b[start+offset]){
                
                case 0://end flag
                    if(offset +1 != length){
                        throw new IOException("malformed options list:"+(offset-length) );
                    }
                    offset++;
                    break;
                    
                case 1://padding flag
                    this.add(new Option(Option.type_padding_flag,b, 0,0));
                    offset++;
                    break;
                    
                case 2://maximum segment size
                    if(b[start+1+offset] != 4){ 
                        throw new IOException("malformed options list" );
                    }
                    offset+=2;
                    this.add(new Option(Option.type_max_seg_size, b,start+offset, 2));
                    offset+=2;
                    break;
                    
                case 3://window scale
                    if(b[start+1+offset] != 3){ 
                        throw new IOException("malformed options list:"+arrayToString(b, start,length) );
                    }
                    offset+=2;
                    this.add(new Option(Option.type_wind_scale, b,start+offset, 1));
                    offset++;
                    break;
                    
                case 4: // Selective Acknowledgement permitted
                    if(b[start+1+offset] != 2){ 
                        throw new IOException("malformed options list" );
                    }
                    offset+=2;
                    this.add(new Option(Option.type_selec_ack_permit, b,start+offset, 0));
                    break;
                    
                case 5: //Selective ACKnowledgement
                    offset+=2;
                    this.add( new Option(5, b,start+offset, b[start+offset-1]) );
                    offset+=  b[start+offset-1]-2;
                    break;
                    
                case 8: //Timestamp
                    if(b[start+1+offset] != 10){ 
                        throw new IOException("malformed options list" );
                    }
                    offset+=2;
                    this.add( new Option(Option.type_time, b,start+offset,8) );
                    offset+=  8;
                    break;
                    
                default: throw new IOException("unknown option:"+b[start+offset]);
                    
                    
            }
        }
    }
    
    public byte[] toByteArray(){
        ArrayList<Byte> fil = new ArrayList<Byte>();
        for(Option o : this){
            byte[] buf = o.toArray();
            for(byte byt : buf){
                fil.add(byt);
            }
        }
        byte[] retu = new byte[fil.size()];
        for(int i =0; i<retu.length; i++){
            retu[i] = fil.get(i);
        }
        return retu;
    }
    
    /**
     * http://www.networksorcery.com/enp/protocol/tcp/option008.htm
     * @param timestampEcho 
     */
    
    public void addTimeStamp(int timestampEcho){
        byte[] b = new byte[8];
        int ourTime = (int) System.currentTimeMillis();
        b[0] = (byte) ((ourTime>>24)&0xff);
            b[1] = (byte) ((ourTime>>16)&0xff);
            b[2] = (byte) ((ourTime>>8)&0xff);
            b[3] = (byte) ((ourTime)&0xff);
        b[4] = (byte) ((ourTime>>24)&0xff);
            b[5] = (byte) ((ourTime>>16)&0xff);
            b[6] = (byte) ((ourTime>>8)&0xff);
            b[7] = (byte) ((ourTime)&0xff);
        add(new Option(Option.type_time,b, 0, b.length));
    }
}
