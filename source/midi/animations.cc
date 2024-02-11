#include <iostream>
#include <cstring>
#include <string>
#include <algorithm>
#include <iomanip>
#include <unistd.h>
#include <random>
#include "portmidi.h"
#include "controller.hh"
#include "communication.hh"
#include "xtouch.hh"
#include "animations.hh"

void Controller::animRandom(int const &steps, int const &stepTime, bool const &blinking)
{
    int type(0), btn(0);
    if (blinking)
        for (size_t i = 0; i < steps || steps < 0; i++)
        {
            type = std::rand() % 4;
            if (type == 2)
                type = XTOUCH_STATUS_OFF;
            else if (type == 3)
                type = XTOUCH_STATUS_ON;
            btn = std::rand() % XTOUCH_NB_OF_BUTTONS;
            setLight(btn, type);
            usleep(1000 * stepTime);
        }
    else
        for (size_t i = 0; i < steps || steps < 0; i++)
        {
            type = std::rand() % 2;
            if (type > 0)
                type = XTOUCH_STATUS_ON;
            btn = std::rand() % XTOUCH_NB_OF_BUTTONS;
            setLight(btn, type);
            usleep(1000 * stepTime);
        }
}

void Controller::animChaser(int const &stepTime, bool const &reverse)
{
    if (reverse)
        for (size_t i = XTOUCH_NB_OF_BUTTONS - 1; i >= 0; i--)
        {
            setLight(i, XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
            setLight(i, XTOUCH_STATUS_OFF);
        }
    else
        for (size_t i = 0; i < XTOUCH_NB_OF_BUTTONS; i++)
        {
            setLight(i, XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
            setLight(i, XTOUCH_STATUS_OFF);
        }
}

void Controller::animFilledChaser(int const &stepTime, bool const &reverse)
{
    if (reverse)
    {
        for (signed int i = XTOUCH_NB_OF_BUTTONS - 1; i >= 0; i--)
        {
            setLight(i, XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
        }
        for (signed int i = XTOUCH_NB_OF_BUTTONS - 1; i >= 0; i--)
        {
            setLight(i, XTOUCH_STATUS_OFF);
            usleep(1000 * stepTime);
        }
    }
    else
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
}

void Controller::animColumns(int const &stepTime, bool const &reverse)
{
    if (reverse)
        for (signed int i = XTOUCH_COLUMNS.size() - 1; i >= 0; i--)
        {
            setLight(XTOUCH_COLUMNS[i], XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
            setLight(XTOUCH_COLUMNS[i], XTOUCH_STATUS_OFF);
        }
    else
        for (size_t i = 0; i < XTOUCH_COLUMNS.size(); i++)
        {
            setLight(XTOUCH_COLUMNS[i], XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
            setLight(XTOUCH_COLUMNS[i], XTOUCH_STATUS_OFF);
        }
}

void Controller::animFilledColumns(int const &stepTime, bool const &reverse)
{
    if (reverse)
    {
        for (signed int i = XTOUCH_COLUMNS.size() - 1; i >= 0; i--)
        {
            setLight(XTOUCH_COLUMNS[i], XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
        }
        for (signed int i = XTOUCH_COLUMNS.size() - 1; i >= 0; i--)
        {
            setLight(XTOUCH_COLUMNS[i], XTOUCH_STATUS_OFF);
            usleep(1000 * stepTime);
        }
    }
    else
    {
        for (size_t i = 0; i < XTOUCH_COLUMNS.size(); i++)
        {
            setLight(XTOUCH_COLUMNS[i], XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
        }
        for (size_t i = 0; i < XTOUCH_COLUMNS.size(); i++)
        {
            setLight(XTOUCH_COLUMNS[i], XTOUCH_STATUS_OFF);
            usleep(1000 * stepTime);
        }
    }
}

void Controller::animRows(int const &stepTime, bool const &reverse)
{
    if (reverse)
        for (signed int i = XTOUCH_ROWS.size() - 1; i >= 0; i--)
        {
            setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
            setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_OFF);
        }
    else
        for (size_t i = 0; i < XTOUCH_ROWS.size(); i++)
        {
            setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
            setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_OFF);
        }
}

void Controller::animFilledRows(int const &stepTime, bool const &reverse)
{
    if (reverse)
    {
        for (signed int i = XTOUCH_ROWS.size() - 1; i >= 0; i--)
        {
            setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
        }
        for (signed int i = XTOUCH_ROWS.size() - 1; i >= 0; i--)
        {
            setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_OFF);
            usleep(1000 * stepTime);
        }
    }
    else
    {
        for (size_t i = 0; i < XTOUCH_ROWS.size(); i++)
        {
            setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
        }
        for (size_t i = 0; i < XTOUCH_ROWS.size(); i++)
        {
            setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_OFF);
            usleep(1000 * stepTime);
        }
    }
}

void Controller::animUpperLeftCross(int const &stepTime, bool const &reverse)
{
    if (reverse)
        for (signed int i = std::max(XTOUCH_ROWS.size(), XTOUCH_COLUMNS.size()) - 1; i >= 0; i--)
        {
            if (i < XTOUCH_ROWS.size())
                setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_ON);
            if (i < XTOUCH_COLUMNS.size())
                setLight(XTOUCH_COLUMNS[i], XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
            if (i < XTOUCH_ROWS.size())
                setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_OFF);
            if (i < XTOUCH_COLUMNS.size())
                setLight(XTOUCH_COLUMNS[i], XTOUCH_STATUS_OFF);
        }
    else
        for (size_t i = 0; i < std::max(XTOUCH_ROWS.size(), XTOUCH_COLUMNS.size()); i++)
        {
            if (i < XTOUCH_ROWS.size())
                setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_ON);
            if (i < XTOUCH_COLUMNS.size())
                setLight(XTOUCH_COLUMNS[i], XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
            if (i < XTOUCH_ROWS.size())
                setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_OFF);
            if (i < XTOUCH_COLUMNS.size())
                setLight(XTOUCH_COLUMNS[i], XTOUCH_STATUS_OFF);
        }
}

void Controller::animFilledUpperLeftCross(int const &stepTime, bool const &reverse)
{
    if (reverse)
    {
        for (signed int i = std::max(XTOUCH_ROWS.size(), XTOUCH_COLUMNS.size()) - 1; i >= 0; i--)
        {
            if (i < XTOUCH_ROWS.size())
                setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_ON);
            if (i < XTOUCH_COLUMNS.size())
                setLight(XTOUCH_COLUMNS[i], XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
        }
        for (signed int i = std::max(XTOUCH_ROWS.size(), XTOUCH_COLUMNS.size()) - 1; i >= 0; i--)
        {
            if (i < XTOUCH_ROWS.size())
                setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_OFF);
            if (i < XTOUCH_COLUMNS.size())
                setLight(XTOUCH_COLUMNS[i], XTOUCH_STATUS_OFF);
            usleep(1000 * stepTime);
        }
    }
    else
    {
        for (size_t i = 0; i < std::max(XTOUCH_ROWS.size(), XTOUCH_COLUMNS.size()); i++)
        {
            if (i < XTOUCH_ROWS.size())
                setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_ON);
            if (i < XTOUCH_COLUMNS.size())
                setLight(XTOUCH_COLUMNS[i], XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
        }
        for (size_t i = 0; i < std::max(XTOUCH_ROWS.size(), XTOUCH_COLUMNS.size()); i++)
        {
            if (i < XTOUCH_ROWS.size())
                setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_OFF);
            if (i < XTOUCH_COLUMNS.size())
                setLight(XTOUCH_COLUMNS[i], XTOUCH_STATUS_OFF);
            usleep(1000 * stepTime);
        }
    }
}

