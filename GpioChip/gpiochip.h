#ifndef GPIOCHIP_H
#define GPIOCHIP_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>


class GpioChip
{
public:
    GpioChip();
    int m_fdExport, m_fdVal, m_fdDir, m_ret;
    void initGPIOChip(const char *dev, const char *devValue,const char *devDirection);
    int setState(const char *state);
    int GPIOWrite(const char *buf);
    int GPIORead(char *buf, const char *devValue);

};

#endif // GPIOCHIP_H
