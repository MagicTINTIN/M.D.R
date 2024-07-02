#pragma once

#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <map>
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

    void startThreads();

    // lights
    void setLight(int const &button, int const &value);
    void setLight(std::vector<int> const &buttons, int const &value);
    void setLight(std::vector<int> const &buttons, std::vector<int> const &values);

    void setAllLights(int const &status);
    void allLightsRed(int const &status, bool const &withLCDs = false);
    void allLightsBlue(int const &status, bool const &withLCDs = false);
    void allLightsGreen(int const &status, bool const &withLCDs = false);
    void allLightsYellow(int const &status, bool const &withLCDs = false);

    // sound peaks

    void setSoundPeak(int const &channel, int const &value);
    void setSoundPeak(std::vector<int> const &channels, int const &value);
    void setSoundPeak(std::vector<int> const &channels, std::vector<int> const &values);

    // faders
    void setFader(int const &fader, int const &value);
    void setFader(std::vector<int> const &faders, int const &value);
    void setFader(std::vector<int> const &faders, std::vector<int> const &values);

    // lcd
    void setLCDColor(int const &lcd, unsigned char const &color);
    void setLCDColor(std::vector<int> const &lcds, unsigned char const &color);
    void setLCDColor(std::vector<int> const &lcds, std::vector<unsigned char> const &colors);

    void setLCDText(int const &lcd, int const &line, std::string const &text, int const &alignment = 0);
    void setLCDText(std::vector<int> const &lcds, std::vector<int> const &lines, std::string const &text, int const &alignment = 0);
    void setLCDText(std::vector<int> const &lcds, std::vector<int> const &lines, std::vector<std::string> const &texts, int const &alignment = 0);
    
    void setLCDFullLineText(int const &line, std::string const &text, int const &alignment = 0);

    // rings
    void setRing(int const &ring, int const &mode, int const &value);
    void setRing(std::vector<int> const &rings, int const &mode, int const &value);
    void setRing(std::vector<int> const &rings, std::vector<int> const &modes, std::vector<int> const &values);

    // segments
    void setSegments(int const &segment, int const &value);
    void setSegments(std::vector<int> const &segments, int const &value);
    void setSegments(std::vector<int> const &segments, std::vector<int> const &values);

    void setSegmentsChar(int const &segment, char const &value, bool const &point = false);
    void setSegmentsChar(std::vector<int> const &segments, char const &value, bool const &point = false);
    void setSegmentsChar(std::vector<int> const &segments, std::vector<char> const &values);

    void setAssignmentSegment(char const &value1, char const &value2);
    void setAssignmentSegment(std::string const &value);
    void setHoursSegment(char const &value1, char const &value2, char const &value3);
    void setHoursSegment(std::string const &value);
    void setMinutesSegment(char const &value1, char const &value2);
    void setMinutesSegment(std::string const &value);
    void setSecondsSegment(char const &value1, char const &value2);
    void setSecondsSegment(std::string const &value);
    void setFramesSegment(char const &value1, char const &value2, char const &value3);
    void setFramesSegment(std::string const &value);

    // animations
    void Controller::animVector(std::vector<int> &vec, int const &stepTime, bool const &reverse);
    void Controller::animFilledVector(std::vector<int> &vec, int const &stepTime, bool const &reverse);
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
    void animColors(int const &stepTime, bool const &withLCDs = false);

    void animFaders(int const &stepTime, bool const &reverse = false);
    void animFilledFaders(int const &stepTime, bool const &reverse = false);

    void animLCDRainbow(int const &stepTime, int const &numberOfLoops, bool const &reverse = false);

    // more functions
    void displayCurrentDateTime();
    void dateTimeThread();

    // to be confirmed
    void toggleBacklight(bool const &on);
    void recalibrateFaders();
    void reset();

    // other
    void manual(int const &ch, int const &bt, int const &val);
    void analyser(int const &chStart, int const &chEnd, int const &btStart, int const &btEnd, int const &valStart, int const &valEnd, int const &stepTime);
    void advancedAnalyser(std::vector<unsigned char> const &values, int const &addHeader = 1);

private:
    std::string _name;
    PmDeviceID _deviceIn;
    PmDeviceID _deviceOut;
    PmStream *_midiInStream;
    PmStream *_midiOutStream;
    MidiEvent _midiEvent;
    unsigned char _xTouchColors[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int _temp = 0;
    int _toggletemp = 0;

    void updateLCDColorsMemory(int const &lcd, unsigned char const &color);
    void refreshLCDColors();
    void processMidiInput();
    void processMidiEvents();
    
    void buttonsHandler(int const &channel, int const &button, int const &value);
    void fadersHandler(int const &channel, int const &button, int const &value);
};

bool findController(PmDeviceID &in, PmDeviceID &out, std::string const &name);