void Controller::animUpperRightCross(int const &stepTime, bool const &reverse)
{
    const int MAX_ROWS_COLS = std::max(XTOUCH_ROWS.size(), XTOUCH_COLUMNS.size());
    if (reverse)
        for (signed int i = MAX_ROWS_COLS - 1; i >= 0; i--)
        {
            if (i < XTOUCH_ROWS.size())
                setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_ON);
            if (i < XTOUCH_COLUMNS.size())
                setLight(XTOUCH_COLUMNS[MAX_ROWS_COLS - 1 - i], XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
            if (i < XTOUCH_ROWS.size())
                setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_OFF);
            if (i < XTOUCH_COLUMNS.size())
                setLight(XTOUCH_COLUMNS[MAX_ROWS_COLS - 1 - i], XTOUCH_STATUS_OFF);
        }
    else
        for (size_t i = 0; i < MAX_ROWS_COLS; i++)
        {
            if (i < XTOUCH_ROWS.size())
                setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_ON);
            if (i < XTOUCH_COLUMNS.size())
                setLight(XTOUCH_COLUMNS[MAX_ROWS_COLS - 1 - i], XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
            if (i < XTOUCH_ROWS.size())
                setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_OFF);
            if (i < XTOUCH_COLUMNS.size())
                setLight(XTOUCH_COLUMNS[MAX_ROWS_COLS - 1 - i], XTOUCH_STATUS_OFF);
        }
}

