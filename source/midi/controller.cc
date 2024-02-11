#include <iostream>
#include <thread>
#include <atomic>
#include <cstring>
#include <string>
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

/*
void Controller::processMidiInput()
{
    PmEvent buffer[BUFFER_SIZE];

    while (true)
    {
        int numEvents = Pm_Read(_midiInStream, buffer, BUFFER_SIZE);

        if (numEvents < 0)
        {
            std::cerr << "Error (process input) " << Pm_GetErrorText((PmError)numEvents) << std::endl;
            break;
        }

        for (int i = 0; i < numEvents; ++i)
        {
            PmEvent event = buffer[i];

            int messageType = Pm_MessageStatus(event.message);
            int channel = Pm_MessageStatus(event.message);

            if (messageType == 0x90)
            { // Note On
                int note = Pm_MessageData1(event.message);
                int velocity = Pm_MessageData2(event.message);
                std::cout << "Note On - Channel: " << channel << ", Note: " << note << ", Velocity: " << velocity << std::endl;
            }
            else if (messageType == 0xB0)
            { // Control Change
                int controller = Pm_MessageData1(event.message);
                int value = Pm_MessageData2(event.message);
                std::cout << "Control Change - Channel: " << channel << ", Controller: " << controller << ", Value: " << value << std::endl;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
*/

// Process incoming MIDI events
void Controller::processMidiInput()
{
    while (true)
    {
        int numEvents = Pm_Read(_midiInStream, midiEvent.events + midiEvent.endIdx, BUFFER_SIZE - midiEvent.endIdx);
        if (numEvents < 0)
        {
            std::cerr << "Error reading MIDI input: " << Pm_GetErrorText((PmError) numEvents) << std::endl;
            break;
        }

        // Update endIdx atomically
        int newEndIdx = (midiEvent.endIdx + numEvents) % BUFFER_SIZE;
        midiEvent.endIdx.store(newEndIdx, std::memory_order_relaxed);
    }
}

// Function to process MIDI events from the circular buffer
void Controller::processMidiEvents()
{
    while (true)
    {
        // Load startIdx and endIdx atomically
        int startIdx = midiEvent.startIdx.load(std::memory_order_relaxed);
        int endIdx = midiEvent.endIdx.load(std::memory_order_relaxed);

        if (startIdx != endIdx)
        {
            // Process MIDI events in the circular buffer
            while (startIdx != endIdx)
            {
                PmEvent event = midiEvent.events[startIdx];
                startIdx = (startIdx + 1) % BUFFER_SIZE;

                // Process the MIDI event as needed
                // (assuming Behringer X Touch MIDI message format)

                int messageType = Pm_MessageStatus(event.message);

                int channel = Pm_MessageStatus(event.message);
                int controller = Pm_MessageData1(event.message);
                int value = Pm_MessageData2(event.message);
                std::cout << "> chn: " << channel << ", btn: " << controller << ", val: " << value << std::endl;
            }
        }

        // Update startIdx atomically
        midiEvent.startIdx.store(startIdx, std::memory_order_relaxed);
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

void Controller::setLight(int const &button, int const &value)
{
    PmEvent midiEvent;
    midiEvent.message = Pm_Message(0x90, button, value);
    midiEvent.timestamp = 0;
    Pm_Write(_midiOutStream, &midiEvent, 1);
}

void Controller::setLight(std::vector<int> const &buttons, int const &value)
{
    for (int i : buttons)
    {
        setLight(i, value);
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