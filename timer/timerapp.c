#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "sys/ioctl.h"

#define CMD_LED_CLOSE   (_IO(0xef,0x1))
#define CMD_LED_OPEN    (_IO(0xef,0x2))
#define CMD_SET_PERIOD  (_IO(0Xef,0x3))

int main(int argc,char *argv[]){
    int fd,ret;
    unsigned int cmd;
    unsigned int period;

    if(2 != argc){
        fprintf(stderr,"Usage:\n"
                       "\t./ledapp /dev/gpioled   @open the led          @first led\n"
        );
        return -1;
    }
    fd = open(argv[1],O_RDWR);
    if(0 > fd){
        fprintf(stderr,"file %s open failed!\r\n",argv[1]);
        return -1;
    }

    for(;;){
        fprintf(stdout,"input cmd:");
        fscanf(stdin,"%d",&cmd);

        switch (cmd) {
            case 0:
                cmd = CMD_LED_CLOSE;
                break;
            case 1:
                cmd = CMD_LED_OPEN;
                break;
            case 2:
                cmd = CMD_SET_PERIOD;
                fprintf(stdout,"input timer period:");
                fscanf(stdin,"%d",&period);
                break;
            case 3:
                close(fd);
                return 0;
            default:
                break;
        }
        ioctl(fd,cmd,period);
    }
    return 0;
}