void Controller::animFilledUpperRightCross(int const &stepTime, bool const &reverse)
{
    const int MAX_ROWS_COLS = std::max(XTOUCH_ROWS.size(), XTOUCH_COLUMNS.size());
    if (reverse)
    {
        for (signed int i = MAX_ROWS_COLS - 1; i >= 0; i--)
        {
            if (i < XTOUCH_ROWS.size())
                setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_ON);
            if (i < XTOUCH_COLUMNS.size())
                setLight(XTOUCH_COLUMNS[MAX_ROWS_COLS - 1 - i], XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
        }
        for (signed int i = MAX_ROWS_COLS - 1; i >= 0; i--)
        {
            if (i < XTOUCH_ROWS.size())
                setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_OFF);
            if (i < XTOUCH_COLUMNS.size())
                setLight(XTOUCH_COLUMNS[MAX_ROWS_COLS - 1 - i], XTOUCH_STATUS_OFF);
            usleep(1000 * stepTime);
        }
    }
    else
    {
        for (size_t i = 0; i < std::max(XTOUCH_ROWS.size(), XTOUCH_COLUMNS.size()); i++)
        {
            if (i < XTOUCH_ROWS.size())
                setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_ON);
            if (i < XTOUCH_COLUMNS.size())
                setLight(XTOUCH_COLUMNS[MAX_ROWS_COLS - 1 - i], XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
        }
        for (size_t i = 0; i < std::max(XTOUCH_ROWS.size(), XTOUCH_COLUMNS.size()); i++)
        {
            if (i < XTOUCH_ROWS.size())
                setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_OFF);
            if (i < XTOUCH_COLUMNS.size())
                setLight(XTOUCH_COLUMNS[MAX_ROWS_COLS - 1 - i], XTOUCH_STATUS_OFF);
            usleep(1000 * stepTime);
        }
    }
}

void Controller::animColors(int const &stepTime)
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

void demoLights(Controller &surface)
{
    surface.setAllLights(XTOUCH_STATUS_OFF);
    surface.animChaser(20);
    surface.animFilledChaser(5, true);
    surface.animRandom(10, 30, true);
    surface.setAllLights(XTOUCH_STATUS_OFF);
    surface.animColors(200);
    surface.setAllLights(XTOUCH_STATUS_ON);
    usleep(400000);
    surface.setAllLights(XTOUCH_STATUS_OFF);
    usleep(20000);
    surface.setAllLights(XTOUCH_STATUS_ON);
    usleep(100000);
    surface.setAllLights(XTOUCH_STATUS_OFF);
    usleep(50000);
    surface.setAllLights(XTOUCH_STATUS_ON);
    usleep(25000);
    surface.setAllLights(XTOUCH_STATUS_OFF);
    usleep(12500);
    surface.setAllLights(XTOUCH_STATUS_ON);
    usleep(12500);
    surface.setAllLights(XTOUCH_STATUS_OFF);
    usleep(12500);
    surface.setAllLights(XTOUCH_STATUS_ON);
    usleep(12500);
    surface.setAllLights(XTOUCH_STATUS_OFF);
    usleep(12500);
    surface.animRows(50);
    surface.animColumns(50);
    surface.animFilledColumns(30, true);
    surface.animUpperRightCross(60);
    surface.animFilledUpperRightCross(30, true);
    surface.animUpperLeftCross(60);
    surface.animFilledRows(30);
    surface.animRandom(-1, 80, false);
}

void initAnim(Controller &surface)
{
    surface.setAllLights(XTOUCH_STATUS_OFF);
    surface.animUpperRightCross(50);
    surface.animFilledFaders(50);
    surface.animFilledColumns(25);
}

void Controller::animFaders(int const &stepTime, bool const &reverse)
{
    if (reverse)
        for (signed int i = XTOUCH_FADERS.size() - 1; i >= 0; i--)
        {
            setFader(XTOUCH_FADERS[i], XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
            setFader(XTOUCH_FADERS[i], XTOUCH_STATUS_OFF);
        }
    else
        for (size_t i = 0; i < XTOUCH_FADERS.size(); i++)
        {
            setFader(XTOUCH_FADERS[i], XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
            setFader(XTOUCH_FADERS[i], XTOUCH_STATUS_OFF);
        }
}

void Controller::animFilledFaders(int const &stepTime, bool const &reverse)
{
    if (reverse) {
        for (signed int i = XTOUCH_FADERS.size() - 1; i >= 0; i--)
        {
            setFader(XTOUCH_FADERS[i], XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
        }
        for (signed int i = XTOUCH_FADERS.size() - 1; i >= 0; i--)
        {
            setFader(XTOUCH_FADERS[i], XTOUCH_STATUS_OFF);
            usleep(1000 * stepTime);
        }
    }
    else {
        for (size_t i = 0; i < XTOUCH_FADERS.size(); i++)
        {
            setFader(XTOUCH_FADERS[i], XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
        }
        for (size_t i = 0; i < XTOUCH_FADERS.size(); i++)
        {
            setFader(XTOUCH_FADERS[i], XTOUCH_STATUS_OFF);
            usleep(1000 * stepTime);
        }
    }
}