#include <iostream>
#include <cstring>
#include <string>
#include "portmidi.h"

PmError initializePortMidi()
{
    PmError error = Pm_Initialize();
    if (error != pmNoError)
    {
        std::cerr << "Error initializing PortMidi: " << Pm_GetErrorText(error) << std::endl;
    }
    return error;
}

void displayDeviceInfo(PmDeviceInfo d)
{
    std::cout << d.name << " > I/O: " << d.input << "/" << d.output << " | opened: " << d.opened << " virtual:" << d.is_virtual;
}

void getAllDeviceInfo()
{
    int devicesCount = Pm_CountDevices();
    std::cout << "Total number of devices: " << devicesCount << std::endl
              << "Default device: " << Pm_GetDefaultInputDeviceID() << std::endl;
    for (PmDeviceID i = 0; i < devicesCount; i++)
    {
        PmDeviceInfo device = *Pm_GetDeviceInfo(i);
        std::cout << i << " - ";
        displayDeviceInfo(device);
        std::cout << std::endl;
    }
}

bool findController(PmDeviceID &in, PmDeviceID &out)
{
    int found = 1;
    int devicesCount = Pm_CountDevices();
    std::cout << "Searching for X-Touch device...\n";
    for (PmDeviceID i = 0; i < devicesCount; i++)
    {
        PmDeviceInfo device = *Pm_GetDeviceInfo(i);
        if (std::strcmp(device.name, "X-Touch X-TOUCH_INT")==0)  {
            if (device.input == 1) {
                std::cout << "Input found in " << i << std::endl;
                in = i;
                found*=2;
            }
            if (device.output == 1) {
                std::cout << "Output found in " << i << std::endl;
                out = i;
                found*=3;
            }
        }
    }
    return found % 6 == 0;
}

void openAndSendMidiMessage(PmDeviceID &deviceOut)
{
    // open the midi output device
    PmStream *midiStream;
    PmError error = Pm_OpenOutput(&midiStream, deviceOut, nullptr, 1, nullptr, nullptr, 0);

    if (error != pmNoError)
    {
        std::cerr << "Error opening MIDI output: " << Pm_GetErrorText(error) << std::endl;
        return;
    }

    // MIDI message to light up the second button
    PmEvent midiEvent;
    midiEvent.message = Pm_Message(0x90, 0x01, 0x7F); // MIDI Note On, channel 1, note number 1, velocity 127
    midiEvent.timestamp = 0;                          // send immediately

    // send the MIDI message
    Pm_Write(midiStream, &midiEvent, 1);

    // close the MIDI output device
    Pm_Close(midiStream);
}

void terminatePortMidi()
{
    Pm_Terminate();
}

int main()
{
    std::cout << "Starting mdr\n";

    if (initializePortMidi() != pmNoError)
    {
        return 1;
    }
    PmDeviceID dInputId = Pm_GetDefaultInputDeviceID();
    PmDeviceID dOutputId = Pm_GetDefaultOutputDeviceID();
    getAllDeviceInfo();

    if (!findController(dInputId, dOutputId))
    {
        std::cout << "Could not find X-Touch device among MIDI controlers\n";
        return 2;
    }
    openAndSendMidiMessage(dOutputId);

    terminatePortMidi();
    std::cout << "Ending mdr\n";
    return 0;
}