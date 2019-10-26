/*
 * <a href="https://tools.ietf.org/html/rfc1533"> source </a>
 */
package dhcp.dhcpPacket;

import java.util.Arrays;
import sockets.Socket;
import sockets.editable.TcpEditable;

/**
 *
 * @author root
 */
public class Option {
    public static final int type_subnet_mask = 1,
            type_router =3,
            type_dns = 6,
            type_host_name = 12,
            type_static_route = 33,
            type_message_type = 53,
            type_client_identifier = 61,
            type_param_request_list = 55,
            type_max_message_size = 57,
            type_class_identifier = 60,
            type_end = 255;
    public int type;
    public byte[] data;
    
    public static Option initialize(byte[] b, int start){
        switch(b[start]&0xff){
            case type_subnet_mask:  return new SubnetMask(b,start);
            case type_router:  return new RouterOption(b,start);
            case type_dns:  return new DNSOption(b,start);
            case type_host_name:  return new HostName(b,start);
            case type_static_route:  return new StaticRouteOption(b,start);
            case type_message_type:  return new MessageType(b,start);
            case type_client_identifier:  return new ClientIdentifier(b,start);
            case type_param_request_list:  return new ParamRequestList(b,start);
            case type_max_message_size:  return new MaxMessageSize(b,start);
            case type_class_identifier:  return new ClassIdentifier(b,start);
            case type_end:  return new EndOption(b,start);
        }
        return new Option(b,start);
    }
    private  Option(byte[]b, int start){
            type = b[start]&0xff;
        if(type == 0 || type == 0xff){
            data = new byte[0];
            return;
        }
        else{
            if(b[start+1]<0){
                System.out.println("array[start=>length]:"+Arrays.toString(Arrays.copyOfRange(b, start, b.length)));
                throw new IndexOutOfBoundsException("at index:"+start);
            }
            data = Arrays.copyOfRange(b, start+2, b[start+1]+start+2);
        }
    }
    public int getByteArrayLength(){
        if(data.length == 0){
            return 1;
        }
        return 2+data.length;
    }
    public String toString(){
        return typeToString(type)+":"+Arrays.toString(data);
    }
    public static String typeToString(int type){
        switch(type){
            case type_message_type:  return  "MessageType";
            case type_client_identifier:  return "ClientIdentifier";
            case type_param_request_list:  return "ParamRequestList";
            case type_max_message_size:  return "MaxMessageSize";
            case type_class_identifier:  return "ClassIdentifier";
            case type_end:  return "EndOption";
        }
        return "unknown("+type+")";
    }
    
    //----------------------------------------------------------------------------------------------------
    //extended classes
    public static class MessageType extends Option{
         public static final int DHCPDISCOVER = 1,DHCPOFFER = 2, DHCPREQUEST = 3, 
                 DHCPDECLINE = 4, DHCPACK = 5, DHCPNAK = 6, DHCPRELEASE = 7;
        public MessageType(byte[] b, int start) {
            super(b, start);
            if(type!= type_message_type){
                throw new IndexOutOfBoundsException("incorrect Type:"+type);
            }
        }
        public String toString(){
            switch(data[0]){
                case DHCPDISCOVER: return "MessageType:Discover";
                case DHCPOFFER: return "MessageType:Offer";
                case DHCPREQUEST: return "MessageType:Request";
                case DHCPDECLINE: return "MessageType:Decline";
                case DHCPACK: return "MessageType:Ack";
                case DHCPNAK: return "MessageType:Nak";
                case DHCPRELEASE: return "MessageType:Release";
            }
            return "MessageType:unknown("+data[0]+")";
        }
    }
    
    public static class ClientIdentifier extends Option{
        
        public ClientIdentifier(byte[] b, int start) {
            super(b, start);
            
            if(type!= type_client_identifier){
                throw new IndexOutOfBoundsException("incorrect Type:"+type);
            }
        }
        public String toString(){
            return "CliendIdentifier:"+Arrays.toString(data);
        }
        
    }
    
    public static class MaxMessageSize extends Option{
        
