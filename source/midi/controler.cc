#include <iostream>
#include <cstring>
#include <string>
#include "portmidi.h"
#include "controler.hh"
#include "communication.hh"

bool findController(PmDeviceID &in, PmDeviceID &out, std::string const &name)
{
    int found = 1;
    int devicesCount = Pm_CountDevices();
    std::cout << "Searching for a " << name << " device...\n";
    for (PmDeviceID i = 0; i < devicesCount; i++)
    {
        PmDeviceInfo device = *Pm_GetDeviceInfo(i);
        if (std::strcmp(device.name, name.c_str())==0)  {
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