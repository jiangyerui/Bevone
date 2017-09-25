#include "watchdog.h"

int Watchdog::fd = 0;
int Watchdog::init(char *device,int timeOut)
{
    Watchdog::fd = open(device,O_RDWR);
    if(fd < 0)
    {
        printf("**** Watchdog Open Failed ****\n");
        return fd;
    }
    else
    {
        printf("**** Watchdog Open Ok ****\n");
        ::ioctl(fd,WDIOC_SETTIMEOUT,&timeOut);
    }

    return 1;
}
void Watchdog::kellLive()
{
    ::ioctl(fd,WDIOC_KEEPALIVE,0);
}
