#include <iostream>
#include <thread>
#include <atomic>
#include <cstring>
#include <string>
#include <unistd.h>
#include "portmidi.h"
#include "controller.hh"
#include "communication.hh"
#include "xtouch.hh"

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
        std::unique_lock<std::mutex> lock(midiEvent.mutex);
        for (int i = 0; i < numEvents; ++i)
        {
            midiEvent.events[midiEvent.endIdx] = buffer[i];
            midiEvent.endIdx = (midiEvent.endIdx + 1) % AUX_BUFFER_SIZE;
            if (midiEvent.endIdx == midiEvent.startIdx)
            {
                std::cerr << "Circular buffer overflow. Some events may be lost." << std::endl;
                midiEvent.startIdx = (midiEvent.startIdx + 1) % AUX_BUFFER_SIZE;
            }
        }
        lock.unlock();
        // notify the event processing thread
        midiEvent.cv.notify_one();
    }
}

// Process MIDI events from the circular buffer
void Controller::processMidiEvents()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(midiEvent.mutex);
        midiEvent.cv.wait(lock, [&]
                          { return midiEvent.startIdx != midiEvent.endIdx; });

        // Process MIDI events in the circular buffer
        while (midiEvent.startIdx != midiEvent.endIdx)
        {
            PmEvent event = midiEvent.events[midiEvent.startIdx];
            midiEvent.startIdx = (midiEvent.startIdx + 1) % AUX_BUFFER_SIZE;

            // int messageType = Pm_MessageStatus(event.message);
            int channel = Pm_MessageStatus(event.message);
            int button = Pm_MessageData1(event.message);
            int value = Pm_MessageData2(event.message);
            std::cout << "> chn: " << channel << ", btn: " << button << ", val: " << value << std::endl;

            if (channel == XTOUCH_BUTTONS_CH && button < XTOUCH_NB_OF_BUTTONS)
            {
                setLight(button, value);
            }
        }
        lock.unlock();
    }
}

void Controller::startInputThreads()
{
    std::thread inputThread(&Controller::processMidiInput, this);
    std::thread eventThread(&Controller::processMidiEvents, this);

    inputThread.join();
    eventThread.join();
}

// OUTPUT FUNCTIONS

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

void Controller::allLightsRed(int const &status)
{
    for (int i : XTOUCH_RED)
    {
        setLight(i, status);
    }
}
void Controller::allLightsBlue(int const &status)
{
    for (int i : XTOUCH_BLUE)
    {
        setLight(i, status);
    }
}
void Controller::allLightsGreen(int const &status)
{
    for (int i : XTOUCH_GREEN)
    {
        setLight(i, status);
    }
}
void Controller::allLightsYellow(int const &status)
{
    for (int i : XTOUCH_YELLOW)
    {
        setLight(i, status);
    }
}

//  sound peaks

void Controller::setSoundPeak(int const &channel, int const &value)
{
    if (channel < 1 || channel > 8) {
        std::cerr << "Sound peak channels go from 1 to 8" << std::endl;
        return;
    }
    if (value < 0 || value > 8) {
        std::cerr << "Sound peak values go from 0 to 8" << std::endl;
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

void Controller::setLCD()
{
    // unsigned char msg[] = {0xF0, 0x00, 0x00, 0x66, 0x58, 0x20, 0x41, 0x43, 0x68, 0x20, 0x31, 0x00, 0x00, 0x00, 0x20, 0x20, 0x20, 0x20, 0x61, 0x42, 0x33, 0xF7};
    unsigned char sysexMessage[] = {
        0xF0, 0x00, 0x00, 0x66, 0x58, 0x20, 0x41, 0x43, 0x68, 0x20, 0x31, 0x00, 0x00, 0x00, 0x20, 0x20, 0x20, 0x20, 0x61, 0x42, 0x33, 0xF7};
    PmError error = Pm_WriteSysEx(_midiOutStream, 0, sysexMessage);
    if (error != pmNoError)
    {
        std::cerr << "Error sending SysEx message: " << Pm_GetErrorText(error) << std::endl;
    }
}

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