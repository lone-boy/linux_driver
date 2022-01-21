#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

static char userdata[] = "user data!";


int main(int argc,char *argv[])
{
    int fd,ret;
    char *filename;
    char readbuf[100],writebuf[100];

    if(3 != argc)
    {
        fprintf(stderr,"Error Usage!\r\n");
        return -1;
    }

    filename = argv[1];

    fd = open(filename,O_RDWR);
    if(0 > fd)
    {
        fprintf(stderr,"can't open file %s\r\n",filename);
        return -1;
    }

    if(1 == atoi(argv[2]))
    {
        ret = read(fd,readbuf,50);
        if(0 > ret)
        {
            fprintf(stderr,"read file %s failed\r\n",filename);
        }
        else
        {
            fprintf(stdout,"read data:%s\r\n",readbuf);
        }
    }

    else if(2 ==  atoi(argv[2]))
    {
        memcpy(writebuf,userdata,sizeof(userdata));
        ret = write(fd,writebuf,sizeof(userdata));
        if(0 > ret)
        {
            fprintf(stderr,"write file %s failed \r\n",filename);
            return -1;
        }
    }

    ret = close(fd);
    if(0 > ret)
    {
        fprintf(stderr,"can't close file %s!\r\n",filename);
        return -1;
    }

    return 0;
}