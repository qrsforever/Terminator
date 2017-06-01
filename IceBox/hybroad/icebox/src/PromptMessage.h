#ifndef __PROMPTMESSAGE__H_
#define __PROMPTMESSAGE__H_ 

#define FP_REGISTER     "按键功能: 注册新用户"
#define FP_POWEROFF     "按键功能: 关闭系统" 
#define FP_RESET        "按键功能: 恢复配置"
#define FP_UNDEF        "按键功能: 尚未定义"

namespace IBox {

typedef enum {
    ePM_SoftwareVersion = 0x001,
    ePM_PowerOffAsk, 
    ePM_RebootAsk,
    ePM_ConfResetAsk,
    ePM_AuthCode,
    ePM_KeyPressFP,
    ePM_DebugAsk,

    ePM_NetworkError = 0x101,
    ePM_NtpSyncError,
    ePM_NetworkAddress,
    ePM_EasysetIDCode,
    ePM_EasysetSuccess,
    ePM_EasysetFailtry,

    ePM_XmppRegisterAsk = 0x201,
    ePM_BoxNotConnected,
    ePM_BoxLoginSuccess,
    ePM_BoxLoginFail,
    ePM_XmppRegisterUser,

    ePM_DiskFormat = 0x301,
    ePM_DiskCopyTimingDown,

    ePM_UpgradeTimingDown = 0x401,
    ePM_UDownloadSuccess,
    ePM_UDownloadFail,
    ePM_UVerifySuccess,
    ePM_UVerifyFail,
    ePM_UBurningSuccess,
    ePM_UBurningFail,
} PromptMessageID_t;

const char* GetMessages(int);

}
#endif
