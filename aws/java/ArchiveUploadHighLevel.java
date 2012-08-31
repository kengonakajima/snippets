import java.io.File;
import java.io.IOException;
import java.util.Date;

import com.amazonaws.auth.AWSCredentials;
import com.amazonaws.auth.PropertiesCredentials;
import com.amazonaws.services.glacier.AmazonGlacierClient;
import com.amazonaws.services.glacier.transfer.ArchiveTransferManager;
import com.amazonaws.services.glacier.transfer.UploadResult;


public class ArchiveUploadHighLevel {
    public static String vaultName = "kengonakajima"; //"*** provide vault name ***";
    public static String archiveToUpload = "bigstar.wav"; //"*** provide name of file to upload ***";
    
    public static AmazonGlacierClient client;
    
    public static void main(String[] args) throws IOException {
        
        AWSCredentials credentials = new PropertiesCredentials(
                ArchiveUploadHighLevel.class.getResourceAsStream("AwsCredentials.properties"));
        client = new AmazonGlacierClient(credentials);
        //        client.setEndpoint("https://glacier.us-east-1.amazonaws.com/");
        client.setEndpoint("https://glacier.ap-northeast-1.amazonaws.com/");        

        try {
            ArchiveTransferManager atm = new ArchiveTransferManager(client, credentials);
            
            UploadResult result = atm.upload(vaultName, "my archive " + (new Date()), new File(archiveToUpload));
            System.out.println("Archive ID: " + result.getArchiveId());
            
        } catch (Exception e)
        {
            System.err.println(e);
        }
    }

}
