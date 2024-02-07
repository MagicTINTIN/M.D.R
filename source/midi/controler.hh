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

    // lights
    void setLight(int const &button, int const &value);
    void setAllLights(int const &status);
    void allLightsRed(int const &status);
    void allLightsBlue(int const &status);
    void allLightsGreen(int const &status);
    void allLightsYellow(int const &status);

    // animations
    void anim_chaser(int const &stepTime);
    void anim_columns(int const &stepTime);
    void anim_rows(int const &stepTime);
    void anim_colors(int const &stepTime);
private:
    std::string _name;
    PmDeviceID _deviceIn;
    PmDeviceID _deviceOut;
    PmStream *_midiInStream;
    PmStream *_midiOutStream;
};

bool findController(PmDeviceID &in, PmDeviceID &out, std::string const &name);