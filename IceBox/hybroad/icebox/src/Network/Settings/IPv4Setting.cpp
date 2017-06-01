#include "IPv4Setting.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>

namespace IBox {

IPv4Setting::IPv4Setting() :
    mAddress("0.0.0.0"),
    mNetmask("0.0.0.0"),
    mGateway("0.0.0.0"),
    mDns0("0.0.0.0"),
    mDns1("0.0.0.0")
{
}

IPv4Setting::~IPv4Setting()
{
    printf("####~IPv4Setting\n");
    clear();
}

IPv4Setting&
IPv4Setting::operator = (const IPv4Setting& rhs)
{
    mAddress = rhs.getAddress();
    mNetmask = rhs.getNetmask();
    mGateway = rhs.getGateway();
    mDns0 = rhs.getDns0();
    mDns1 = rhs.getDns1();
    return *this;
}

void
IPv4Setting::clear()
{
    mAddress.clear();
    mNetmask.clear();
    mGateway.clear();
    mDns0.clear();
    mDns1.clear();
}

void
IPv4Setting::setAddress(const char* addr)
{
    struct in_addr addr4;
    if (addr && inet_pton(AF_INET, addr, &addr4) > 0)
        mAddress = addr;
}

const char*
IPv4Setting::getAddress() const
{
    return mAddress.c_str();
}

void
IPv4Setting::setNetmask(const char* mask)
{
    struct in_addr addr4;
    if (mask && inet_pton(AF_INET, mask, &addr4) > 0)
        mNetmask = mask;
}

const char*
IPv4Setting::getNetmask() const
{
    return mNetmask.c_str();
}

void
IPv4Setting::setGateway(const char* gate)
{
    struct in_addr addr4;
    if (gate && inet_pton(AF_INET, gate, &addr4) > 0)
        mGateway = gate;
}

const char*
IPv4Setting::getGateway() const
{
    return mGateway.c_str();
}

void
IPv4Setting::setDns0(const char* dns0)
{
    struct in_addr addr4;
    if (dns0 && inet_pton(AF_INET, dns0, &addr4) > 0)
        mDns0 = dns0;
}

const char*
IPv4Setting::getDns0() const
{
    return mDns0.c_str();
}

void
IPv4Setting::setDns1(const char* dns1)
{
    struct in_addr addr4;
    if (dns1 && inet_pton(AF_INET, dns1, &addr4) > 0)
        mDns1 = dns1;
}

const char*
IPv4Setting::getDns1() const
{
    return mDns1.c_str();
}

}
