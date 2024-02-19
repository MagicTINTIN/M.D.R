#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include "portmidi.h"
#include "../controller.hh"
#include "../xtouch.hh"

// rings


void Controller::setRing(int const &ring, int const &mode, int const &value)
{
    // 176, 48-55, 0-127
    if (ring < 1 || ring > 8)
    {
        std::cerr << "Rings go from 1 to 8" << std::endl;
        return;
    }
    if (mode < 0 || mode > 127 || mode % 16 != 0)
    {
        std::cerr << "Modes are multiples of 16 (see XTOUCH_RING_MODE_* in xtouch.hh)" << std::endl;
        return;
    }
    if (mode == XTOUCH_RING_MODE_DISPERSION || XTOUCH_RING_MODE_DISPERSION_WEXT)
    {
        if (value < XTOUCH_RING_MIN_VALUE || value > XTOUCH_RING_MAX_VALUE_DISPERSION)
        {
            std::cerr << "Dispersion values go from 0 to 5" << std::endl;
            return;
        }
    }
    else
    {
        if (value < XTOUCH_RING_MIN_VALUE || value > XTOUCH_RING_MAX_VALUE_NORMAL)
        {
            std::cerr << "Ring values go from 0 to 10" << std::endl;
            return;
        }
    }

    PmEvent portmidiEvent;
    portmidiEvent.message = Pm_Message(XTOUCH_ENCODERS_CH, XTOUCH_RING1 - 1 + ring, mode + value);
    portmidiEvent.timestamp = 0;
    Pm_Write(_midiOutStream, &portmidiEvent, 1);
}

void Controller::setRing(std::vector<int> const &rings, int const &mode, int const &value)
{
    for (unsigned char i = 0; i < rings.size(); i++)
    {
        setRing(rings[i], mode, value);
    }
}

void Controller::setRing(std::vector<int> const &rings, std::vector<int> const &modes, std::vector<int> const &values)
{
    if (rings.size() == modes.size() && modes.size() == values.size())
    {
        for (unsigned char i = 0; i < rings.size(); i++)
        {
            setRing(rings[i], modes[i], values[i]);
        }
    }
    else
        std::cerr << "Error: rings, modes & values do not have same sizes" << std::endl;
}