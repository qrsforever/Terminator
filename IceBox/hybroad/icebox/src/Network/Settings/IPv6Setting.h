#ifndef __IPv6Setting__H_
#define __IPv6Setting__H_

#ifdef __cplusplus
#include <string>
#include "BaseSetting.h"

namespace IBox {

class IPv6Setting : public BaseSetting {
public:
    IPv6Setting();
    ~IPv6Setting();

    virtual int type() { return eBS_IPv6; }

    IPv6Setting& operator = (const IPv6Setting& rhs);

    void clear();

    void setAddress(const char* addr);
    const char* getAddress() const;

    void setSubnetPrefix(int prefix);
    int getSubnetPrefix() const;

    void setGateway(const char* gate);
    const char* getGateway() const;

    void setDns0(const char* dns0);
    const char* getDns0() const;

    void setDns1(const char* dns1);
    const char* getDns1() const;

private:
    int mPrefix;
    std::string mAddress;
    std::string mGateway;
    std::string mDns0;
    std::string mDns1;
};

}

#endif

#endif
