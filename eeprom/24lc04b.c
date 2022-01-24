#include "stdio.h"
#include "fcntl.h"
#include "linux/types.h"
#include "sys/ioctl.h"
#include "unistd.h"
#include <stdlib.h>


#define page_byte  16   //one page is 16 bytes
#define page_num   32   //total page

static void read_from_eeprom(int fd,int addr);
static void write_to_eeprom(int fd,int addr);
static void display_eeprom(int fd);
static void eeprom_reset(int fd);


int main(int argc,char *argv[]){
    int fd;
    int lseek_addr = 0;
    if(4 != argc){
        fprintf(stderr,"Usage:\n"
                       "\t./eeprom   (0 is write 1) (is read) (2 is show all) (3 is retset eeprom with 0xff) \n"
                       "\t rows cols(total is 32 rows and 16 cols))\n"
        );
        return -1;
    }

    fd = open("/sys/bus/i2c/devices/0-0050/eeprom",O_RDWR);

    if(0 > fd){
        fprintf(stderr,"open file error!");
        return 0;
    }
    lseek_addr = (atoi(argv[2]) - 1) * 16 + atoi(argv[3]) - 1;
    if(0 == atoi(argv[1])){
        write_to_eeprom(fd,lseek_addr);
    }

    if(1 == atoi(argv[1])){
        read_from_eeprom(fd,lseek_addr);
    }

    if(2 == atoi(argv[1])){
        display_eeprom(fd);
    }

    if(3 == atoi(argv[1])){
        eeprom_reset(fd);
    }
}

static void read_from_eeprom(int fd,int addr){
    char read_date[page_byte] = {0x0};     //once can write or read 16 bytes  one page
    int size = 0,i = 0;
    lseek(fd,addr,SEEK_SET);
    fprintf(stdout,"please input read num:");
    fscanf(stdin,"%d",&size);
    read(fd,read_date,size);
    for(i=0;i<size;i++){
        if(0 == i%16)
            printf("\n");
        printf("%02x  ",read_date[i]);
    }
    printf("\n");
}

static void write_to_eeprom(int fd,int addr){
    char write_data[page_byte] = {0x0};     //once can write or read 16bytes
    int size = 0,i = 0;
    if(-1 == lseek(fd,addr,SEEK_SET)){
        fprintf(stderr,"lseek error");
        exit(0);
    }
    fprintf(stdout,"please input write num:");
    fscanf(stdin,"%d",&size);
    fprintf(stdout,"please input:");
    for(i=0;i<size;i++)
        scanf("%02x",&write_data[i]);
    write(fd,write_data,size);
}

static void display_eeprom(int fd){
    int i =0,j = 0;
    char read_data[page_byte] ={};

    //printf for page
    for(i=0;i<32;i++){
        lseek(fd,i*page_byte,SEEK_SET);
        read(fd,read_data,page_byte);

        for(j=0;j<page_byte;j++){
            printf("%02x    ",read_data[j]);
            if(0 == (j+1)%16)
                printf("\n");
        }
        if(0 == (i+1)%16){
            printf("\n");
        }
    }
}

static void eeprom_reset(int fd){
    char write_data[page_byte] = {0xff};     //once can write or read 16bytes
    int i=0,j=0;
    fprintf(stdout,"Is resetting the eeprom witr 0xff!\n");
    lseek(fd,0,SEEK_SET);
    for(i=0;i<page_byte;i++){
        write_data[i] = 0xff;
    }
    for(i=0;i<page_num;i++){
        write(fd,write_data,page_byte);
    }
}