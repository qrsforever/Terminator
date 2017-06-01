#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <openssl/md5.h>
#include <openssl/des.h>

//TODO need remodify if hw need.
char* hw_generate_option60(char* value, const char* userid, const char* password, int enterprise, long int xid)
{
    if (!value || !userid || !password)
        return 0;
    const int kTxtLength = strlen(userid) / 8 + 1;
    const int kRemainder = strlen(userid) % 8;
    if (kTxtLength > 16)
        return 0;

    unsigned char R[9] = { 0 };
    unsigned char TS[9] = { 0 };
    unsigned char C[kTxtLength * 8];
    unsigned char Key[16] = { 0 };
    unsigned char O[1] = { 1 };
    unsigned char Message[512] = { 0 };

    int i = 0, j = 0, len = 0;
    DES_cblock key1, key2, key3;
    DES_cblock in[kTxtLength], out[kTxtLength];
    DES_key_schedule ks1, ks2, ks3;
    MD5_CTX md5;
    unsigned char* p = &Message[0];
    unsigned char head[4] = { 0 };

    srand(time(0));
    //1. generate R (8Bytes)
    snprintf((char*)R, 9, "%08u", (unsigned int)(xid % 100000000));
    memcpy(key1, R, 8);

    //2. generate TS (8Bytes)
    snprintf((char*)TS, 9, "%08u", (unsigned int)(time(0) % 100000000));
    memcpy(key2, TS, 8);

    //3. generate C = EnCry(R+TS+64Bit, Login), here Login = userid
    memset(key3, 0, 8);
    DES_set_key_unchecked(&key1, &ks1);
    DES_set_key_unchecked(&key2, &ks2);
    DES_set_key_unchecked(&key3, &ks3);
    for(i = 0; i < kTxtLength; ++i) {
        for(j = 0; j < 8; ++j)
            in[i][j] = 0;
    }
    for(i = 0; i < kTxtLength; ++i) {
        for(j = 0; j < 8; ++j) {
            if(i == kTxtLength - 1 && j >= kRemainder)
                in[i][j] = 0x08 - kRemainder;
            else
                in[i][j] = userid[i * 8 + j];
        }
    }
    for(i = 0; i < kTxtLength; ++i)
        DES_ecb3_encrypt(&in[i], &out[i], &ks1, &ks2, &ks3, DES_ENCRYPT);
    memcpy(C, out, kTxtLength * 8);

    //4. generate Key = Hash(R+Password+TS) (16Bytes), here Hash use MD5 instead.
    MD5_Init(&md5);
    MD5_Update(&md5, R, strlen((char*)R));
    MD5_Update(&md5, password, strlen(password));
    MD5_Update(&md5, TS, strlen((char*)TS));
    MD5_Final(Key, &md5);

    //5. send Message = O+R+TS+Key+C
    p = (unsigned char*)memcpy(p, &O, 1) + 1; //Android not support mempcpy, so mod.
    p = (unsigned char*)memcpy(p, &R, 8) + 8;
    p = (unsigned char*)memcpy(p, &TS, 8) + 8;
    p = (unsigned char*)memcpy(p, &Key, 16) + 16;
    p = (unsigned char*)memcpy(p, &C, kTxtLength * 8) + kTxtLength * 8;

    //6. encapsulate option60 infomation
    head[0] = (enterprise & 0x0000FF00) >> 8;
    head[1] = enterprise & 0x000000FF;
    head[2] = 31; //fix value 31
    head[3] = p - Message;
    len = 0;
    for (i = 0; i < 4; ++i) 
        len += sprintf(value + len, "%02x:", head[i]);
    for (i = 0; i < head[3]; ++i)
        len += sprintf(value + len, "%02x:", Message[i]);
    value[len - 1] = 0;
    return value;
}
