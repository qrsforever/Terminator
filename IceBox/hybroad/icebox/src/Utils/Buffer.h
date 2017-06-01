#ifndef __BUFFER__H_
#define __BUFFER__H_

#include <iostream>

namespace IBox {

namespace tools {
class Buffer {
public:
    Buffer(const char * buffer, int size);
    Buffer(int size, char c = '\0');
    Buffer();
    Buffer(const Buffer& o);
    ~Buffer();

    int length() const { return len; }
    const char * buffer() const { return data; }
    bool isEmpty();

    void assign(const char * data, int len);
    void assign(int size, char c = '\0');

    char& at(int offset);
    char& operator[](int offset);
    char operator[] (int offset) const;
    operator char*() { return data; }
    operator unsigned char*() { return (unsigned char*)data; }
    operator void*() { return data; }
    Buffer& operator=(const Buffer& o);
    Buffer& operator=(const Buffer* o);
    unsigned char* operator+(int offset) { return (unsigned char*)data + offset; }

private:
    int     len;
    char*   data;
};

}
}

#endif
