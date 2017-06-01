#ifndef __DiskDevice__H_
#define __DiskDevice__H_

#include "Partition.h"
#include "Mutex.h"

#include <vector>
#include <map>
#include <string>

#define DISK_PROC_PARTITIONS "/proc/partitions"

#define BUS_TYPE_UND "UND"
#define BUS_TYPE_USB "USB"
#define BUS_TYPE_ATA "ATA"
#define BUS_TYPE_SDC "SDC"

namespace IBox {

class DiskDevice {
public:
    DiskDevice(std::string name);
    ~DiskDevice();
    enum TransportType {
        e_BUS_UND = 0,
        e_BUS_USB = 1,
        e_BUS_ATA = 2,
        e_BUS_SDC = 3,
    };
    std::string& diskname() { return mDiskName; }
    std::string& buslabel() { return mBusLabel; }
    std::string& bustype() { return mBusType; }
    std::string& getID(); 

    int getPartitionCount();
    int mountPartitions();
    int umountPartitions();
    int createPartitions(int pcount, int type, int* divide);

    int getAllSpace(unsigned long long*, unsigned long long*);

    int init();
    int refresh();
    Partition* getPartition(std::string fsname) { return getPartition(fsname.c_str()); }
    Partition* getPartition(const char* fsname);
    Partition& addPartition(const char* fsname);
    int delPartition(const char* fsname);
    bool isAllPartitionsMounted();
    std::vector<std::string> getAllPartitionNames();

    std::string toJsonString(int flag = 0);

    std::string getSerialNumber();

private:
    std::string _Hex2Dec(const char* hex);
    std::string mID;
    std::string mBusType;
    std::string mDiskName; // eg. sda
    std::string mBusLabel;  // eg. ata1, ata2
    Mutex mMutex; // for mPartitions
    std::map<std::string, Partition*> mPartitions;
};

}

#endif
