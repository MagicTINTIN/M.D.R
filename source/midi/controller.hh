#pragma once

#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "portmidi.h"
#define BUFFER_SIZE 512
#define AUX_BUFFER_SIZE 512

struct MidiEvent
{
    PmEvent events[AUX_BUFFER_SIZE];
    int startIdx;
    int endIdx;
    std::mutex mutex;
    std::condition_variable cv;

    MidiEvent() : startIdx(0), endIdx(0) {}
};

class Controller
{
public:
    Controller(std::string name, PmDeviceID in, PmDeviceID out);
    ~Controller();

    //----- in -----//

    void startInputThreads();

    //----- out -----//

    // lights
    void setLight(int const &button, int const &value);
    void setLight(std::vector<int> const &buttons, int const &value);
    void setLight(std::vector<int> const &buttons, std::vector<int> const &values);
    
    void setAllLights(int const &status);
    void allLightsRed(int const &status);
    void allLightsBlue(int const &status);
    void allLightsGreen(int const &status);
    void allLightsYellow(int const &status);

    // sound peaks

    void setSoundPeak(int const &channel, int const &value);
    void setSoundPeak(std::vector<int> const &channels, int const &value);
    void setSoundPeak(std::vector<int> const &channels, std::vector<int> const &values);

    // faders
    void setFader(int const &fader, int const &value);
    void setFader(std::vector<int> const &faders, int const &value);
    void setFader(std::vector<int> const &faders, std::vector<int> const &values);

    // lcd
    void setLCD();

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

    void animFaders(int const &stepTime, bool const &reverse = false);
    void animFilledFaders(int const &stepTime, bool const &reverse = false);

    // other
    void manual(int const &ch, int const &bt, int const &val);
    void analyser(int const &chStart, int const &chEnd, int const &btStart, int const &btEnd, int const &valStart, int const &valEnd, int const &stepTime);

private:
    std::string _name;
    PmDeviceID _deviceIn;
    PmDeviceID _deviceOut;
    PmStream *_midiInStream;
    PmStream *_midiOutStream;
    MidiEvent midiEvent;
    void processMidiInput();
    void processMidiEvents();
};

bool findController(PmDeviceID &in, PmDeviceID &out, std::string const &name);