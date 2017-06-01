#ifndef _MessageTypes_H_
#define _MessageTypes_H_

#ifdef __cplusplus

namespace IBox {


} // namespace IBox

#endif // __cplusplus

typedef enum {
    MessageType_DesktopRefresh = 0,
	MessageType_System = 1,
	MessageType_KeyDown = 2,
	MessageType_KeyUp = 3,
    MessageType_Network = 4,
    MessageType_Repaint = 5,  
    MessageType_Weather = 6,
    MessageType_Upgrade = 7,
    MessageType_Xmpp = 8,
    MessageType_Database = 9,
    MessageType_Disk = 10,
    MessageType_Prompt = 11,
    MessageType_Unknow = 9999
} MessageTypes;

#define MV_NETWORK_CONNECT_OK           0x000
#define MV_NETWORK_CONNECT_ERROR        0x001
#define MV_NETWORK_NTPSYNC_OK           0x002
#define MV_NETWORK_NTPSYNC_ERROR        0x003
#define MV_NETWORK_NTPSYNC_TIMEOUT      0x004
#define MV_NETWORK_NTPSYNC_AGAIN        0x005
#define MV_NETWORK_EASYSETUP_START      0x006
#define MV_NETWORK_EASYSETUP_OK         0x007
#define MV_NETWORK_EASYSETUP_ERROR      0x008
#define MV_NETWORK_EASYSETUP_TIMEOUT    0x009
#define MV_NETWORK_EASYSETUP_QUERING    0x010
#define MV_NETWORK_EASYSETUP_INTERRUPT  0x011
#define MV_NETWORK_JOINAP_OK            0x012
#define MV_NETWORK_JOINAP_ERROR         0x013
#define MV_NETWORK_LINK_DOWN            0x014
#define MV_NETWORK_LINK_UP              0x015
#define MV_NETWORK_THUNDER_STATUS       0x016


#define MV_XMPP_ICEBOX_LOGINOK     0x100
#define MV_XMPP_ICEBOX_LOGINERR    0x101

#define MV_DATABASE_REQUEST      0x200
#define MV_DATABASE_RESPONSE     0x201
#define MV_DATABASE_ADDDISK      0x202
#define MV_DATABASE_REMOVEDISK   0x203
#define MV_DATABASE_OPENERROR    0x204

#define MV_DISK_ADD            0x300
#define MV_DISK_REMOVE         0x301
#define MV_DISK_MOUNT          0x302
#define MV_DISK_UNMOUNT        0x303
#define MV_DISK_COPY           0x304
#define MV_DISK_COPY_SCAN      0x305
#define MV_DISK_COPY_PROGRESS  0x306
#define MV_DISK_COPY_NEXT      0x307
#define MV_DISK_COPY_END       0x308
#define MV_DISK_COPY_RECEIVE   0x309
#define MV_DISK_COPY_WAITATA   0x310
#define MV_DISK_COPY_WAITSCAN  0x311

#define MV_SYSTEM_HDISK0_DOOR  0x400
#define MV_SYSTEM_HDISK1_DOOR  0x401
#define MV_SYSTEM_POWER_DOWN   0x402    
#define MV_SYSTEM_UPS_CHARGE   0x403

#define MV_PROMPT_ADD       0x500
#define MV_PROMPT_REMOVE    0x501
#define MV_PROMTP_TIMEOUT   0x502

#define MV_PRESSKEY_REGISTUSER 0x600
#define MV_PRESSKEY_SHORTFP    0x601
#define MV_PRESSKEY_LEFT       0x602
#define MV_PRESSKEY_UP         0x603
#define MV_PRESSKEY_RIGHT      0x604
#define MV_PRESSKEY_DOWN       0x605
#define MV_PRESSKEY_ENTER      0x606
#define MV_PRESSKEY_POWER      0x607
#define MV_PRESSKEY_RESET      0x608
#define MV_PRESSKEY_UNDEF      0x609

#define MV_UPGRADE_CHECK        0x700
#define MV_UPGRADE_AGREE        0x701
#define MV_UPGRADE_REQCONFIG    0x702
#define MV_UPGRADE_DPROGRESS    0x704
#define MV_UPGRADE_START        0x705
#define MV_UPGRADE_REQSOFTWARE  0x706
#define MV_UPGRADE_BURNING      0x707
#define MV_UPGRADE_BPROGRESS    0x708
#define MV_UPGRADE_END          0x709

#define MV_WEATHER_QUERY      0x800
#define MV_WEATHER_QUERY_OK   0x801
#define MV_WEATHER_QUERY_ERR  0x802

#endif // _MessageTypes_H_
