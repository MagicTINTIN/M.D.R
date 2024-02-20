#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include "portmidi.h"
#include "../controller.hh"
#include "../xtouch.hh"
#include "../../global/utils.hh"

// lcds

// colors

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
    unsigned char sysexMessage[] = {XTOUCH_COM_START, 0x00, 0x00, 0x66, 0x14, 0x72, 0, 0, 0, 0, 0, 0, 0, 0, XTOUCH_COM_END};
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

// text

void Controller::setLCDText(int const &lcd, int const &line, std::string const &text, int const &alignment)
{
    if (line != 0 && line != 1) {
        std::cerr << "Line can only be 0 or 1" << std::endl;
        return;
    }
    if (lcd < 1 || lcd > 8) {
        std::cerr << "Lcds gp from 1 to 8" << std::endl;
        return;
    }
    // offset (0 to 0x37 first line, 0x38 to 0x6f for second line)
    unsigned char offset = (XTOUCH_MAX_CHARS_PER_LCD * (lcd - 1)) + line * XTOUCH_MAX_CHARS_PER_FULL_LINE;
    unsigned char sysexMessage[] = {XTOUCH_COM_START, 0x00, 0x00, 0x66, 0x14, 0x12, offset, 0, 0, 0, 0, 0, 0, 0, XTOUCH_COM_END};
    
    std::string toPrint = convertToPrintable(text, XTOUCH_MAX_CHARS_PER_LCD, alignment);
    for (unsigned char i = 0; i < XTOUCH_MAX_CHARS_PER_LCD; i++)
    {
        sysexMessage[i + 7] = toPrint.at(i);
    }

    PmError error = Pm_WriteSysEx(_midiOutStream, 0, sysexMessage);
    if (error != pmNoError)
    {
        std::cerr << "Error sending SysEx message: " << Pm_GetErrorText(error) << std::endl;
    }
}

void Controller::setLCDText(std::vector<int> const &lcds, std::vector<int> const &lines, std::string const &text, int const &alignment)
{
    if (lcds.size() == lines.size())
        for (unsigned char i = 0; i < lcds.size(); i++)
        {
            setLCDText(lcds[i], lines[i], text, alignment);
        }
    else
        std::cerr << "Error: lcds & lines do not have same sizes" << std::endl;
}

void Controller::setLCDText(std::vector<int> const &lcds, std::vector<int> const &lines, std::vector<std::string> const &texts, int const &alignment)
{
    if (lcds.size() == lines.size() && lines.size() == texts.size())
        for (unsigned char i = 0; i < lcds.size(); i++)
        {
            setLCDText(lcds[i], lines[i], texts[i], alignment);
        }
    else
        std::cerr << "Error: lcds, lines & texts do not have same sizes" << std::endl;
}

// full line text

void Controller::setLCDFullLineText(int const &line, std::string const &text, int const &alignment)
{
    if (line != 0 && line != 1) {
        std::cerr << "Line can only be 0 or 1" << std::endl;
        return;
    }
    // offset (0 to 0x37 first line, 0x38 to 0x6f for second line)
    unsigned char offset = line * XTOUCH_MAX_CHARS_PER_FULL_LINE;
    unsigned char sysexMessage[] = {XTOUCH_COM_START, 0x00, 0x00, 0x66, 0x14, 0x12, offset, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, XTOUCH_COM_END};
    
    std::string toPrint = convertToPrintable(text, XTOUCH_MAX_CHARS_PER_FULL_LINE,alignment);
    for (unsigned char i = 0; i < XTOUCH_MAX_CHARS_PER_FULL_LINE; i++)
    {
        sysexMessage[i + 7] = toPrint.at(i);
    }

    PmError error = Pm_WriteSysEx(_midiOutStream, 0, sysexMessage);
    if (error != pmNoError)
    {
        std::cerr << "Error sending SysEx message: " << Pm_GetErrorText(error) << std::endl;
    }
}