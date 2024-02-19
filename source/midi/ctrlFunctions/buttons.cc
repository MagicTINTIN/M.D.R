#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include "portmidi.h"
#include "../controller.hh"
#include "../xtouch.hh"

// lights

void Controller::setLight(int const &button, int const &value)
{
    PmEvent portmidiEvent;
    portmidiEvent.message = Pm_Message(XTOUCH_BUTTONS_CH, button, value);
    portmidiEvent.timestamp = 0;
    Pm_Write(_midiOutStream, &portmidiEvent, 1);
}

void Controller::setLight(std::vector<int> const &buttons, int const &value)
{
    for (int i : buttons)
    {
        setLight(i, value);
    }
}

void Controller::setLight(std::vector<int> const &buttons, std::vector<int> const &values)
{
    if (buttons.size() == values.size())
        for (size_t i = 0; i < buttons.size(); i++)
        {
            setLight(buttons[i], values[i]);
        }
}

void Controller::setAllLights(int const &status)
{
    for (size_t i = 0; i < XTOUCH_NB_OF_BUTTONS; i++)
    {
        setLight(i, status);
    }
}

void Controller::allLightsRed(int const &status, bool const &withLCDs)
{
    for (int i : XTOUCH_BTN_RED)
    {
        setLight(i, status);
    }
    if (withLCDs)
        setLCDColor(XTOUCH_CHANNELS, XTOUCH_SCREEN_RED);
}
void Controller::allLightsBlue(int const &status, bool const &withLCDs)
{
    for (int i : XTOUCH_BTN_BLUE)
    {
        setLight(i, status);
    }
    if (withLCDs)
        setLCDColor(XTOUCH_CHANNELS, XTOUCH_SCREEN_BLUE);
}
void Controller::allLightsGreen(int const &status, bool const &withLCDs)
{
    for (int i : XTOUCH_BTN_GREEN)
    {
        setLight(i, status);
    }
    if (withLCDs)
        setLCDColor(XTOUCH_CHANNELS, XTOUCH_SCREEN_GREEN);
}
void Controller::allLightsYellow(int const &status, bool const &withLCDs)
{
    for (int i : XTOUCH_BTN_YELLOW)
    {
        setLight(i, status);
    }
    if (withLCDs)
        setLCDColor(XTOUCH_CHANNELS, XTOUCH_SCREEN_YELLOW);
}