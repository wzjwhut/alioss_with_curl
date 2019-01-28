package com.wzjwhut.example;

import java.io.ByteArrayInputStream;

import com.aliyuncs.DefaultAcsClient;
import com.aliyuncs.auth.sts.AssumeRoleRequest;
import com.aliyuncs.auth.sts.AssumeRoleResponse;
import com.aliyuncs.http.MethodType;
import com.aliyuncs.profile.DefaultProfile;
import com.aliyuncs.profile.IClientProfile;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import com.aliyun.oss.OSSClient;

public class Main {
	private final static Logger logger = LogManager.getLogger(Main.class);

	/**  */
    private final static String accessKeyId = "your id";
    private final static String accessKeySecret = "your secret";

    public static class Cred{
        public String keyId;
        public String keySecret;
        public String token;
        public String endpoint;
        public Cred(){
        }
    }

	public static void main(String[] args) throws Throwable {
        Cred cred = getToken();
	}

	public static Cred getToken() throws Throwable{
        String endpoint = "sts.aliyuncs.com";
        String roleArn = "acs:ram::your:role/your";
        String roleSessionName = "session-name";
        String policy = "{\n" +
                "    \"Version\": \"1\", \n" +
                "    \"Statement\": [\n" +
                "        {\n" +
                "            \"Action\": [\n" +
                "                \"oss:*\"\n" +
                "            ], \n" +
                "            \"Resource\": [\n" +
                "                \"acs:oss:*:*:*\" \n" +
                "            ], \n" +
                "            \"Effect\": \"Allow\"\n" +
                "        }\n" +
                "    ]\n" +
                "}";
            // 添加endpoint（直接使用STS endpoint，前两个参数留空，无需添加region ID）
            DefaultProfile.addEndpoint("", "", "Sts", endpoint);
            // 构造default profile（参数留空，无需添加region ID）
            IClientProfile profile = DefaultProfile.getProfile("", accessKeyId, accessKeySecret);
            // 用profile构造client
            DefaultAcsClient client = new DefaultAcsClient(profile);
            final AssumeRoleRequest request = new AssumeRoleRequest();
            request.setMethod(MethodType.POST);
            request.setRoleArn(roleArn);
            request.setRoleSessionName(roleSessionName);
            request.setPolicy(policy); // Optional
            final AssumeRoleResponse response = client.getAcsResponse(request);
            System.out.println("Expiration: " + response.getCredentials().getExpiration());
            System.out.println("Access Key Id: " + response.getCredentials().getAccessKeyId());
            System.out.println("Access Key Secret: " + response.getCredentials().getAccessKeySecret());
            System.out.println("Security Token: " + response.getCredentials().getSecurityToken());
            System.out.println("RequestId: " + response.getRequestId());
            Cred  cred = new Cred();
            cred.keyId = response.getCredentials().getAccessKeyId();
            cred.keySecret = response.getCredentials().getAccessKeySecret();
            cred.token = response.getCredentials().getSecurityToken();
            cred.endpoint = endpoint;
            return cred;
    }

	public static void stsUpload(Cred cred) throws Exception {
		String endpoint = "http://oss-cn-beijing.aliyuncs.com";
		OSSClient ossClient = new OSSClient(endpoint, cred.keyId, cred.keySecret, cred.token);
		String content = "Hello OSS";
		ossClient.putObject("wzj-test", "wzj.txt", new ByteArrayInputStream(content.getBytes()));
		ossClient.shutdown();
	}
	
}
