#include <iostream>
#include <thread>
#include <atomic>
#include <algorithm>
#include <cstring>
#include <string>
#include <unistd.h>
#include "portmidi.h"
#include "controller.hh"
#include "communication.hh"
#include "xtouch.hh"
#include "../global/utils.hh"

bool findController(PmDeviceID &in, PmDeviceID &out, std::string const &name)
{
    int found = 1;
    int devicesCount = Pm_CountDevices();
    std::cout << "Searching for a " << name << " device...\n";
    for (PmDeviceID i = 0; i < devicesCount; i++)
    {
        PmDeviceInfo device = *Pm_GetDeviceInfo(i);
        if (std::strcmp(device.name, name.c_str()) == 0)
        {
            if (device.input == 1)
            {
                std::cout << "Input found in " << i << std::endl;
                in = i;
                found *= 2;
            }
            if (device.output == 1)
            {
                std::cout << "Output found in " << i << std::endl;
                out = i;
                found *= 3;
            }
        }
    }
    return found % 6 == 0;
}

Controller::Controller(std::string name, PmDeviceID in, PmDeviceID out) : _name(name), _deviceIn(in), _deviceOut(out)
{
    PmError errorIn = Pm_OpenInput(&_midiInStream, _deviceIn, nullptr, 1, nullptr, nullptr);
    PmError errorOut = Pm_OpenOutput(&_midiOutStream, _deviceOut, nullptr, 1, nullptr, nullptr, 0);
    if (errorIn != pmNoError)
    {
        std::cerr << "Error opening " << _name << " MIDI input: " << Pm_GetErrorText(errorIn) << std::endl;
    }
    if (errorOut != pmNoError)
    {
        std::cerr << "Error opening " << _name << " MIDI output: " << Pm_GetErrorText(errorOut) << std::endl;
    }
    refreshLCDColors();
    setSegments(XTOUCH_REVERSED_SEGMENTS, 0);
    setRing(XTOUCH_RINGS, 0, 0);
    setFader(XTOUCH_FADERS, 0);
    setAllLights(XTOUCH_STATUS_OFF);
    setLCDFullLineText(0,"");
    setLCDFullLineText(1,"");
}

Controller::~Controller()
{
    Pm_Close(_midiInStream);
    Pm_Close(_midiOutStream);
}

// INPUT FUNCTIONS

// Process incoming MIDI events
void Controller::processMidiInput()
{
    while (true)
    {
        PmEvent buffer[BUFFER_SIZE];

        int numEvents = Pm_Read(_midiInStream, buffer, BUFFER_SIZE);
        if (numEvents < 0)
        {
            if (numEvents == pmBufferOverflow)
            {
                std::cerr << "Buffer overflow occurred. Flushing buffer." << std::endl;
                // Continue reading from the input
                continue;
            }
            else
            {
                std::cerr << "Error (input process) " << Pm_GetErrorText((PmError)numEvents) << std::endl;
                break;
            }
        }

        // store events to the circular buffer
        std::unique_lock<std::mutex> lock(_midiEvent.mutex);
        for (int i = 0; i < numEvents; ++i)
        {
            _midiEvent.events[_midiEvent.endIdx] = buffer[i];
            _midiEvent.endIdx = (_midiEvent.endIdx + 1) % AUX_BUFFER_SIZE;
            if (_midiEvent.endIdx == _midiEvent.startIdx)
            {
                std::cerr << "Circular buffer overflow. Some events may be lost." << std::endl;
                _midiEvent.startIdx = (_midiEvent.startIdx + 1) % AUX_BUFFER_SIZE;
            }
        }
        lock.unlock();
        // notify the event processing thread
        _midiEvent.cv.notify_one();
    }
}

// Process MIDI events from the circular buffer
void Controller::processMidiEvents()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(_midiEvent.mutex);
        _midiEvent.cv.wait(lock, [&]
                           { return _midiEvent.startIdx != _midiEvent.endIdx; });

        // Process MIDI events in the circular buffer
        while (_midiEvent.startIdx != _midiEvent.endIdx)
        {
            PmEvent event = _midiEvent.events[_midiEvent.startIdx];
            _midiEvent.startIdx = (_midiEvent.startIdx + 1) % AUX_BUFFER_SIZE;

            // int messageType = Pm_MessageStatus(event.message);
            int channel = Pm_MessageStatus(event.message);
            int button = Pm_MessageData1(event.message);
            int value = Pm_MessageData2(event.message);
            std::cout << "> chn: " << channel << ", btn: " << button << ", val: " << value << std::endl;

            if (channel == XTOUCH_BUTTONS_CH && button < XTOUCH_NB_OF_BUTTONS)
            {
                buttonsHandler(channel, button, value);
            }
            else if (std::find(XTOUCH_FADERS.begin(), XTOUCH_FADERS.end(), channel) != XTOUCH_FADERS.end())
            {
                fadersHandler(channel, button, value);
            }
        }
        lock.unlock();
    }
}

