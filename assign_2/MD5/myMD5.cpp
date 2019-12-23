#include "myMD5.hpp"

void type_change(byte* bytes, bit32* bit32s, size_t len, int judge)
{
    size_t i, j;
    switch (judge){
      // bit32->byte
      case 0:
        for (size_t i = 0, j = 0; j < len; ++i, j += 4) {
            bytes[j + 0] = (byte)(bit32s[i] & 0xff);
            bytes[j + 1] = (byte)((bit32s[i] >> 8) & 0xff);
            bytes[j + 2] = (byte)((bit32s[i] >> 16) & 0xff);
            bytes[j + 3] = (byte)((bit32s[i] >> 24) & 0xff);
        }
        break;

      // byte->bit32
      case 1:
        for (size_t i = 0, j = 0; j < len; ++i, j += 4) {
            bit32s[i] = ((bit32)bytes[j]) | (((bit32)bytes[j + 1]) << 8) | (((bit32)bytes[j + 2]) << 16) | (((bit32)bytes[j + 3]) << 24);
        }
        break;
    
    default:
        break;
    }
}

void round(byte block[64])
{
    bit32 a = state[0], b = state[1], c = state[2], d = state[3], x[16];

    TC(block, x, 64, 1);

    for(size_t i = 0; i < 64; ++i) {
        if (0 <= i && i < 16) {
            if (i % 4 == 0) FF(a, b, c, d, x[X[i]], s[i], T[i]);
            if (i % 4 == 1) FF(d, a, b, c, x[X[i]], s[i], T[i]);
            if (i % 4 == 2) FF(c, d, a, b, x[X[i]], s[i], T[i]);
            if (i % 4 == 3) FF(b, c, d, a, x[X[i]], s[i], T[i]);
        }
        if (16 <= i && i < 32) {
            if (i % 4 == 0) GG(a, b, c, d, x[X[i]], s[i], T[i]);
            if (i % 4 == 1) GG(d, a, b, c, x[X[i]], s[i], T[i]);
            if (i % 4 == 2) GG(c, d, a, b, x[X[i]], s[i], T[i]);
            if (i % 4 == 3) GG(b, c, d, a, x[X[i]], s[i], T[i]);
        }
        if (32 <= i && i < 48) {
            if (i % 4 == 0) HH(a, b, c, d, x[X[i]], s[i], T[i]);
            if (i % 4 == 1) HH(d, a, b, c, x[X[i]], s[i], T[i]);
            if (i % 4 == 2) HH(c, d, a, b, x[X[i]], s[i], T[i]);
            if (i % 4 == 3) HH(b, c, d, a, x[X[i]], s[i], T[i]);
        }
        if (48 <= i && i < 64) { 
            if (i % 4 == 0) II(a, b, c, d, x[X[i]], s[i], T[i]);
            if (i % 4 == 1) II(d, a, b, c, x[X[i]], s[i], T[i]);
            if (i % 4 == 2) II(c, d, a, b, x[X[i]], s[i], T[i]);
            if (i % 4 == 3) II(b, c, d, a, x[X[i]], s[i], T[i]);
        }
    }

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
}

void MD5(byte* input, size_t len)
{
    bit32 i, j, l;

    j = (bit32)((count[0] >> 3) & 0x3f);

    if ((count[0] += ((bit32)len << 3)) < ((bit32)len << 3)) ++count[1];

    count[1] += ((bit32)len >> 29);
    l = 64 - j;

    if (len >= l) {
        CPY(&buffer[j], input, l);
        round(buffer);
        for (i = l; i + 63 < len; i += 64) round(&input[i]);
        j = 0;
    } 
    else {
        i = 0;
    }

    CPY(&buffer[j], &input[i], len - i);
}

void calculate_digest(string msg)
{
    byte* input = (byte*)msg.c_str();
    size_t len = msg.length();
    MD5(input, len);

    byte bytes[8];
    bit32 i, l, s[4], c[2];

    CPY(s, state, 16);
    CPY(c, count, 8);

    TC(bytes, count, 8, 0);

    i = (bit32)((count[0] >> 3) & 0x3f);
    l = (i < 56) ? (56 - i) : (120 - i);

    MD5(PADDING, l);
    MD5(bytes, 8);

    TC(digest, state, 16, 0);

    CPY(state, s, 16);
    CPY(count, c, 8);
}

void print_ans(string msg)
{
    string str;
    for (size_t i = 0; i < 16; ++i) {
        str.append(1, PRINT_CHAR[digest[i]/16]);
        str.append(1, PRINT_CHAR[digest[i]%16]);
    }
    cout<<"Input : "<<msg<<"\n"<<"M D 5 : "<<str<<endl;
}