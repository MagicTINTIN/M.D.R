#include <iostream>
#include <cstring>
#include <string>
#include <thread>
#include <unistd.h>
#include "portmidi.h"
#include "communication.hh"
#include "controller.hh"
#include "xtouch.hh"
#include "../global/utils.hh"

void Controller::displayCurrentDateTime()
{
    setHoursSegment(" " + getCurrentHour());
    setMinutesSegment(getCurrentMinute());
    setSecondsSegment(getCurrentSecond());
}

void Controller::dateTimeThread()
{
    while (true)
    {
        displayCurrentDateTime();
        usleep(1000000);
    }
    
}