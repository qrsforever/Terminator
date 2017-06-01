#include "curl/curl.h"
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

size_t write_data(void* ptr,size_t size,size_t nmemb, void* arg)
{
    int fd = *(int*)arg;

    write(fd, ptr, size*nmemb);

    return size*nmemb;

}

int main(int argc, char** argv)
{
    CURLcode res= CURLE_OK;
    CURL* lCurl = NULL;

    int fd;
    if ((fd = open("/var/icebox.bin",O_WRONLY|O_CREAT|O_TRUNC,0777)) < 0)
    {
        printf("fail to open!");
        return -1;
    }
    curl_global_init(CURL_GLOBAL_ALL); /* sets up the program environment that libcurl needs */
    lCurl = curl_easy_init(); /* start a libcurl easy session */
    if(lCurl) {
        curl_easy_setopt(lCurl,CURLOPT_URL,"file:////mnt/usb2/iceupgrade//HY7252_ICEBOX_1.0.519.bin");

        curl_easy_setopt(lCurl,CURLOPT_TIMEOUT,3);

        curl_easy_setopt(lCurl, CURLOPT_WRITEFUNCTION, write_data);

        curl_easy_setopt(lCurl, CURLOPT_WRITEDATA, &fd);

        res = curl_easy_perform(lCurl);

        if(res != CURLE_OK)
            printf("curl_easy_perform not return CURLE_OK!, res = %d\n", res);
        curl_easy_cleanup(lCurl);
    }
    close(fd);
    return 0;
}
