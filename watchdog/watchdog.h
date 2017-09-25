#ifndef WATCH_DOG_H
#define WATCH_DOG_H
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <linux/watchdog.h>

class Watchdog
{
public:
    static int fd;
    static int init(char *device, int timeOut);
    static void kellLive();
};
#endif // WATCH_DOG_H
