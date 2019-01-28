#ifndef outstream_H
#define outstream_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifndef FREEIF
#define FREEIF(x) do{if(x!=NULL){free(x);x=NULL;}}while(0)
#endif

#ifndef FREE
#define FREE free
#endif

#ifndef MATH_MAX
#define MATH_MAX(a, b) ((a)>(b)?(a):(b))
#endif

#ifndef MALLOC
#define MALLOC malloc
#endif

typedef struct outstream_t{
    uint8_t* data;
    int      data_len;
    int      capacity;
}outstream_t;

static inline void outstream_init(outstream_t* buf){
    buf->data = NULL;
    buf->data_len = 0;
    buf->capacity = 0;
}

static inline void outstream_close(outstream_t* buf){
    FREEIF(buf->data);
    buf->data_len = 0;
    buf->capacity = 0;
}

static inline void outstream_ensure(outstream_t* buf, int new_size){
    if(new_size <= buf->capacity){
        return;
    }

    uint8_t* new_array;
    if (buf->data != NULL){
        new_size = MATH_MAX(new_size, buf->capacity << 1);
        new_array = (uint8_t*)MALLOC(new_size);
        memcpy(new_array, buf->data, buf->data_len);
        FREE(buf->data);
    }else{
        new_size += 32;
        new_array = (uint8_t*)MALLOC(new_size);
    }
    buf->data = new_array;
    buf->capacity = new_size;
}

static inline int outstream_lastchar(outstream_t* buf){
    if(buf->data_len == 0){
        return -1;
    }else{
        return buf->data[buf->data_len-1];
    }
}


static inline void outstream_writebuf(outstream_t* buf, const char* input, int len){
    if(len <=0){
        return;
    }
    int remain = buf->capacity - buf->data_len;
    if(remain < len){
        outstream_ensure(buf, buf->data_len + len);
    }
    memcpy(buf->data + buf->data_len, input, len);
    buf->data_len += len;
}
static inline void outstream_writechar(outstream_t* buf, const char c){
    int remain = buf->capacity - buf->data_len;
    if(remain < 1){
        outstream_ensure(buf, buf->data_len + 1);
    }
    buf->data[buf->data_len] = (uint8_t)c;
    buf->data_len++;
}


#ifdef __cplusplus
}
#endif

#endif //outstream_H

