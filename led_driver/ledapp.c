#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char *argv[]){
    int fd,ret;
    unsigned char buf[2];


    if(4 != argc){
        fprintf(stderr,"Usage:\n"
                "\t./ledapp /dev/gpioled 1   1   @open the led          @first led\n"
                "\t./ledapp /dev/gpioled 0   2   @close the led          @second led\n"
         );
        return -1;
    }
    fd = open(argv[1],O_RDWR);
    if(0 > fd){
        fprintf(stderr,"file %s open failed!\r\n",argv[1]);
        return -1;
    }

    buf[0] = atoi(argv[2]);
    buf[1] = atoi(argv[3]);
    fprintf(stdout,"write %d%d to kernel!\r\n",buf[0],buf[1]);
    ret = write(fd,buf,sizeof(buf));

    if(0 > ret){
        fprintf(stderr,"led contro failed!\n");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}