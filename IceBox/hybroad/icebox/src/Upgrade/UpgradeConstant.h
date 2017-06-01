#ifndef __UpgradeConstant__H_
#define __UpgradeConstant__H_

namespace IBox {

#define TEMP_CONFIG_PATH    "/var/icebox_upgrade_config.ini"
#define TEMP_SOFTWARE_PATH  "/var/icebox_upgrade_software.bin"

#define RESULT_FAIL      -1
#define RESULT_SUCCESS    0
#define RESULT_PROGRESS   1

#define UPGRADE_ERR     -1

#define UPGRADE_BY_UDISK  0
#define UPGRADE_BY_SERVER 1

#define UPGRADE_CHECK   1
#define UPGRADE_START   2
#define UPGRADE_BURNING 3

#define IS_CHECKING     4
#define IS_STARTING     5

#define HAVE_NEW_VERSION 5
#define NO_NEW_VERSION   6

#define SOFTWARE_VERIFY_OK   7 
#define SOFTWARE_VERIFY_FAIL 8

enum {
    eU_RequestConfig,
    eU_RequestSoftware,
    eU_BurningSoftware,
};

}

#endif
