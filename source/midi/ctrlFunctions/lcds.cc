#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include "portmidi.h"
#include "../controller.hh"
#include "../xtouch.hh"

// lcds


void Controller::updateLCDColorsMemory(int const &lcd, unsigned char const &color)
{
    if (lcd < 1 || lcd > 8)
    {
        std::cerr << "LCDs go from 1 to 8" << std::endl;
        return;
    }
    if (color < 0 || color > 7)
    {
        std::cerr << "Colors go from 0 to 7" << std::endl;
        return;
    }
    _xTouchColors[lcd - 1] = color;
}

void Controller::refreshLCDColors()
{
    unsigned char sysexMessage[] = {0xF0, 0x00, 0x00, 0x66, 0x14, 0x72, 0, 0, 0, 0, 0, 0, 0, 0, 0xF7};
    for (unsigned char i = 0; i < 8; i++)
    {
        sysexMessage[i + 6] = _xTouchColors[i];
    }

    PmError error = Pm_WriteSysEx(_midiOutStream, 0, sysexMessage);
    if (error != pmNoError)
    {
        std::cerr << "Error sending SysEx message: " << Pm_GetErrorText(error) << std::endl;
    }
}

void Controller::setLCDColor(int const &lcd, unsigned char const &color)
{
    updateLCDColorsMemory(lcd, color);
    refreshLCDColors();
}

void Controller::setLCDColor(std::vector<int> const &lcds, unsigned char const &color)
{
    for (unsigned char i = 0; i < lcds.size(); i++)
    {
        updateLCDColorsMemory(lcds[i], color);
    }
    refreshLCDColors();
}

void Controller::setLCDColor(std::vector<int> const &lcds, std::vector<unsigned char> const &colors)
{
    if (lcds.size() == colors.size())
    {
        for (unsigned char i = 0; i < lcds.size(); i++)
        {
            updateLCDColorsMemory(lcds[i], colors[i]);
        }
        refreshLCDColors();
    }
    else
        std::cerr << "Error: lcds & colors do not have same sizes" << std::endl;
}