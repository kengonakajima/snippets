import java.applet.Applet;
import java.net.*;
import java.awt.*;
import java.io.*;
import java.util.*;

public class aho extends Applet 
{
        URL url;
        InputStream in;
        
        public void init()
        {

                while(true )
                {
                        long l;
                        
        
                        try{
                                Thread.sleep( 200);
                        }catch( InterruptedException e ){}

                        try{
                                url = new URL( "http://murasaki/cgi-bin/ringo/test/a.out");
                        }catch( MalformedURLException e ){}
                        try{
                                in = url.openStream();
                        }catch( IOException e ){}

                        l = System.currentTimeMillis();

                        try{

                                System.out.println( in.available() );
                                int count=0;
                                int readb = 0;
                                int r;
                                {
                                        
                                        byte[] b = new byte[256];
                                        if( (r = in.read( b , 0 , 256))  == -1 ) break;
                                        readb += r;
                                        count++;
                                        System.out.println( new String( b ,0));
                                        
                                }
                                System.err.println("----------"+ count + "times read. total:"+ readb + "bytes");
                                System.err.println("time:" + ( System.currentTimeMillis() -l) );
                        }catch( IOException e ){}
                }


        }


}
