import java.io.*;

public class aho { 
    public static void main(String args[]) 
    { 
        try {
            FileInputStream fis = new FileInputStream(args[0]); 
            int cnt = 0;
            int n = Integer.parseInt(args[1]);
            int ch;
            byte buf[] = new byte[n];
            int len;
            while ((len = fis.read(buf)) >0) {
                for(int i=0;i<len;i++){
                    if (buf[i] == '\n') cnt++;
                }
            } 
            fis.close(); 
            System.out.println(cnt); 
        } 
        catch (IOException e) { 
            System.err.println(e); 
        } 
    } 
}
