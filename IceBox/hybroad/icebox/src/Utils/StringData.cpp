#include "StringData.h"

#include <stdio.h>
#include <string.h>

#define DEBUG_MEM_LEAK 0

namespace IBox {

StringData::StringData(const char* data) : mData(0), mSize(0)
{
#if DEBUG_MEM_LEAK 
    printf("###DEBUG### StringData %s\n", data);
#endif
    if (data) {
        mSize = strlen(data) + 1;
        mData = new char[strlen(data) + 1];
        strcpy(mData, data);
    }
}

StringData::~StringData() 
{ 
#if DEBUG_MEM_LEAK 
    printf("###DEBUG### ~StringData %s\n", data);
#endif
    if (mData) 
        delete []mData; 
}

}
