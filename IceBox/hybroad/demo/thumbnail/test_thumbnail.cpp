#include "Thumbnail.h"

using namespace IBox;

int main(int argc, char**argv) 
{
    Thumbnail thumb;
    char const* input = "/root/before.jpg";
    char const* output = "/root/after.jpg";
    if (argc > 2) {
        input = argv[1];
        output= argv[2];
    }
    int ret = thumb.createBySize(input, output, 320, 320);
    if (ret != Thumbnail::R_SUCCESS) {
        printf("error!\n");
        return -1;
    }
    printf("ok!\n");
    return 0;
}
