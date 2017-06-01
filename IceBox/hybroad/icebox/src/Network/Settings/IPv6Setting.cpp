#include "IPv6Setting.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace IBox {

IPv6Setting::IPv6Setting()
{
}

IPv6Setting::~IPv6Setting()
{
    clear();
}

IPv6Setting&
IPv6Setting::operator = (const IPv6Setting& rhs)
{
    mAddress = rhs.getAddress();
    mGateway = rhs.getGateway();
    mPrefix  = rhs.getSubnetPrefix();
    mDns0 = rhs.getDns0();
    mDns1 = rhs.getDns1();
    return *this;
}

void
IPv6Setting::clear()
{
    mAddress.clear();
    mGateway.clear();
    mDns0.clear();
    mDns1.clear();
}

void
IPv6Setting::setAddress(const char* addr)
{
    struct in6_addr addr6;
    if (addr && inet_pton(AF_INET6, addr, &addr6) > 0)
        mAddress = addr;
}

const char*
IPv6Setting::getAddress() const
{
    return mAddress.c_str();
}

void
IPv6Setting::setSubnetPrefix(int prefix)
{
    mPrefix = prefix;
}

int
IPv6Setting::getSubnetPrefix() const
{
    return mPrefix;
}

void
IPv6Setting::setGateway(const char* gate)
{
    struct in6_addr addr6;
    if (gate && inet_pton(AF_INET6, gate, &addr6) > 0)
        mGateway = gate;
}

const char*
IPv6Setting::getGateway() const
{
    return mGateway.c_str();
}

void
IPv6Setting::setDns0(const char* dns0)
{
    struct in6_addr addr6;
    if (dns0 && inet_pton(AF_INET6, dns0, &addr6) > 0)
        mDns0 = dns0;
}

const char*
IPv6Setting::getDns0() const
{
    return mDns0.c_str();
}

void
IPv6Setting::setDns1(const char* dns1)
{
    struct in6_addr addr6;
    if (dns1 && inet_pton(AF_INET6, dns1, &addr6) > 0)
        mDns1 = dns1;
}

const char*
IPv6Setting::getDns1() const
{
    return mDns1.c_str();
}

}
