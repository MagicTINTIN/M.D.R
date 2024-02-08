#pragma once

#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include "portmidi.h"

class Controler
{
public:
    Controler(std::string name, PmDeviceID in, PmDeviceID out);
    ~Controler();

    // lights
    void setLight(int const &button, int const &value);
    void setLight(std::vector<int> const &buttons, int const &value);
    void setAllLights(int const &status);
    void allLightsRed(int const &status);
    void allLightsBlue(int const &status);
    void allLightsGreen(int const &status);
    void allLightsYellow(int const &status);

    // animations
    void animRandom(int const &steps, int const &stepTime, bool const &blinking = false);
    void animChaser(int const &stepTime, bool const &reverse = false);
    void animFilledChaser(int const &stepTime, bool const &reverse = false);
    void animColumns(int const &stepTime, bool const &reverse = false);
    void animFilledColumns(int const &stepTime, bool const &reverse = false);
    void animRows(int const &stepTime, bool const &reverse = false);
    void animFilledRows(int const &stepTime, bool const &reverse = false);
    void animUpperLeftCross(int const &stepTime, bool const &reverse = false);
    void animFilledUpperLeftCross(int const &stepTime, bool const &reverse = false);
    void animUpperRightCross(int const &stepTime, bool const &reverse = false);
    void animFilledUpperRightCross(int const &stepTime, bool const &reverse = false);
    void animColors(int const &stepTime);

private:
    std::string _name;
    PmDeviceID _deviceIn;
    PmDeviceID _deviceOut;
    PmStream *_midiInStream;
    PmStream *_midiOutStream;
};

bool findController(PmDeviceID &in, PmDeviceID &out, std::string const &name);