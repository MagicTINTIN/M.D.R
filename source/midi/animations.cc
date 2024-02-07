#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include "portmidi.h"
#include "controler.hh"
#include "communication.hh"
#include "xtouch.hh"

void Controler::anim_chaser(int const &stepTime)
{
    for (size_t i = 0; i < XTOUCH_NB_OF_BUTTONS; i++)
    {
        setLight(i, XTOUCH_STATUS_ON);
        usleep(1000 * stepTime);
    }

    for (size_t i = 0; i < XTOUCH_NB_OF_BUTTONS; i++)
    {
        setLight(i, XTOUCH_STATUS_OFF);
        usleep(1000 * stepTime);
    }
}

void Controler::anim_columns(int const &stepTime)
{
    for (std::vector<int> c : XTOUCH_COLUMNS)
    {
        for (int i : c)
        {
            setLight(i, XTOUCH_STATUS_ON);
        }
        usleep(1000 * stepTime);
        for (int i : c)
        {
            setLight(i, XTOUCH_STATUS_OFF);
        }
    }
}

void Controler::anim_rows(int const &stepTime)
{
    for (std::vector<int> c : XTOUCH_ROWS)
    {
        for (int i : c)
        {
            setLight(i, XTOUCH_STATUS_ON);
        }
        usleep(1000 * stepTime);
        for (int i : c)
        {
            setLight(i, XTOUCH_STATUS_OFF);
        }
    }
}

void Controler::anim_colors(int const &stepTime)
{
    allLightsRed(XTOUCH_STATUS_ON);
    usleep(1000 * stepTime);
    allLightsRed(XTOUCH_STATUS_OFF);
    allLightsYellow(XTOUCH_STATUS_ON);
    usleep(1000 * stepTime);
    allLightsYellow(XTOUCH_STATUS_OFF);
    allLightsGreen(XTOUCH_STATUS_ON);
    usleep(1000 * stepTime);
    allLightsGreen(XTOUCH_STATUS_OFF);
    allLightsBlue(XTOUCH_STATUS_ON);
    usleep(1000 * stepTime);
    allLightsBlue(XTOUCH_STATUS_OFF);
}