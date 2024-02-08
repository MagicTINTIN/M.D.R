#include <iostream>
#include <cstring>
#include <string>
#include <algorithm>
#include <iomanip>
#include <unistd.h>
#include "portmidi.h"
#include "controler.hh"
#include "communication.hh"
#include "xtouch.hh"

void Controler::animChaser(int const &stepTime, bool const &reverse)
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

void Controler::animFilledChaser(int const &stepTime, bool const &reverse)
{
    if (reverse)
    {
        for (size_t i = XTOUCH_NB_OF_BUTTONS - 1; i >= 0; i--)
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

void Controler::animColumns(int const &stepTime, bool const &reverse)
{
    if (reverse)
        for (size_t i = XTOUCH_COLUMNS.size() - 1; i >= 0; i--)
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

void Controler::animFilledColumns(int const &stepTime, bool const &reverse)
{
    if (reverse)
    {
        for (size_t i = XTOUCH_COLUMNS.size() - 1; i >= 0; i--)
        {
            setLight(XTOUCH_COLUMNS[i], XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
        }
        for (size_t i = XTOUCH_COLUMNS.size() - 1; i >= 0; i--)
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

void Controler::animRows(int const &stepTime, bool const &reverse)
{
    if (reverse)
        for (size_t i = XTOUCH_ROWS.size() - 1; i >= 0; i--)
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

void Controler::animFilledRows(int const &stepTime, bool const &reverse)
{
    if (reverse)
    {
        for (size_t i = XTOUCH_ROWS.size() - 1; i >= 0; i--)
        {
            setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
        }
        for (size_t i = XTOUCH_ROWS.size() - 1; i >= 0; i--)
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

void Controler::animUpperLeftCross(int const &stepTime, bool const &reverse)
{
    if (reverse)
        for (size_t i = std::max(XTOUCH_ROWS.size(), XTOUCH_COLUMNS.size()) - 1; i >= 0; i--)
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

void Controler::animFilledUpperLeftCross(int const &stepTime, bool const &reverse)
{
    if (reverse) {
        for (size_t i = std::max(XTOUCH_ROWS.size(), XTOUCH_COLUMNS.size()) - 1; i >= 0; i--)
        {
            if (i < XTOUCH_ROWS.size())
                setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_ON);
            if (i < XTOUCH_COLUMNS.size())
                setLight(XTOUCH_COLUMNS[i], XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
        }
        for (size_t i = std::max(XTOUCH_ROWS.size(), XTOUCH_COLUMNS.size()) - 1; i >= 0; i--)
        {
            if (i < XTOUCH_ROWS.size())
                setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_OFF);
            if (i < XTOUCH_COLUMNS.size())
                setLight(XTOUCH_COLUMNS[i], XTOUCH_STATUS_OFF);
            usleep(1000 * stepTime);
        }
    }
    else {
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

void Controler::animUpperRightCross(int const &stepTime, bool const &reverse)
{
    const int MAX_ROWS_COLS = std::max(XTOUCH_ROWS.size(), XTOUCH_COLUMNS.size());
    if (reverse)
        for (size_t i = MAX_ROWS_COLS - 1; i >= 0; i--)
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

void Controler::animFilledUpperRightCross(int const &stepTime, bool const &reverse)
{
    const int MAX_ROWS_COLS = std::max(XTOUCH_ROWS.size(), XTOUCH_COLUMNS.size());
    if (reverse) {
        for (size_t i = MAX_ROWS_COLS - 1; i >= 0; i--)
        {
            if (i < XTOUCH_ROWS.size())
                setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_ON);
            if (i < XTOUCH_COLUMNS.size())
                setLight(XTOUCH_COLUMNS[MAX_ROWS_COLS - 1 - i], XTOUCH_STATUS_ON);
            usleep(1000 * stepTime);
        }
        for (size_t i = MAX_ROWS_COLS - 1; i >= 0; i--)
        {
            if (i < XTOUCH_ROWS.size())
                setLight(XTOUCH_ROWS[i], XTOUCH_STATUS_OFF);
            if (i < XTOUCH_COLUMNS.size())
                setLight(XTOUCH_COLUMNS[MAX_ROWS_COLS - 1 - i], XTOUCH_STATUS_OFF);
            usleep(1000 * stepTime);
        }
    }
    else {
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

void Controler::animColors(int const &stepTime)
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