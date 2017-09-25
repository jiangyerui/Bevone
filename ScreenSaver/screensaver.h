#ifndef SCREENSAVER_H
#define SCREENSAVER_H
#include <QWSScreenSaver>
#include "GlobalData/globaldata.h"
class ScreenSaver : public QWSScreenSaver
{
public:
    ScreenSaver();
    void restore();
    bool save(int level);
};

#endif // SCREENSAVER_H
