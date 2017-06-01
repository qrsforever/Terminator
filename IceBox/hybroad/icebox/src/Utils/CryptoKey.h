#ifndef __CRYPTOKEY_H_
#define __CRYPTOKEy_H_

#ifdef __cplusplus

namespace IBox {

class CryptoKey {
public:
    CryptoKey();
    ~CryptoKey();

    int encrypt(char* to, const char* from, int len);
    int decrypt(char* to, const char* from, int len);
};

}
#endif //__cplusplus

#endif
