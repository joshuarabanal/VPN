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
        String type;
        byte[] data;
        public Option(String type, byte[] b , int start, int length){
            this.type = type;
            this.data = Arrays.copyOfRange(b, start, start+length);
        }
        public String toString(){
            return type;
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
                    offset++;
                    break;
                    
                case 2://maximum segment size
                    if(b[start+1+offset] != 4){ 
                        throw new IOException("malformed options list" );
                    }
                    offset+=2;
                    this.add(new Option("maximum segment size", b,start+offset, 2));
                    offset+=2;
                    break;
                    
                case 3://window scale
                    if(b[start+1+offset] != 3){ 
                        throw new IOException("malformed options list:"+arrayToString(b, start,length) );
                    }
                    offset+=2;
                    this.add(new Option("Window scale", b,start+offset, 1));
                    offset++;
                    break;
                    
                case 4: // Selective Acknowledgement permitted
                    if(b[start+1+offset] != 2){ 
                        throw new IOException("malformed options list" );
                    }
                    offset+=2;
                    this.add(new Option("Selective Acknowledgement permitted", b,start+offset, 0));
                    break;
                    
                case 5: //Selective ACKnowledgement
                    offset+=2;
                    this.add( new Option("Selective Acknowledgement", b,start+offset, b[start+offset-1]) );
                    offset+=  b[start+offset-1]-2;
                    break;
                    
                case 8: //Timestamp
                    if(b[start+1+offset] != 10){ 
                        throw new IOException("malformed options list" );
                    }
                    offset+=2;
                    this.add( new Option("timestamp", b,start+offset,10) );
                    offset+=  8;
                    break;
                    
                default: throw new IOException("unknown option:"+b[start+offset]);
                    
                    
            }
        }
    }
    
}
