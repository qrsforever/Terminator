#ifndef __RandNumber__H_
#define __RandNumber__H_

#include <string>

namespace IBox {

class RandNumber {
public:
    static int createInt(int start = 100000, int end = 999999, int seed = -1);
    static std::string createString(int start = 100000, int end = 999999, const char* fill = 0, int seed = -1);
};

}
#endif
