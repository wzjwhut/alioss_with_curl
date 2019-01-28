#include <QCoreApplication>
#include <stdio.h>
#include <stdint.h>

#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#include "openssl/crypto.h"
#include "openssl/bn.h"
#include "alioss.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    printf("main\n");

    const char* endpoint = "oss-cn-beijing.aliyuncs.com";
    const char* key_id = "STS.NHu8DpRykTnkQyiVjiAH8VT1u";
    const char* key_secret = "GS9iVntu7MN35mFaxsjpumwHf8pw9PZYNS8RLRGWU485";
    const char* security_token = "CAISggJ1q6Ft5B2yfSjIr4vAc/7Ev6ZK46yAU1/Ysmo8TccUuZGalzz2IHBIfnlsCOgdtvk0mGhY5/8SlqVoRoReREvCKM1565kPO6xPqGuE6aKP9rUhpMCPKwr6UmzGvqL7Z+H+U6mqGJOEYEzFkSle2KbzcS7YMXWuLZyOj+wIDLkQRRLqL0AFZrFsKxBltdUROFbIKP+pKWSKuGfLC1dysQcO7gEa4K+kkMqH8Uic3h+oiM1t/tqoecf/NpI2YMcuD4zlg9YbLPSRjHRijDFR77pzgaB+/jPKg8qQGVE54W/dY7aOooE+dFQhO/NmQ/4V9ajm+uJquPDejJjnEMethmjI1U4agAFp8dk7VIjCETliMp2x1aRdJ/Mx8YfSxoAITvfsnzFP9ti4FYg5kooS1qCX5fmwYVUwjPXjvyKaEAyAG2MMBYRz/e2Cbb0HuvTmD0vSRxeLv4cCG+gVNDr7A6LU4d8yi6uMYJnmcjrtRS/31R8vPqyhUNLsZ4s/OimJhcVfnnCPlw==";
    const char* bucket = "wzj-test";
    const char* object_name = "wzj2.txt";
    const char* file_path = "/1.txt";
    ali_oss_upload(endpoint,
                   key_id,
                   key_secret,
                   security_token,
                   bucket,
                   object_name,
                   file_path);

    return a.exec();
}












