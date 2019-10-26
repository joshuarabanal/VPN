/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package test;

import gateway.Server;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author joshuarabanal
 */
public class Test {
    
    public static void main_tcp(String[] args){
        
        
        System.setErr(System.out);
        System.out.println(System.getProperty("user.dir"));
        Server s = new Server();
        s.run();
        
       
    }
    
     public static void main(String[] args){
        
        
        System.setErr(System.out);
        gateway.UDPServer s = new gateway.UDPServer();
        s.run();
        
       
    }

    /**
     * @param args the command line arguments
     * @deprecated 
     */
    @Deprecated public static void mainz(String[] args) {
        // TODO code application logic here
        System.out.println("test");
        ServerSocket serve = null;
        try {
            serve = new ServerSocket();
            serve.bind(new InetSocketAddress("192.168.1.15",1080));
            System.out.println("ip:"+serve.getInetAddress()+", local ip:"+serve.getLocalSocketAddress());
           
            java.net.Socket s = serve.accept();
            System.out.println("new sock\n"+ 
                    "remote address:"+ s.getRemoteSocketAddress().toString()+
                    " inet address:"+s.getInetAddress()
            );
        } catch (IOException ex) {
            Logger.getLogger(Test.class.getName()).log(Level.SEVERE, null, ex);
            return;
        }
    }
    
}
