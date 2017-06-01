#include "NetworkAssertions.h"
#include "ThunderService.h"
#include "MessageHandler.h"
#include "PlatformAPI.h"
#include "MessageTypes.h"

#include <stdio.h>
#include <stdlib.h>

namespace IBox {

ThunderService::ThunderService(MessageHandler* handler) 
    : mHandler(handler), mTaskState(-1), mBindKey(""), mResult(-1)
{
}

ThunderService::~ThunderService()
{
    if (isRunning())
        etmStop();
}

int 
ThunderService::etmStart(std::string ifname)
{
    NETWORK_LOG("run here!\n");
    std::string cmd("");
    cmd.append(" THUNDER_BIN_DIR=");
    cmd.append("/home/hybroad/bin");
    cmd.append(" THUNDER_SYSTEM_DIR=");
    cmd.append("/root/thunder");
    cmd.append(" THUNDER_CONFIG_DIR=");
    cmd.append("/home/hybroad/share/thunder");
    cmd.append(" THUNDER_TEMP_DIR=");
    cmd.append("/var/run");
    cmd.append(" THUNDER_LICENSE=");
    cmd.append("1505260001000004h0009554wkl5fniembpxa04sek");
    cmd.append(" THUNDER_NETWORK_ADAPTER=");
    cmd.append(ifname);
    cmd.append(" THUNDER_NTFS_TYPE=");
    cmd.append("1");
    cmd.append(" THUNDER_VOD_PORT=");
    cmd.append("8002");
    cmd.append(" THUNDER_LC_PORT=");
    cmd.append("9000");
    cmd.append(" /home/hybroad/bin/start_etm_monitor.sh ");
    Platform().systemCall(cmd.c_str());
    mEtmStart = true;

    if (mTaskState != _Task::_eRunning)
        (new _Task(*this))->start();
    return 0;
}

int 
ThunderService::etmStop()
{ 
    NETWORK_LOG("run here!\n");
    std::string cmd("");

    cmd.append(" THUNDER_BIN_DIR=");
    cmd.append("/home/hybroad/bin");
    cmd.append(" THUNDER_SYSTEM_DIR=");
    cmd.append("/root/thunder");
    cmd.append(" THUNDER_CONFIG_DIR=");
    cmd.append("/home/hybroad/share/thunder");
    cmd.append(" THUNDER_TEMP_DIR=");
    cmd.append("/var/run");
    cmd.append(" THUNDER_PIPE_DIR=");
    cmd.append("/var/run");
    cmd.append(" /home/hybroad/bin/stop_etm_monitor.sh");
    Platform().systemCall(cmd.c_str());
    mEtmStart = false;

    while (mTaskState == _Task::_eRunning)
        usleep(200000);

    NETWORK_LOG("run here!\n");
    return 0;
}

int 
ThunderService::checkStatus()
{
    int trycount;
    int res = 0;
    while (mEtmStart) {
        std::string url = "http://127.0.0.1:9000/getsysinfo";
        //[result,is_net_ok,is_license_ok, is_bind_ok,"bind_acktive_key",is_disk_ok, “version”,”user_name”,is_ever_binded, userid,vip_level] 
        for (trycount = 3; trycount >= 0; trycount--) {
            res = CurlEasy::download(url.c_str(), 1, 1);
            if (!res) {
                trycount = 3;
                break;
            }
        }
        if (!trycount) {
            if (mHandler)
                mHandler->sendMessage(mHandler->obtainMessage(MessageType_Network, MV_NETWORK_THUNDER_STATUS, THUNDER_RUN_ERROR));
            break;
        }
        sleep(5);
    }
    return 0;
}

int 
ThunderService::receiveData(char *buf, size_t size, size_t nmemb)
{
    //buff=[[0,1,21002,0,"",1,"3.955.2.251_30","",1,"0",0]
    //[result,is_net_ok,is_license_ok, is_bind_ok,"bind_acktive_key",is_disk_ok, “version”,”user_name”,is_ever_binded, userid,vip_level] 
    NETWORK_LOG("%s\n", buf);
    if (buf) {
        if (mBindKey.empty()) {
            int a1,a2,a3,a4,a5,a6,a7;
            char b1[16] = { 0 }; //bind_acktive_key
            char b2[16] = { 0 }; //version
            char b3[16] = { 0 }; //user_name
            char b4[16] = { 0 }; //userid
            int num = sscanf(buf, "[%d,%d,%d,%d,%[^,],%d,%[^,],%[^,],%d,%[^,],%d]", &a1, &a2, &a3, &a4, b1, &a5, b2, b3, &a6, b4, &a7); 
            if (11 == num) {
                mResult = a1;
                if (mBindKey.compare(b1)) {
                    mBindKey = b1;
                    if (mHandler)
                        mHandler->sendMessage(mHandler->obtainMessage(MessageType_Network, MV_NETWORK_THUNDER_STATUS, THUNDER_KEY_BIND, 0));
                }
            }
        }
    }
    return size * nmemb;
}

int 
ThunderService::receiveProgress(double total, double now)
{
    return 0;
}

void 
ThunderService::_Task::run()
{
    _Thunder.mTaskState = _eRunning;
    {
        _Thunder.checkStatus(); //block
    }
    _Thunder.mTaskState = _eFinish;

    delete this;
}

} 
