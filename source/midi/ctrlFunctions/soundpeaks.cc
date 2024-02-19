#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include "portmidi.h"
#include "../controller.hh"
#include "../xtouch.hh"

//  sound peaks

void Controller::setSoundPeak(int const &channel, int const &value)
{
    if (channel < 1 || channel > 8)
    {
        std::cerr << "Sound peak channels go from 1 to 8" << std::endl;
        return;
    }
    if (value < 0 || value > 14)
    {
        std::cerr << "Sound peak values go from 0 to 14" << std::endl;
        return;
    }
    int sentValue = (16 * (channel - 1)) + value;
    PmEvent portmidiEvent;
    portmidiEvent.message = Pm_Message(XTOUCH_SOUNDPEAKS_CH, sentValue, 0);
    portmidiEvent.timestamp = 0;
    Pm_Write(_midiOutStream, &portmidiEvent, 1);
}

void Controller::setSoundPeak(std::vector<int> const &channels, int const &value)
{
    for (size_t i = 0; i < channels.size(); i++)
    {
        setSoundPeak(channels[i], value);
    }
}

void Controller::setSoundPeak(std::vector<int> const &channels, std::vector<int> const &values)
{
    if (channels.size() == values.size())
        for (size_t i = 0; i < channels.size(); i++)
        {
            setSoundPeak(channels[i], values[i]);
        }
    else
        std::cerr << "Error: channels & values do not have same sizes" << std::endl;
}