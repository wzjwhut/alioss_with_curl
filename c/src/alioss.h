#ifndef ALI_OSS_H
#define ALI_OSS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


int ali_oss_upload(const char* endpoint,
                    const char* key_id,
                    const char* key_secret,
                    const char* security_token,
                    const char* bucket,
                    const char* object_name,
                    const char* file_path);

#ifdef __cplusplus
}
#endif


#endif //ALI_OSS_H


