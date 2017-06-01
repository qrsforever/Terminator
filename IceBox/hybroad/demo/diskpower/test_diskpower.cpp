#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "libzebra.h"

int main(int argc, char**argv) 
{
    int flags = 0, opt = '?', power = 0;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s [-h/-u] [0/1] \n", argv[0]);
        return -1;
    }

    yhw_board_init();

    while ((opt = getopt(argc, argv, "h:u:")) != -1) {
        switch (opt) {
        case 'h':
            flags = 1;
            power = atoi(optarg);
            break;
        case 'u':
            flags = 2;
            power = atoi(optarg);
            break;
        default: /* '?' */
            fprintf(stderr, "Usage: %s [-h/-u] [0/1] \n", argv[0]);
            return -1;
        }
    }

    if (flags == 1) {
        yhw_powerControl_setHDEnabled(0, power);
        yhw_powerControl_setHDEnabled(1, power);
        sleep(5);
        return 0;
    }

    if (flags == 2) {
        yhw_powerControl_setUSBEnabled(0, power);
        yhw_powerControl_setUSBEnabled(1, power);
        sleep(5);
        return 0;
    }

    fprintf(stderr, "Usage: %s [-h/-u] [0/1] \n", argv[0]);
    return -1;
}
