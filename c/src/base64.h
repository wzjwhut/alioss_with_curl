#ifndef EQ_BASE64_H
#define EQ_BASE64_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

static char Base64Code[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
'8', '9', '+', '/', '=', '\0' };

static const uint8_t data_ascii2bin[128] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0xF0, 0xFF,
    0xFF, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xF2, 0xFF, 0x3F,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12,
    0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24,
    0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30,
    0x31, 0x32, 0x33, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

inline static uint8_t conv_ascii2bin(uint8_t a)
{
    if (a >= 128)
    {
        return 0xFF;
    }
    return data_ascii2bin[a];
}

#define PADDING	    '='

inline static void base256to64(uint8_t c1, uint8_t c2, uint8_t c3,
                        int padding, char *output)
{
    *output++ = Base64Code[c1 >> 2];
    *output++ = Base64Code[((c1 & 0x3) << 4) | ((c2 & 0xF0) >> 4)];
    switch (padding)
    {
    case 0:
        *output++ = Base64Code[((c2 & 0xF) << 2) | ((c3 & 0xC0) >> 6)];
        *output = Base64Code[c3 & 0x3F];
        break;
    case 1:
        *output++ = Base64Code[((c2 & 0xF) << 2) | ((c3 & 0xC0) >> 6)];
        *output = PADDING;
        break;
    case 2:
    default:
        *output++ = PADDING;
        *output = PADDING;
        break;
    }
}


inline static int calcEncodedMaxLength(int in_len){
        return (in_len + 2 - ((in_len + 2) % 3)) * 4 / 3;
}
inline static int calcDecodedMaxLength(int in_len){
        return ((in_len + 3) >> 2) * 3;
}

inline static void Base64Encode(const uint8_t *input, int in_len, char *output, int* out_len){
    const uint8_t *pi = input;
    uint8_t c1, c2, c3;
    int i = 0;
    char *po = output;

    while (i < in_len){
        c1 = *pi++;
        ++i;

        if (i == in_len){
            base256to64(c1, 0, 0, 2, po);
            po += 4;
            break;
        }else{
            c2 = *pi++;
            ++i;

            if (i == in_len){
                base256to64(c1, c2, 0, 1, po);
                po += 4;
                break;
            }else{
                c3 = *pi++;
                ++i;
                base256to64(c1, c2, c3, 0, po);
            }
        }
        po += 4;
    }
    *out_len = (int)(po - output);
}

inline static int Base64Decode(const char* in, int in_len, char *out, int* out_len)
{
    uint8_t a, b, c, d;
    int pad_len = 0, len = 0;
    int i;
    uint32_t l;


    if (in_len % 4 != 0)
    {
        return -1;
    }
    int need_out_len = (in_len / 4) * 3;
    if (need_out_len > *out_len)
    {
        return -1;
    }

    for (i = 0; i < in_len; i += 4)
    {
        a = conv_ascii2bin(*(in++));
        b = conv_ascii2bin(*(in++));
        if (i + 4 == in_len && in[1] == '=')
        {
            if (in[0] == '=')
            {
                pad_len = 2;
            }
            else
            {
                pad_len = 1;
            }
        }
        if (pad_len < 2)
        {
            c = conv_ascii2bin(*(in++));
        }
        else
        {
            c = 0;
        }
        if (pad_len < 1)
        {
            d = conv_ascii2bin(*(in++));
        }
        else
        {
            d = 0;
        }
        if ((a & 0x80) || (b & 0x80) || (c & 0x80) || (d & 0x80))
        {
            return 0;
        }
        l = ((((uint32_t)a) << 18L) | (((uint32_t)b) << 12L) |
             (((uint32_t)c) << 6L) | (((uint32_t)d)));
        *(out++) = (uint8_t)(l >> 16L) & 0xff;
        if (pad_len < 2)
        {
            *(out++) = (uint8_t)(l >> 8L) & 0xff;
        }
        if (pad_len < 1)
        {
            *(out++) = (uint8_t)(l)& 0xff;
        }
        len += 3 - pad_len;
    }
    *out_len = len;
    return 0;
}


#ifdef __cplusplus
}
#endif

#endif // EQ_BASE64_H
