#include "screensaver.h"

ScreenSaver::ScreenSaver()
{

}

void ScreenSaver::restore()
{

}

bool ScreenSaver::save(int level)
{
   Q_UNUSED(level)
    g_login = true;
    return g_login;
}

