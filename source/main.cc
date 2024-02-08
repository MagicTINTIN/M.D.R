#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include "portmidi.h"
#include "midi/communication.hh"
#include "midi/controler.hh"
#include "midi/xtouch.hh"

int main()
{
    std::cout << "Starting mdr\n";
    std::string surfaceName = "X-Touch X-TOUCH_INT";

    if (initializePortMidi() != pmNoError)
    {
        return 1;
    }
    PmDeviceID dInputId = Pm_GetDefaultInputDeviceID();
    PmDeviceID dOutputId = Pm_GetDefaultOutputDeviceID();
    getAllDeviceInfo();

    if (!findController(dInputId, dOutputId, surfaceName))
    {
        std::cout << "Could not find X-Touch device among MIDI controlers\n";
        return 2;
    }
    Controler surface(surfaceName, dInputId, dOutputId);

    surface.setAllLights(XTOUCH_STATUS_OFF);
    surface.animChaser(20);
    surface.animFilledChaser(5, true);
    surface.animColors(200);
    surface.setAllLights(XTOUCH_STATUS_ON);
    usleep(400000);
    surface.setAllLights(XTOUCH_STATUS_OFF);
    usleep(20000);
    surface.setAllLights(XTOUCH_STATUS_ON);
    usleep(100000);
    surface.setAllLights(XTOUCH_STATUS_OFF);
    usleep(50000);
    surface.setAllLights(XTOUCH_STATUS_ON);
    usleep(25000);
    surface.setAllLights(XTOUCH_STATUS_OFF);
    usleep(12500);
    surface.setAllLights(XTOUCH_STATUS_ON);
    usleep(12500);
    surface.setAllLights(XTOUCH_STATUS_OFF);
    usleep(12500);
    surface.setAllLights(XTOUCH_STATUS_ON);
    usleep(12500);
    surface.setAllLights(XTOUCH_STATUS_OFF);
    usleep(12500);
    surface.animRows(50);
    surface.animColumns(50);
    surface.animFilledColumns(30, true);
    surface.animUpperRightCross(60);
    surface.animFilledUpperRightCross(30, true);
    surface.animUpperLeftCross(60);
    surface.animFilledRows(30);

    terminatePortMidi();
    std::cout << "Ending mdr\n";
    return 0;
}