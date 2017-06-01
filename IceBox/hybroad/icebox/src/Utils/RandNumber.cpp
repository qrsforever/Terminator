#include "RandNumber.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

namespace IBox {

int
RandNumber::createInt(int start, int end, int seed)
{
    int e = 0;
    if (-1 == seed)
        e = time(0);

    srand(e);
    return ((double)rand()/RAND_MAX)*(end-start) + start; 
}

std::string
RandNumber::createString(int start, int end, const char* fill, int seed)
{
    char s[256] = { 0 };
    char t[256] = { 0 };
    int div = createInt(start, end, seed);
    int mod = 0;
    while (div) {
        mod = div % 10;
        div = div / 10;
        if (!strlen(t))
            sprintf(s, "%d", mod);
        else 
            sprintf(s, "%d%s%s", mod, (fill == 0 ? "" : fill), t);
        strcpy(t, s);
    }
    return std::string(s);
}

}