        public MaxMessageSize(byte[] b, int start) {
            super(b, start);
            
            if(type!= type_max_message_size){
                throw new IndexOutOfBoundsException("incorrect Type:"+type);
            }
        }
        public String toString(){
            return "CliendIdentifier:"+TcpEditable.getShort(0,data);
        }
        
    }
    
    public static class ClassIdentifier extends Option{
        
        public ClassIdentifier(byte[] b, int start) {
            super(b, start);
            
            if(type!= type_class_identifier){
                throw new IndexOutOfBoundsException("incorrect Type:"+type);
            }
        }
        public String toString(){
            return "CliendIdentifier:"+new String(data,0,data.length);
        }
        
    }
    
    public static class EndOption extends Option{
        
        public EndOption(byte[] b, int start) {
            super(b, start);
            
            if(type!= type_end){
                throw new IndexOutOfBoundsException("incorrect Type:"+type);
            }
        }
        public String toString(){
            return "EndOption";
        }
        
    }
    public static class ParamRequestList extends Option{
        
        public ParamRequestList(byte[] b, int start) {
            super(b, start);
            
            if(type!= type_param_request_list){
                throw new IndexOutOfBoundsException("incorrect Type:"+type);
            }
        }
        public String toString(){
            StringBuilder sb = new StringBuilder("ParamRequestList:[");
            for(int i = 0; i<data.length; i++){
                if(i>0){sb.append(", "); }
                sb.append(typeToString(data[i]));
            }
            sb.append("]");
            return sb.toString();
        }
        
    }
    
    public static class SubnetMask extends Option{
        
        public SubnetMask(byte[] b, int start) {
            super(b, start);
            
            if(type!= type_subnet_mask){
                throw new IndexOutOfBoundsException("incorrect Type:"+type);
            }
        }
        public String toString(){
           return "SubnetMask:"+Socket.ipIntToString(TcpEditable.getInt(0, data));
        }
        
    }
    
    public static class StaticRouteOption extends Option{
        
        public StaticRouteOption(byte[] b, int start) {
            super(b, start);
            
            if(type!= type_static_route){
                throw new IndexOutOfBoundsException("incorrect Type:"+type);
            }
        }
        public String toString(){
            StringBuilder sb = new StringBuilder("static route option:");
            for(int i = 0; i<data.length; i+=8){
                sb.append("\n")
                        .append(Socket.ipIntToString(TcpEditable.getInt(i, data)))
                        .append("=>")
                        .append(Socket.ipIntToString(TcpEditable.getInt(i+4, data)));
            }
           return sb.toString();
        }
        
    }
    
    public static class RouterOption extends Option{
        
        public RouterOption(byte[] b, int start) {
            super(b, start);
            
            if(type!= type_router){
                throw new IndexOutOfBoundsException("incorrect Type:"+type);
            }
        }
        public String toString(){
            StringBuilder sb = new StringBuilder("router option:");
            for(int i = 0; i<data.length; i+=8){
                sb.append("\n")
                        .append(Socket.ipIntToString(TcpEditable.getInt(i, data)))
                        .append("=>")
                        .append(Socket.ipIntToString(TcpEditable.getInt(i+4, data)));
            }
           return sb.toString();
        }
        
    }
    
    public static class DNSOption extends Option{
        
        public DNSOption(byte[] b, int start) {
            super(b, start);
            
            if(type!= type_router){
                throw new IndexOutOfBoundsException("incorrect Type:"+type);
            }
        }
        public String toString(){
            StringBuilder sb = new StringBuilder("DNS option:");
            for(int i = 0; i<data.length; i+=4){
                sb.append("\n")
                        .append(Socket.ipIntToString(TcpEditable.getInt(i, data)));
            }
           return sb.toString();
        }
        
    }
    
    public static class HostName extends Option{
        
        public HostName(byte[] b, int start) {
            super(b, start);
            
            if(type!= type_host_name){
                throw new IndexOutOfBoundsException("incorrect Type:"+type);
            }
        }
        public String toString(){
            return "host name:"+new String(data,0,data.length);
        }
        
    }
    
    
    
}
