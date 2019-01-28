#include "outstream.h"
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

void test_outstream(void){
    outstream_t gb;
    outstream_init(&gb);
    outstream_writebuf(&gb, "x", 1);
    outstream_writebuf(&gb, "01234567890123456789012345678901234567890123456789", 40);
    outstream_writechar(&gb, '\0');
    printf("buf: %s, len: %d", gb.data, gb.data_len);
    outstream_close(&gb);
}

#ifdef __cplusplus
}
#endif
