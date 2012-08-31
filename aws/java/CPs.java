import java.lang.System;
import java.util.Arrays;
import java.io.File;
import java.io.IOException;
import java.util.Date;

public class CPs {
    public static void main(String[] args) throws IOException {
        
        String classpath = java.lang.System.getProperty( "java.class.path" );
        for (String path : classpath.split(System.getProperty("path.separator"))){
            File f = new File (path);
            String resource = (f.isDirectory() ? Arrays.asList( f.list()).toString():f.toString());
            System.out.println (resource);
        }
    }
}
