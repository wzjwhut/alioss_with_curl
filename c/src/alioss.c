#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>

#include "alioss.h"
#include "curl/curl.h"
#include "openssl/hmac.h"
#include "base64.h"
#include "outstream.h"

#ifndef ASSERT
#define ASSERT(x) assert(x)
#endif

#ifndef LOGE
#define LOGE printf
#endif

#ifndef LOGI
#define LOGI printf
#endif

#ifndef PERROR
#define PERROR printf
#endif

/** defined by curl */
extern const char * const Curl_wkday[7];
extern const char * const Curl_month[12];

#ifdef WIN32
#define ftello _ftelli64
#define fseeko _fseeki64
#endif

static inline uint64_t utc_timestamp(){
#ifdef _WIN32
    ULARGE_INTEGER large;
    GetSystemTimeAsFileTime((LPFILETIME)&large);
    const uint64_t UNIX_TIME_START = 0x019DB1DED53E8000;
    return (large.QuadPart - UNIX_TIME_START)/10000;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)((uint64_t)tv.tv_sec * 1000) + (uint64_t)tv.tv_usec/1000;
#endif // _WIN32
}


CURLcode Curl_gmtime(time_t intime, struct tm *store);

static size_t on_http_content(void *buffer, size_t unused, size_t size, void *userp){
    ASSERT(unused == 1);
    outstream_t* content = (outstream_t*)userp;
    int last_char =outstream_lastchar(content);
    if(last_char == '\0'){
        content->data_len--;
    }
    outstream_writebuf(content, (const char*)buffer, (int)size);
    outstream_writechar(content, '\0');
    return size;
}

static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream){
    printf("size: %u, nmemb: %u\n", (int)size, (int)nmemb);
    size_t retcode = fread(ptr, size, nmemb, stream);
    return retcode;
}

static int gmt_date_str(char datestr[64]){
    const struct tm *tm;
    struct tm keeptime;
    CURLcode result;
    result = Curl_gmtime(utc_timestamp()/1000, &keeptime);
    if(result) {
        printf("gmtime failed: %d\n", result);
        return 0;
    }
    tm = &keeptime;
    /* format: "Tue, 15 Nov 1994 12:45:26 GMT" */
    return snprintf(datestr, 64,
                    "%s, %02d %s %4d %02d:%02d:%02d GMT",
                    Curl_wkday[tm->tm_wday?tm->tm_wday-1:6],
            tm->tm_mday,
            Curl_month[tm->tm_mon],
            tm->tm_year + 1900,
            tm->tm_hour,
            tm->tm_min,
            tm->tm_sec);
}

#define STR_x_oss_security_token "x-oss-security-token"

#define TEXT_PLAIN "text/plain"
#define OCTET_STREAM "application/octet-stream"
#define HEADER_AUTH_PREFIX "Authorization:OSS "