void Controller::startThreads()
{
    std::thread inputThread(&Controller::processMidiInput, this);
    std::thread eventThread(&Controller::processMidiEvents, this);
    std::thread dateTimeThread(&Controller::dateTimeThread, this);

    inputThread.join();
    eventThread.join();
    dateTimeThread.join();
}

// COMMANDS HANDLERS

void Controller::buttonsHandler(int const &channel, int const &button, int const &value)
{
    setLight(button, value);
    if (value != XTOUCH_STATUS_ON)
        return;

    if (button == XTOUCH_LEFT && _temp > 0)
    {
        _temp--;
        std::cout << "Temp value = " << _temp << std::endl;
        setSegments(0, _temp);
    }
    else if (button == XTOUCH_RIGHT && _temp < 128)
    {
        _temp++;
        std::cout << "Temp value = " << _temp << std::endl;
        setSegments(0, _temp);
    }
    else if (button == XTOUCH_REC)
        reset();
    else if (button == XTOUCH_SCRUB)
    {
        _toggletemp = (_toggletemp + 1) % 2;
        if (_toggletemp)
        {
            std::cout << "Toggle = " << _toggletemp << std::endl;
            toggleBacklight(true);
        }
        else
        {
            std::cout << "Toggle = " << _toggletemp << std::endl;
            toggleBacklight(false);
        }
    }
}

void Controller::fadersHandler(int const &channel, int const &button, int const &value)
{
    //std::vector<int>::iterator it = std::find(XTOUCH_FADERS.begin(), XTOUCH_FADERS.end(), channel);
    //int index = std::distance(XTOUCH_FADERS.begin(), it) + 1;
    if (std::find(XTOUCH_FADERS.begin(), XTOUCH_FADERS.end(), channel) == XTOUCH_FADERS.end()) return;
    setFader(channel, value);
    std::string strval = convertToPrintable(std::to_string(value*100/127), 3, 1);
    std::cout << "'"<< strval << "'\n";
    
    setFramesSegment(strval);
}

// OUTPUT FUNCTIONS

void Controller::manual(int const &ch, int const &bt, int const &val)
{
    PmEvent portmidiEvent;
    portmidiEvent.message = Pm_Message(ch, bt, val);
    portmidiEvent.timestamp = 0;
    Pm_Write(_midiOutStream, &portmidiEvent, 1);
}

void Controller::analyser(int const &chStart, int const &chEnd, int const &btStart, int const &btEnd, int const &valStart, int const &valEnd, int const &stepTime)
{
    for (size_t c = chStart; c <= chEnd; c++)
    {
        for (size_t b = btStart; b <= btEnd; b++)
        {
            for (size_t v = valStart; v <= valEnd; v++)
            {
                PmEvent portmidiEvent;
                portmidiEvent.message = Pm_Message(c, b, v);
                portmidiEvent.timestamp = 0;
                Pm_Write(_midiOutStream, &portmidiEvent, 1);
                usleep(1000 * stepTime);
            }
        }
    }
}

void Controller::advancedAnalyser(std::vector<unsigned char> const &values, int const &addHeader)
{
    std::vector<unsigned char> sysexVector = {};
    if (addHeader > 0)
    {
        sysexVector.emplace_back(XTOUCH_COM_START);
        if (addHeader == 2)
            for (unsigned char v : XTOUCH_COM_HEADER)
                sysexVector.emplace_back(v);
        if (addHeader == 3)
            for (unsigned char v : XTOUCH_COM_ARDOUR_HEADER)
                sysexVector.emplace_back(v);
    }
    for (unsigned char v : values)
        sysexVector.emplace_back(v);
    if (addHeader > 0)
        sysexVector.emplace_back(XTOUCH_COM_END);
    PmError error = Pm_WriteSysEx(_midiOutStream, 0, &sysexVector[0]);
    if (error != pmNoError)
    {
        std::cerr << "Error sending SysEx message: " << Pm_GetErrorText(error) << std::endl;
    }
}

// to be confirmed
void Controller::toggleBacklight(bool const &on)
{
    if (on)
    {
        std::cout << "Toggle backlight = " << on << std::endl;
        advancedAnalyser({0xa, 0x1});
    }
    else
    {
        std::cout << "Toggle backlight = " << on << std::endl;
        advancedAnalyser({0xa, 0x0});
    }
}

void Controller::recalibrateFaders()
{
    std::cout << "Recalibrating faders..." << std::endl;
    advancedAnalyser({0x09, 0x00});
}

void Controller::reset()
{
    std::cout << "Resetting controler..." << std::endl;
    advancedAnalyser({0x08, 0x00});
}
