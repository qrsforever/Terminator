
#include "DataSink.h"

#include <stdio.h>


namespace IBox {

DataSink::DataSink()
    : m_ringBuffer(NULL)
    , m_dataSize(0)
{
}

DataSink::~DataSink()
{
}

bool 
DataSink::onStart()
{
    return true;
}

bool 
DataSink::onDataArrive()
{
    return true;
}

bool 
DataSink::onError()
{
    return true;
}

bool 
DataSink::onEnd()
{
    return true;
}

} /* namespace IBox */
