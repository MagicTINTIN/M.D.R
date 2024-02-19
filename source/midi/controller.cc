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
    refreshLCDColors();
    setSegments(XTOUCH_REVERSED_SEGMENTS, 0);
    setRing(XTOUCH_RINGS, 0, 0);
    setFader(XTOUCH_FADERS, 0);
    setAllLights(XTOUCH_STATUS_OFF);
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
        std::unique_lock<std::mutex> lock(_midiEvent.mutex);
        for (int i = 0; i < numEvents; ++i)
        {
            _midiEvent.events[_midiEvent.endIdx] = buffer[i];
            _midiEvent.endIdx = (_midiEvent.endIdx + 1) % AUX_BUFFER_SIZE;
            if (_midiEvent.endIdx == _midiEvent.startIdx)
            {
                std::cerr << "Circular buffer overflow. Some events may be lost." << std::endl;
                _midiEvent.startIdx = (_midiEvent.startIdx + 1) % AUX_BUFFER_SIZE;
            }
        }
        lock.unlock();
        // notify the event processing thread
        _midiEvent.cv.notify_one();
    }
}

// Process MIDI events from the circular buffer
void Controller::processMidiEvents()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(_midiEvent.mutex);
        _midiEvent.cv.wait(lock, [&]
                           { return _midiEvent.startIdx != _midiEvent.endIdx; });

        // Process MIDI events in the circular buffer
        while (_midiEvent.startIdx != _midiEvent.endIdx)
        {
            PmEvent event = _midiEvent.events[_midiEvent.startIdx];
            _midiEvent.startIdx = (_midiEvent.startIdx + 1) % AUX_BUFFER_SIZE;

            // int messageType = Pm_MessageStatus(event.message);
            int channel = Pm_MessageStatus(event.message);
            int button = Pm_MessageData1(event.message);
            int value = Pm_MessageData2(event.message);
            std::cout << "> chn: " << channel << ", btn: " << button << ", val: " << value << std::endl;

            if (channel == XTOUCH_BUTTONS_CH && button < XTOUCH_NB_OF_BUTTONS)
            {
                buttonsHandler(channel, button, value);
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

// COMMANDS HANDLERS

void Controller::buttonsHandler(int const &channel, int const &button, int const &value)
{
    setLight(button, value);
    if (value != XTOUCH_STATUS_ON)
        return;

    if (button == XTOUCH_LEFT && _temp > 0)
    {
        _temp--;
        std::cout << "Temp value = " << _temp << std::endl;
        setSegments(0, _temp);
    }
    else if (button == XTOUCH_RIGHT && _temp < 128)
    {
        _temp++;
        std::cout << "Temp value = " << _temp << std::endl;
        setSegments(0, _temp);
    }
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

void Controller::allLightsRed(int const &status, bool const &withLCDs)
{
    for (int i : XTOUCH_BTN_RED)
    {
        setLight(i, status);
    }
    if (withLCDs)
        setLCDColor(XTOUCH_CHANNELS, XTOUCH_SCREEN_RED);
}
void Controller::allLightsBlue(int const &status, bool const &withLCDs)
{
    for (int i : XTOUCH_BTN_BLUE)
    {
        setLight(i, status);
    }
    if (withLCDs)
        setLCDColor(XTOUCH_CHANNELS, XTOUCH_SCREEN_BLUE);
}
void Controller::allLightsGreen(int const &status, bool const &withLCDs)
{
    for (int i : XTOUCH_BTN_GREEN)
    {
        setLight(i, status);
    }
    if (withLCDs)
        setLCDColor(XTOUCH_CHANNELS, XTOUCH_SCREEN_GREEN);
}
void Controller::allLightsYellow(int const &status, bool const &withLCDs)
{
    for (int i : XTOUCH_BTN_YELLOW)
    {
        setLight(i, status);
    }
    if (withLCDs)
        setLCDColor(XTOUCH_CHANNELS, XTOUCH_SCREEN_YELLOW);
}

//  sound peaks

void Controller::setSoundPeak(int const &channel, int const &value)
{
    if (channel < 1 || channel > 8)
    {
        std::cerr << "Sound peak channels go from 1 to 8" << std::endl;
        return;
    }
    if (value < 0 || value > 14)
    {
        std::cerr << "Sound peak values go from 0 to 14" << std::endl;
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

void Controller::updateLCDColorsMemory(int const &lcd, unsigned char const &color)
{
    if (lcd < 1 || lcd > 8)
    {
        std::cerr << "LCDs go from 1 to 8" << std::endl;
        return;
    }
    if (color < 0 || color > 7)
    {
        std::cerr << "Colors go from 0 to 7" << std::endl;
        return;
    }
    _xTouchColors[lcd - 1] = color;
}

void Controller::refreshLCDColors()
{
    unsigned char sysexMessage[] = {0xF0, 0x00, 0x00, 0x66, 0x14, 0x72, 0, 0, 0, 0, 0, 0, 0, 0, 0xF7};
    for (unsigned char i = 0; i < 8; i++)
    {
        sysexMessage[i + 6] = _xTouchColors[i];
    }

    PmError error = Pm_WriteSysEx(_midiOutStream, 0, sysexMessage);
    if (error != pmNoError)
    {
        std::cerr << "Error sending SysEx message: " << Pm_GetErrorText(error) << std::endl;
    }
}

void Controller::setLCDColor(int const &lcd, unsigned char const &color)
{
    updateLCDColorsMemory(lcd, color);
    refreshLCDColors();
}

void Controller::setLCDColor(std::vector<int> const &lcds, unsigned char const &color)
{
    for (unsigned char i = 0; i < lcds.size(); i++)
    {
        updateLCDColorsMemory(lcds[i], color);
    }
    refreshLCDColors();
}

void Controller::setLCDColor(std::vector<int> const &lcds, std::vector<unsigned char> const &colors)
{
    if (lcds.size() == colors.size())
    {
        for (unsigned char i = 0; i < lcds.size(); i++)
        {
            updateLCDColorsMemory(lcds[i], colors[i]);
        }
        refreshLCDColors();
    }
    else
        std::cerr << "Error: lcds & colors do not have same sizes" << std::endl;
}

void Controller::setRing(int const &ring, int const &mode, int const &value)
{
    // 176, 48-55, 0-127
    if (ring < 1 || ring > 8)
    {
        std::cerr << "Rings go from 1 to 8" << std::endl;
        return;
    }
    if (mode < 0 || mode > 127 || mode % 16 != 0)
    {
        std::cerr << "Modes are multiples of 16 (see XTOUCH_RING_MODE_* in xtouch.hh)" << std::endl;
        return;
    }
    if (mode == XTOUCH_RING_MODE_DISPERSION || XTOUCH_RING_MODE_DISPERSION_WEXT)
    {
        if (value < XTOUCH_RING_MIN_VALUE || value > XTOUCH_RING_MAX_VALUE_DISPERSION)
        {
            std::cerr << "Dispersion values go from 0 to 5" << std::endl;
            return;
        }
    }
    else
    {
        if (value < XTOUCH_RING_MIN_VALUE || value > XTOUCH_RING_MAX_VALUE_NORMAL)
        {
            std::cerr << "Ring values go from 0 to 10" << std::endl;
            return;
        }
    }

    PmEvent portmidiEvent;
    portmidiEvent.message = Pm_Message(XTOUCH_ENCODERS_CH, XTOUCH_RING1 - 1 + ring, mode + value);
    portmidiEvent.timestamp = 0;
    Pm_Write(_midiOutStream, &portmidiEvent, 1);
}

void Controller::setRing(std::vector<int> const &rings, int const &mode, int const &value)
{
    for (unsigned char i = 0; i < rings.size(); i++)
    {
        setRing(rings[i], mode, value);
    }
}

void Controller::setRing(std::vector<int> const &rings, std::vector<int> const &modes, std::vector<int> const &values)
{
    if (rings.size() == modes.size() && modes.size() == values.size())
    {
        for (unsigned char i = 0; i < rings.size(); i++)
        {
            setRing(rings[i], modes[i], values[i]);
        }
    }
    else
        std::cerr << "Error: rings, modes & values do not have same sizes" << std::endl;
}

void Controller::setSegments(int const &segment, int const &value)
{
    // 176, 64-75, 0-127
    if (segment < 0 || segment > 11)
    {
        std::cerr << "Segments go from 0 to 11" << std::endl;
        return;
    }
    PmEvent portmidiEvent;
    portmidiEvent.message = Pm_Message(XTOUCH_SEGMENTS_CH, XTOUCH_FIRST_SEGMENT + segment, value);
    portmidiEvent.timestamp = 0;
    Pm_Write(_midiOutStream, &portmidiEvent, 1);
}

void Controller::setSegments(std::vector<int> const &segments, int const &value)
{
    for (unsigned char i = 0; i < segments.size(); i++)
    {
        setSegments(segments[i], value);
    }
}

void Controller::setSegments(std::vector<int> const &segments, std::vector<int> const &values)
{
    if (segments.size() == values.size())
    {
        for (unsigned char i = 0; i < segments.size(); i++)
        {
            setSegments(segments[i], values[i]);
        }
    }
    else
        std::cerr << "Error: segments & values do not have same sizes" << std::endl;
}

void Controller::setSegmentsChar(int const &segment, char const &value, bool const &point)
{
    if (!XTOUCH_SEGMENTS_CHARS.count(value))
        std::cerr << "Error: the char '" << value << "' is not available" << std::endl;
    else
        setSegments(segment, XTOUCH_SEGMENTS_CHARS.at(value)+(point ? XTOUCH_SEGMENT_POINT_VARIANT : 0));
}

void Controller::setSegmentsChar(std::vector<int> const &segments, char const &value, bool const &point)
{
    if (!XTOUCH_SEGMENTS_CHARS.count(value))
        std::cerr << "Error: the char '" << value << "' is not available" << std::endl;
    else
        setSegments(segments, XTOUCH_SEGMENTS_CHARS.at(value)+(point ? XTOUCH_SEGMENT_POINT_VARIANT : 0));
}

void Controller::setSegmentsChar(std::vector<int> const &segments, std::vector<char> const &values)
{
    std::vector<int> chars;
    for (size_t i = 0; i < values.size(); i++)
    {
        if (!XTOUCH_SEGMENTS_CHARS.count(values[i]))
        std::cerr << "Error: the char '" << values[i] << "' is not available" << std::endl;
    else
        chars.emplace_back(XTOUCH_SEGMENTS_CHARS.at(values[i]));
    }
    setSegments(segments, chars);
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

void Controller::advancedAnalyser(std::vector<unsigned char> const &values, bool const &addHeader)
{
    std::vector<unsigned char> sysexVector = {};
    if (addHeader)
    {
        sysexVector.emplace_back(XTOUCH_COM_START);
        for (unsigned char v : XTOUCH_COM_HEADER)
            sysexVector.emplace_back(v);
    }
    for (unsigned char v : values)
        sysexVector.emplace_back(v);
    if (addHeader)
        sysexVector.emplace_back(XTOUCH_COM_END);
    PmError error = Pm_WriteSysEx(_midiOutStream, 0, &sysexVector[0]);
    if (error != pmNoError)
    {
        std::cerr << "Error sending SysEx message: " << Pm_GetErrorText(error) << std::endl;
    }
}