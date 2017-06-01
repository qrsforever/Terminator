#include "DiskAssertions.h"
#include "DiskDevice.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "JsonObjectArray.h"
#include "JsonObject.h"

extern "C" int disk_partition_create(char *dev, int cntOfPartitions, int *dividedPercentage, int systype);

namespace IBox {

DiskDevice::DiskDevice(std::string name) 
    : mDiskName(name), mBusLabel(""), mBusType(BUS_TYPE_UND), mID("")
{
    init();
    // mountPartitions();
}

DiskDevice::~DiskDevice()
{
    // umountPartitions();
}

int 
DiskDevice::init()
{
    char sysblock[256] = { 0 };
    char longlink[256] = { 0 };
    char *pstr = 0, *tstr = 0;

    /*  ls -l /sys/block
     *    mmcblk0 -> ../devices/rdb.7/f03e0000.sdhci/mmc_host/mmc0/mmc0:0007/block/mmcblk0                                                      
     *    sda -> ../devices/f0470300.ehci/usb3/3-1/3-1:1.0/host3/target3:0:0/3:0:0:0/block/sda                                                  
     *    sdb -> ../devices/platform/strict-ahci.0/ata2/host1/target1:0:0/1:0:0:0/block/sdb   
     *  */
    snprintf(sysblock, 255, "/sys/block/%s", mDiskName.c_str());
    int rsize = readlink(sysblock, longlink, 255);
    if (rsize < 0) {
        DISK_LOG("readlink /sys/block/%s error!\n", mDiskName.c_str());
        return -1;
    }
    pstr = strstr(longlink, "ata");
    if (pstr) {
        if (!isdigit(*(pstr+3)))
            pstr = strstr(pstr+1, "ata");
        if (!pstr)
            return -1;
        tstr = strchr(pstr, '/');
        if (tstr) {
            mBusLabel.assign(pstr, (tstr-pstr));
            mBusType = BUS_TYPE_ATA;
        }
    }
    pstr = strstr(longlink, "usb");
    if (pstr) {
        if (!isdigit(*(pstr+3)))
            pstr = strstr(pstr+1, "usb");
        if (!pstr)
            return -1;
        tstr = strchr(pstr, '/');
        if (tstr) {
            mBusLabel.assign(pstr, (tstr-pstr));
            mBusType = BUS_TYPE_USB;
        }
    } 
    pstr = strstr(longlink, "mmc");
    if (pstr) {
        if (!isdigit(*(pstr+3)))
            pstr = strstr(pstr+1, "mmc");
        if (!pstr)
            return -1;
        tstr = strchr(pstr, '/');
        if (tstr) {
            mBusLabel.assign(pstr, (tstr-pstr));
            mBusType = BUS_TYPE_SDC;
        }
    }
    return 0;
}

std::string&
DiskDevice::getID()
{
    if (!mID.empty()) 
        return mID;

    mID = getSerialNumber();
    if (mID.empty()) {
        char buff[33] = { 0 };
        snprintf(buff, 32, "%ld", random());
        mID = buff;
    }
    return mID;
}

std::string 
DiskDevice::getSerialNumber()
{
    if (mBusType.compare(BUS_TYPE_USB) 
        && mBusType.compare(BUS_TYPE_SDC)) 
        return "";

    if (mBusLabel.empty())
        return "";

    FILE *fp = 0;
    char sysblock[256] = { 0 };
    char longlink[256] = { 0 };
    char line[256] = { 0 };
    char *pstr = 0, *tstr = 0;

    /*  ls -l /sys/block
     *    mmcblk0 -> ../devices/rdb.7/f03e0000.sdhci/mmc_host/mmc0/<<mmc0:0007>>/block/mmcblk0                                                      
     *    sda -> ../devices/f0470300.ehci/usb3/<<3-1>>/3-1:1.0/host3/target3:0:0/3:0:0:0/block/sda                                                  
     *    sdb -> ../devices/platform/strict-ahci.0/ata2/host1/target1:0:0/1:0:0:0/block/sdb   
     *  */
    snprintf(sysblock, 255, "/sys/block/%s", mDiskName.c_str());
    int rsize = readlink(sysblock, longlink, 255);
    if (rsize < 0) {
        DISK_LOG("readlink /sys/block/%s error!\n", mDiskName.c_str());
        return "";
    }

    pstr = strstr(longlink, mBusLabel.c_str());
    if (!pstr) 
        return "";
    tstr = strchr(pstr + mBusLabel.length() + 1, '/');
    if (!tstr)
        return "";
    strcpy(sysblock, "/sys/");
    strncat(sysblock, longlink + 3, (tstr-longlink-3));
    strcat(sysblock, "/serial");
    if (access(sysblock, F_OK | R_OK)) {
        DISK_LOG_WARNING("can not access %s\n", sysblock);
        return "";
    }
    fp = fopen(sysblock, "r");
    if (!fp) {
        DISK_LOG_WARNING("can not read %s\n", sysblock);
        return "";
    }
    fgets(line, 255, fp);
    fclose(fp);
    tstr = strchr(line, '\n');
    if (tstr)
        *tstr = 0;
    if (!strncasecmp(line, "0x", 2))
        return _Hex2Dec(line);
    return std::string(line);
}

int
DiskDevice::refresh()
{
    umountPartitions();
    mountPartitions();
    return 0;
}

Partition* 
DiskDevice::getPartition(const char* fsname)
{
    std::map<std::string, Partition*>::iterator it = mPartitions.find(fsname);
    if (it != mPartitions.end()) {
        return it->second;
    }
    return 0;
}

Partition&
DiskDevice::addPartition(const char* fsname)
{
    std::map<std::string, Partition*>::iterator it = mPartitions.find(fsname);
    if (it != mPartitions.end()) {
        it->second->refresh();
        return *(it->second);
    }
    Partition* part = new Partition(fsname, this);

    mMutex.lock();
    mPartitions.insert(std::make_pair(fsname, part));
    mMutex.unlock();
    return *part;
}

int 
DiskDevice::delPartition(const char* fsname)
{
    std::map<std::string, Partition*>::iterator it = mPartitions.find(fsname);
    if (it == mPartitions.end())
        return 1;

    Partition* part = it->second;

    mMutex.lock();
    mPartitions.erase(fsname);
    mMutex.unlock();

    delete part;
    return 0;
}

bool 
DiskDevice::isAllPartitionsMounted()
{
    std::map<std::string, Partition*>::iterator it = mPartitions.begin();
    while (it != mPartitions.end()) {
        if (!it->second->isMounted())
            return false;
        ++it;
    }
    return true;
}

std::vector<std::string> 
DiskDevice::getAllPartitionNames()
{
    std::vector<std::string> vnames;
    std::map<std::string, Partition*>::iterator it = mPartitions.begin();
    while (it != mPartitions.end()) {
        vnames.push_back(it->second->fsname());
        ++it;
    }
    return vnames;
}

int 
DiskDevice::mountPartitions()
{
    int res;
    FILE *fp;
    char line[256] = { 0 };

    fp = fopen(DISK_PROC_PARTITIONS, "r");
    if (!fp) {
        DISK_LOG("can't open file %s\n", DISK_PROC_PARTITIONS);
        return -1;
    }
    char fsname[16] = { 0 };
    int major, minor;
    long long blocks;
    fgets(line, 255, fp);
    fgets(line, 255, fp);
    while (fgets(line, 255, fp)) {
        if (sscanf(line, "%d %d %lld %s", &major, &minor, &blocks, fsname) != 4)       
            continue;
        if (strstr(fsname, mDiskName.c_str()) && strcmp(fsname, mDiskName.c_str()))
            addPartition(fsname).mount();
    }
    fclose(fp);
    return 0;
}

int
DiskDevice::umountPartitions()
{
    std::vector<std::string> vnames = getAllPartitionNames();
    for (int i = 0; i < vnames.size(); ++i)
        delPartition(vnames[i].c_str());
    return 0;
}

int 
DiskDevice::createPartitions(int pcount, int fstype, int* divide)
{
    int res = -1;
    int type = -1;
    std::string device("");
    device.append("/dev/");
    device.append(mDiskName);

    switch (fstype) {
    case Filesystem::e_FS_FAT32:
        type = 0x0c;
        break;
    case Filesystem::e_FS_EXT4:
        type = 0x83;
        break;
    case Filesystem::e_FS_NTFS:
        //TODO
        type = 0x00;
        break;
    default:
        //TODO
        type = 0x83;
    }

    mMutex.lock();
    res = disk_partition_create((char*)device.c_str(), pcount, divide, type);
    if (0 != res) {
        mMutex.unlock();
        return -1;
    }

    char str[16] = { 0 };
    for (int i = 0; i < pcount; ++i) {
        sprintf(str, "%s%d", device.c_str(), (i+1));
        res = Partition::format(str, fstype);
        DISK_LOG("format %s res : %d\n", str, res);
    }
    mMutex.unlock();
    return res;
}

int 
DiskDevice::getPartitionCount() 
{
    return (int)mPartitions.size();
}

int 
DiskDevice::getAllSpace(unsigned long long* total, unsigned long long* avail)
{
    *total = 0;       
    *avail = 0;
    unsigned long long t_total = 0;
    unsigned long long t_avail = 0;
    std::map<std::string, Partition*>::iterator it = mPartitions.begin();
    while (it != mPartitions.end()) {
        if (!it->second->getSpace(&t_total, &t_avail)) {
            *total += t_total;
            *avail += t_avail;
        }
        ++it;
    }
    return 0;
}

std::string
DiskDevice::_Hex2Dec(const char* hexstr)
{
    if (!hexstr)
        return "";
    unsigned long long dec=0;
    for(int i=0; hexstr[i]!='\0'; i++) {
        if (hexstr[i] >= '0'  && hexstr[i] <= '9') 
            dec = dec*16 + hexstr[i] - '0'; 
        else if(hexstr[i]>='A' && hexstr[i] <= 'F') 
            dec = dec*16 + hexstr[i] - 'A' + 10; 
        else if(hexstr[i] >= 'a' && hexstr[i] <='f') 
            dec = dec*16 + hexstr[i] - 'a' + 10;
    }
    char decstr[32] = { 0 };
    snprintf(decstr, 31, "%llu", dec);
    return std::string(decstr);
}

std::string 
DiskDevice::toJsonString(int flag)
{
    unsigned long long total = 0, avail = 0;
    char totalStr[128] = { 0 };
    char availStr[128] = { 0 };

    getAllSpace(&total, &avail);
    snprintf(totalStr, 127, "%lluKB", total);
    snprintf(availStr, 127, "%lluKB", avail);

    JsonObject obj;
    JsonObjectArray array;

    obj.add("Disk", mDiskName);
    obj.add("BusLabel", mBusLabel);
    obj.add("BusType", mBusType);
    obj.add("SerialNumber", getSerialNumber());
    obj.add("TotalSize", totalStr);
    obj.add("AvailSize", availStr);
    obj.add("IsMountedAll", isAllPartitionsMounted() ? "1" : "0");

    if (flag) {
        std::map<std::string, Partition*>::iterator it = mPartitions.begin();
        int count = 0;
        while (it != mPartitions.end()) {
            array.add(it->second->toJsonString());
            ++it;
            count++;
        }
        obj.add("PartitionsCount", count);
        obj.add("Partitions", array.toString());
    }
    return obj.toString();
}

}
