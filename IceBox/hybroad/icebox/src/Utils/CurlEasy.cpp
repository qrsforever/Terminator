#include "Assertions.h"
#include "CurlEasy.h"

#include "curl/curl.h"
#include <unistd.h>
#include <string.h>

namespace IBox {

CurlEasy::CurlEasy()
    : mVerbose(0), mTimeout(0), mConnectTimeout(5)
{

}

CurlEasy::~CurlEasy()
{

}

int
CurlEasy::CURL_WriteCallback(void* ptr,size_t size,size_t nmemb, void* arg)
{
    CurlEasy* self = (CurlEasy*)arg;
    if (self)
        return self->receiveData((char*)ptr, size, nmemb);
    return 0;
}

int
CurlEasy::CURL_ProgressCallback(void *arg, double dltotal/*download*/, double dlnow, double ult/*upload*/, double uln)
{
    CurlEasy* self = (CurlEasy*)arg;
    if (self)
        return self->receiveProgress(dltotal, dlnow);
    return -1;
}

int 
CurlEasy::download(const char* url, int socketTimeout, int connectTimeout)
{
    CURLcode res= CURLE_OK;
    CURL* lCurl = 0;

    lCurl = curl_easy_init(); /* start a libcurl easy session */
    if(!lCurl)
        return -1 * CURL_LAST;

    //remove special characters
    int b = strspn(url, "\n\r\f\t ");
    int e = strcspn(url + b, "\n\r\f\t ");
    char turl[256] = { 0 };
    strncpy(turl, url + b, e);
    RUNNING_LOG("url = [%s] -> [%s]\n", url, turl);

    curl_easy_setopt(lCurl, CURLOPT_URL, turl);

    curl_easy_setopt(lCurl, CURLOPT_VERBOSE, mVerbose);
    curl_easy_setopt(lCurl, CURLOPT_TIMEOUT, mTimeout);
    curl_easy_setopt(lCurl, CURLOPT_CONNECTTIMEOUT, mConnectTimeout);

    curl_easy_setopt(lCurl, CURLOPT_AUTOREFERER, 1);
    curl_easy_setopt(lCurl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(lCurl, CURLOPT_MAXREDIRS, 1);

    curl_easy_setopt(lCurl, CURLOPT_WRITEFUNCTION, CurlEasy::CURL_WriteCallback);
    curl_easy_setopt(lCurl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(lCurl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(lCurl, CURLOPT_PROGRESSFUNCTION, CurlEasy::CURL_ProgressCallback);
    curl_easy_setopt(lCurl, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(lCurl, CURLOPT_PROGRESSDATA, this);

    res = curl_easy_perform(lCurl); /* block */

    curl_easy_cleanup(lCurl);

    // 28 = CURLE_OPERATION_TIMEDOUT
    return res == CURLE_OK ? 0 : (-1 * res);
}

int 
CurlEasy::upload()
{
    //TODO not finished
    return 0;
}

}
