#ifndef __CurlEasy__H_
#define __CurlEasy__H_

#include <stdio.h>

namespace IBox {

class CurlEasy {
public:
    virtual ~CurlEasy();

    void setVerbose(int verbose) { mVerbose = verbose; }
    void setTimeout(int timeout) { mTimeout = timeout; }
    void setConnectTimeout(int timeout) { mConnectTimeout = timeout; }

    static int CURL_WriteCallback(void* ptr, size_t size, size_t nmemb, void* arg);
    static int CURL_ProgressCallback(void *arg, double dltotal, double dlnow, double ult, double uln);

    int download(const char* url, int socketTimeout = 3, int connectTimeout = 3);
    int upload();

protected:
    CurlEasy();
	virtual int receiveData(char *buf, size_t size, size_t nmemb) = 0;
    virtual int receiveProgress(double total, double now) = 0;

private:
    int mVerbose;
    int mTimeout;
    int mConnectTimeout;
};

}

#endif
