#pragma once

#include <iostream>
#include <cstring>
#include <string>
#include "portmidi.h"

class Controler
{
public:
    Controler(std::string name, PmDeviceID in, PmDeviceID out);
    ~Controler();

    void setLight(int const &button, int const &value);
private:
    std::string _name;
    PmDeviceID _deviceIn;
    PmDeviceID _deviceOut;
    PmStream *_midiInStream;
    PmStream *_midiOutStream;
};

bool findController(PmDeviceID &in, PmDeviceID &out, std::string const &name);
void openAndSendMidiMessage(PmDeviceID &deviceOut);