int ali_oss_upload(const char* endpoint,
                      const char* key_id,
                      const char* key_secret,
                      const char* security_token,
                      const char* bucket,
                      const char* object_name,
                      const char* file_path){
    ASSERT(endpoint != NULL);
    ASSERT(key_id != NULL);
    ASSERT(key_secret != NULL);
    ASSERT(security_token != NULL);
    ASSERT(bucket != NULL);
    ASSERT(object_name != NULL);
    ASSERT(file_path != NULL);

    FILE* file = fopen(file_path, "rb");
    int file_length;
    if(file != NULL){
        int64_t cur = ftello(file);
        fseeko(file, 0, SEEK_END);
        file_length = ftello(file);
        fseeko(file, cur, SEEK_SET);
        if(file_length == 0){
            LOGE("file %s is empty\n", file_path);
            fclose(file);
            return FALSE;
        }else{
            LOGI("file size: %d\n", file_length);
        }
    }else{
        PERROR("file not exits: %s\n", file_path);
        return FALSE;
    }


    BOOL result = FALSE;

    const int key_id_len = strlen(key_id);
    const int key_secret_len = strlen(key_secret);
    const int security_token_len = strlen(security_token);
    const int bucket_len = strlen(bucket);
    const int object_name_len = strlen(object_name);
    const char* content_type = OCTET_STREAM;
    const char* header_content_type = "Content-Type:"OCTET_STREAM;
    const int content_type_len = strlen(content_type);

    int url_len = 7 + bucket_len + 1 + strlen(endpoint) + 1 + object_name_len + 1;
    char* url = (char*)MALLOC(url_len);
    url_len = snprintf(url, url_len, "http://%s.%s/%s", bucket, endpoint, object_name);
    LOGI("url: %s", url);

    char datestr[64];
    char header_date[64+5];
    const int datestr_len = gmt_date_str(datestr);

    const int header_date_len = snprintf(header_date, 64+5, "Date:%s", datestr);

    int header_token_len = sizeof(STR_x_oss_security_token) + security_token_len + 1;
    char* header_token = (char*)MALLOC(header_token_len);
    header_token_len = snprintf(header_token, header_token_len,
                                "%s:%s", STR_x_oss_security_token, security_token);

    int resource_path_len = 1 + bucket_len + 1 + object_name_len + 1;
    char* resource_path = (char*)MALLOC(resource_path_len);
    resource_path_len = snprintf(resource_path,resource_path_len,
                                 "/%s/%s",  bucket, object_name);
    LOGI("resource path: %s\n", resource_path);
    LOGI("content_type_len: %d\n", content_type_len);
    LOGI("datestr_len: %d\n", datestr_len);
    LOGI("header_token_len: %d\n", header_token_len);
    LOGI("resource_path_len: %d\n", resource_path_len);
    int canonical_len = 3 + 1 + 1 + content_type_len + 1 + datestr_len + 1 + header_token_len + 1 +
            resource_path_len + 1;
    char* canonical = (char*)MALLOC(canonical_len);

    canonical_len = snprintf(canonical, canonical_len,
                             /** METHOD Content-MD5(empty) Content-Type Date OSS-TOKEN RESOURCE-PATH */
                             "PUT\n%s\n%s\n%s\n%s\n%s",
                             /*content md5 */"",
                             content_type, datestr, header_token, resource_path);
    LOGI("cannonical str: %s", canonical);

    uint8_t hmac_sha1[EVP_MAX_MD_SIZE];
    uint32_t hmac_sha1_len = EVP_MAX_MD_SIZE;
    HMAC(EVP_sha1(), key_secret, key_secret_len,
         (const uint8_t*)canonical, canonical_len, hmac_sha1, &hmac_sha1_len);
    char signature[EVP_MAX_MD_SIZE*2+1];
    int signature_len = EVP_MAX_MD_SIZE*2;
    Base64Encode(hmac_sha1, hmac_sha1_len, signature, &signature_len);
    signature[signature_len] = '\0';
    LOGI("signature: %s", signature);

    int header_authorization_len = sizeof(HEADER_AUTH_PREFIX) + key_id_len + 1 + signature_len +1;
    char* header_authorization = (char*)MALLOC(header_authorization_len);
    header_authorization_len = snprintf(header_authorization, header_authorization_len,
                                        HEADER_AUTH_PREFIX"%s:%s", key_id, signature);
    //char header_content_length[32];
    //snprintf(header_content_length, 32, "Content-Length: %d", file_length);

    LOGI("oss header %s", header_authorization);
    LOGI("oss header %s", header_content_type);
    LOGI("oss header %s", header_date);
    LOGI("oss header %s", header_token);
    //LOGI("oss header %s", header_content_length);

    CURL* curl = curl_easy_init();

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(NULL,header_authorization);
    headers = curl_slist_append(headers,header_content_type);
    headers = curl_slist_append(headers,header_date);
    headers = curl_slist_append(headers,header_token);
    //headers = curl_slist_append(headers,header_content_length);

    outstream_t data;
    outstream_init(&data);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, on_http_content);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(curl, CURLOPT_READDATA, file);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 1000*60);
    curl_easy_setopt(curl, CURLOPT_TCP_NODELAY,1);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    int ret = curl_easy_perform(curl);
    if(ret != CURLE_OK){
        LOGE("curl error: %d", ret);
    }else{
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if(http_code != 200){
            LOGE("http error, status code: %ld, resp: %s", http_code, (char*)data.data);
        }else{
            LOGI("sucess, resp: %s", (char*)data.data);
            result = TRUE;
        }
    }

    curl_slist_free_all(headers); /* free custom header list */
    curl_easy_cleanup(curl);
    FREEIF(url);
    FREEIF(header_token);
    FREEIF(resource_path);
    FREEIF(canonical);
    FREEIF(header_authorization);
    FREEIF(data.data);
    fclose(file);
    return result;
}

#ifdef __cplusplus
}
#endif
