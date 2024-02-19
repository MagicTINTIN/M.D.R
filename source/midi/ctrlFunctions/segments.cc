#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include "portmidi.h"
#include "../controller.hh"
#include "../xtouch.hh"

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
        setSegments(segment, XTOUCH_SEGMENTS_CHARS.at(value) + (point ? XTOUCH_SEGMENT_POINT_VARIANT : 0));
}

void Controller::setSegmentsChar(std::vector<int> const &segments, char const &value, bool const &point)
{
    if (!XTOUCH_SEGMENTS_CHARS.count(value))
        std::cerr << "Error: the char '" << value << "' is not available" << std::endl;
    else
        setSegments(segments, XTOUCH_SEGMENTS_CHARS.at(value) + (point ? XTOUCH_SEGMENT_POINT_VARIANT : 0));
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

void Controller::setAssignmentSegment(char const &value1, char const &value2)
{
    setSegmentsChar({11, 10}, {value1, value2});
}

void Controller::setAssignmentSegment(std::string const &value)
{
    if (value.size() >= 2)
        setSegmentsChar({11, 10}, {value.at(0), value.at(1)});
    else
        std::cerr << "Error: string must be of size 2 (or more)" << std::endl;
}

void Controller::setHoursSegment(char const &value1, char const &value2, char const &value3)
{
    setSegmentsChar({9, 8, 7}, {value1, value2, value3});
}

void Controller::setHoursSegment(std::string const &value)
{
    if (value.size() >= 3)
        setSegmentsChar({9,8,7}, {value.at(0), value.at(1), value.at(2)});
    else
        std::cerr << "Error: string must be of size 3 (or more)" << std::endl;
}

void Controller::setMinutesSegment(char const &value1, char const &value2)
{
    setSegmentsChar({6,5}, {value1, value2});
}

void Controller::setMinutesSegment(std::string const &value)
{
    if (value.size() >= 2)
        setSegmentsChar({6,5}, {value.at(0), value.at(1)});
    else
        std::cerr << "Error: string must be of size 2 (or more)" << std::endl;
}

void Controller::setSecondsSegment(char const &value1, char const &value2)
{
    setSegmentsChar({4,3}, {value1, value2});
}

void Controller::setSecondsSegment(std::string const &value)
{
    if (value.size() >= 2)
        setSegmentsChar({4,3}, {value.at(0), value.at(1)});
    else
        std::cerr << "Error: string must be of size 2 (or more)" << std::endl;
}

void Controller::setFramesSegment(char const &value1, char const &value2, char const &value3)
{
    setSegmentsChar({2,1,0}, {value1, value2, value3});
}

void Controller::setFramesSegment(std::string const &value)
{
    if (value.size() >= 3)
        setSegmentsChar({2,1,0}, {value.at(0), value.at(1), value.at(2)});
    else
        std::cerr << "Error: string must be of size 3 (or more)" << std::endl;
}
