#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include "portmidi.h"
#include "../controller.hh"
#include "../xtouch.hh"

// faders

void Controller::setFader(int const &fader, int const &value)
{
    PmEvent portmidiEvent;
    portmidiEvent.message = Pm_Message(fader, 0, value);
    portmidiEvent.timestamp = 0;
    Pm_Write(_midiOutStream, &portmidiEvent, 1);
}

void Controller::setFader(std::vector<int> const &faders, int const &value)
{
    for (int i : faders)
    {
        setFader(i, value);
    }
}

void Controller::setFader(std::vector<int> const &faders, std::vector<int> const &values)
{
    if (faders.size() == values.size())
        for (size_t i = 0; i < faders.size(); i++)
        {
            setFader(faders[i], values[i]);
        }
    else
        std::cerr << "Error: faders & values do not have same sizes" << std::endl;
}