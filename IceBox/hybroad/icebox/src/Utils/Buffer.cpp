#include "Buffer.h"
#include <stdlib.h>
#include <string.h>
#include <iomanip>

namespace IBox {

namespace tools {

Buffer::Buffer()
    : len(0)
    , data(NULL)
{
}

Buffer::Buffer(const char * buffer, int size)
    : len(0)
    , data(NULL)
{
    assign(buffer, size);
}

Buffer::Buffer(int size, char c)
    : len(0)
    , data(NULL)
{
    assign(size, c);
}

Buffer::Buffer(const Buffer& o)
    : len(0)
    , data(NULL)
{
    assign(o.data, o.len);
}

Buffer::~Buffer()
{
    assign((const char *)NULL, 0);
}

Buffer& Buffer::operator=(const Buffer& o)
{
    assign(o.data, o.len);
    return *this;
}

Buffer& Buffer::operator=(const Buffer* o)
{
    assign(o->data, o->len);
    return *this;
}

void Buffer::assign(const char * buffer, int size)
{
    if (data != NULL) {
        delete[] data;
    }
    len = size;
    data = NULL;

    if (len <= 0) {
        return;
    }
    data = new char[size];
    memcpy(data, buffer, size);
}

void Buffer::assign(int size, char c)
{
    if (data != NULL) {
        delete[] data;
    }
    len = size;
    data = NULL;

    if (size <= 0)
        return;

    data = new char[size];
    memset(data, c, size);
}

bool Buffer::isEmpty()
{
    return (data == NULL);
}

char& Buffer::at(int offset)
{
    if (offset >= len || offset < -len) {
        return *(char*)NULL;
    }
    if (offset < 0) {
        return data[len + offset];
    }
    return data[offset];
}

char& Buffer::operator[](int offset)
{
    return at(offset);
}

char Buffer::operator[] (int offset) const
{
    if (offset >= len || offset < -len) {
        return *(char*)NULL;
    }
    if (offset < 0) {
        return data[len + offset];
    }
    return data[offset];
}

}
}

