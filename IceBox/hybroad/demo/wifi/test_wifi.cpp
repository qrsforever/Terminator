#include <stdio.h>

extern "C" {
#include "libzebra.h"
#include "wireless.h"
int yos_systemcall_runSystemCMD(char*, int*);
}


int main(int argc, char * argv[]) 
{ 
    int wlan0_count = 0; 
    int i = 100; 
    char ssid[256] = {0}; 
    int signal_strength = -1; 
    char auth_mode[256] = {0}; 
    char encrypt_type[256] = {0}; 
    //int index1 = 0, index2 = 0, status = 0; 
    int ret = -1; 

    yhw_board_init(); 

    // getchar(); 

    yos_net_setWLANType(HYBROAD_WLAN_WPA_SUPPLICANT); 

    yos_net_setWpaSupplicantDriverName("nl80211"); 

    yos_net_initWLAN(); 

    // yos_systemcall_runSystemCMD("wpa_cli -p/var/run/wpa_supplicant scan", &ret);     
    // yos_net_scanWLAN(); 

    while(1) { 
        // yos_net_getWLANCount(&wlan0_count); 
        yos_systemcall_runSystemCMD("wpa_cli -p/var/run/wpa_supplicant scan", &ret);     
        sleep(3);
        yos_systemcall_runSystemCMD("wpa_cli -p/var/run/wpa_supplicant scan_results ", &ret);
        // printf("get wlan0 count %d\n", wlan0_count); 
        for (i = 0; i < wlan0_count; i++) { 
            memset(ssid, 0, 256); 
            memset(auth_mode, 0, 256); 
            memset(encrypt_type, 0, 256); 
            signal_strength = -1; 
            yos_net_getWLANResult(i, ssid, &signal_strength, auth_mode, encrypt_type); 
            printf("ssid = %s, signal_strength = %d, auth_mode = %s, encrypt_type = %s\n", ssid, signal_strength, auth_mode, encrypt_type); 
            if (!strncmp(ssid, "yxsofAP", 7))
                break;
        } 

        sleep(1); 
    } 

    //printm("index1 = %d, index2 = %d\n", index1, index2); 

    getchar(); 

    while (1) { 
        ret = yos_net_configWLAN("yxsoftAP", "yuxing6868", "WPA2-PSK", ""); 

        printf("yos_net_configWLAN returns %d\n", ret); 

        ret = yos_net_connectWLAN(); 

        printf("yos_net_connectWLAN returns %d\n", ret); 

        remove("/var/dhcpc/dhcpcd-wlan0.pid"); 

        system("dhcpcd wlan0"); 

        system("ifconfig"); 

        getchar(); 
        break; 
#if 0 
        ret = yos_net_configWLANbyIndex(index2, "yxsoft65", "ihaveadream", "", ""); 

        printm("yos_net_configWLAN returns %d\n", ret); 

        ret = yos_net_connectWLAN(); 

        printm("yos_net_connectWLAN returns %d\n", ret); 

        remove("/var/dhcpc/dhcpcd-wlan0.pid"); 

        system("dhcpcd wlan0"); 

        system("ifconfig"); 

        getchar(); 
#endif 
    } 

    yos_net_uninitWLAN(); 

    getchar(); 
    return 0; 
} 

#if 0
int main(int argc, char const* argv[])
{
    int count = 4, signal = 0;
    char ssid[64] = { 0 };
    char auth[64] = { 0 };
    char encr[64] = { 0 };
    yhw_board_init();
    sleep(3);
    yos_net_setWpaSupplicantDriverName("nl80211"); 
    yos_net_setWLANType(HYBROAD_WLAN_WPA_SUPPLICANT);
    yos_net_initWLAN();
    sleep(1);
#if 1
    yos_net_setInterface("wlan0");
    yos_net_scanWLAN();
    yos_net_getWLANCount(&count); //TODO something is wrong!
    for (int i = 0; i < count; ++i) {
        yos_net_getWLANResult(i, ssid, &signal, auth, encr);
        printf("%d : %s %d %s %s\n", i, ssid, signal, auth, encr);
    } 
#endif
    sleep(1);
    return 0;
}
#endif
