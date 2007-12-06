const char base85_chars[] =
    "0123456789"                       /* 10 Digits */
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"       /* 26 Uppercase letters */
    "abcdefghijklmnopqrstuvwxyz"       /* 26 Lowercase letters */
    "$%&()*+-:;<=>@[]^_`{|}~";         /* 23 Other characters */

const char base85_codes[96] = {
    -1, -1, -1, -1, 62, 63, 64, -1, 65, 66, 67, 68, -1, 69, -1, -1,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 70, 71, 72, 73, 74, -1,
    75, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 76, -1, 77, 78, 79,
    80, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
    51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 81, 82, 83, 84, -1 };

int base85_char(int c)
{
    return c >= 36 && c < 127 && base85_codes[c - 32] >= 0;
}

/*  FIXME: possible integer overflow */

int base85_encode_len(int len)
{
    int res = (len/4)*5;
    if (len%4 > 0)
        res += 1 + len%4;
    return res;
}

int base85_decode_len(int len)
{
    int res = (len/5)*4;
    if (len%5 > 1)
        res += len%5 - 1;
    return res;
}

int base85_real_decode_len(char *buf, int len)
{
    int n = 0;
    while (len-- > 0)
        n += base85_char(*buf++);
    return base85_decode_len(n);
}

int base85_encode(const char *buf_in, int len_in, char *buf_out)
{
    unsigned char *in, *out;
    unsigned long word;
    int n, m;

    in  = (unsigned char*)buf_in;
    out = (unsigned char*)buf_out;

    for (n = 0; n < len_in/4; ++n)
    {
        word = ((unsigned long)in[0] <<  0) | ((unsigned long)in[1] <<  8) |
               ((unsigned long)in[2] << 16) | ((unsigned long)in[3] << 24);
        in += 4;
        for (m = 0; m < 5; ++m)
        {
            *out++ = base85_chars[word%85];
            word /= 85;
        }
    }

    if (len_in%4 > 0)
    {
        word = 0;
        for (n = 0; n < len_in%4; ++n)
            word |= (unsigned long)in[n] << 8*n;
        for (n = 0; n < len_in%4 + 1; ++n)
        {
            *out++ = base85_chars[word%85];
            word /= 85;
        }
    }

    return (char*)out - buf_out;
}

int base85_decode(const char *buf_in, int len_in, char *buf_out)
{
    int n, m, c;
    unsigned char *out, *in;
    unsigned long word;
    static const unsigned long pow85[5] = { 1, 85, 85*85, 85*85*85, 85*85*85*85 };

    in  = (unsigned char *)buf_in;
    out = (unsigned char *)buf_out;

    m = 0, word = 0;
    while (len_in-- > 0)
    {
        c = *in++;
        if (c >= 36 && c < 127 && (c = base85_codes[c - 32]) >= 0)
        {
            word += pow85[m++]*c;
            if (m == 5)
            {
                for (m = 0; m < 4; ++m)
                {
                    *out++ = word&255;
                    word >>= 8;
                }
                m = 0, word = 0;
            }
        }
    }
    for (n = 0; n < m - 1; ++n)
    {
        *out++ = word&255;
        word >>= 8;
    }

    return (char*)out - buf_out;
}
