#include "CryptoKey.h"

#include <string.h>

namespace IBox {

CryptoKey::CryptoKey()
{
}

CryptoKey::~CryptoKey()
{
}

int 
CryptoKey::encrypt(char* to, const char* from, int len)
{
    memcpy(to, from, len);
    return len;
}

int 
CryptoKey::decrypt(char* to, const char* from, int len)
{
   memcpy(to, from, len); 
   return len;
}

}
