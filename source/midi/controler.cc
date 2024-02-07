#include <iostream>
#include <cstring>
#include <string>
#include "portmidi.h"
#include "controler.hh"
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
    midiEvent.message = Pm_Message(0x90, 0x00, 1); // MIDI Note On, channel 1, note number 1, velocity 127
    midiEvent.timestamp = 0;                          // send immediately

    // send the MIDI message
    Pm_Write(midiStream, &midiEvent, 1);

    // close the MIDI output device
    Pm_Close(midiStream);
}

Controler::Controler(std::string name, PmDeviceID in, PmDeviceID out) : _name(name), _deviceIn(in), _deviceOut(out) {
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

Controler::~Controler() {
    Pm_Close(_midiInStream);
    Pm_Close(_midiOutStream);
}

void Controler::setLight(int const &button, int const &value) {
    PmEvent midiEvent;
    midiEvent.message = Pm_Message(0x90, button, value);
    midiEvent.timestamp = 0;
    Pm_Write(_midiOutStream, &midiEvent, 1);
}