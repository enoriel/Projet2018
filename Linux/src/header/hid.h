#ifndef _HID_H_
#define _HID_H_

/* Linux */
#include <linux/types.h>
#include <linux/input.h>
#include <linux/hidraw.h>

/*
 * Ugly hack to work around failing compilation on systems that don't
 * yet populate new version of hidraw.h to userspace.
 */
#ifndef HIDIOCSFEATURE
#warning Please have your distro update the userspace kernel headers
#define HIDIOCSFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x06, len)
#define HIDIOCGFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x07, len)
#endif
#define clear() printf("\033[H\033[J")
//#define DEBUG
/* Unix */
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* C */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

struct spi_settings
{ 
    union {
        unsigned char cmd;
        unsigned char answer;
        unsigned char size;
        unsigned char dc;
        unsigned char bitrate[4];
        unsigned char icsv[2];
        unsigned char acsv[2];
        unsigned char csdd[2];
        unsigned char ldatab[2];
        unsigned char delaydatab[2];
        unsigned char spibytes[2];
        unsigned char spimode;
        unsigned char useless[42];
    };
    unsigned char SPI_SETB[64];
};


struct spi_settings print_spi_settings(int fd);
unsigned char* send_data(int fd, unsigned char *buffer);
void print_mcp2210_info(int fd);
unsigned char send_spi(int fd, unsigned char *buffer, unsigned char *data, char n);
unsigned char* get_gpio(int fd, unsigned char *buffer);
unsigned char* set_gpio(int fd, unsigned char *buffer, int CE, int SS);
unsigned char* status_spi(int fd, unsigned char *buffer);
unsigned char* ConvertShortToByteArray(short int value, unsigned char *buffer, char n);
#endif