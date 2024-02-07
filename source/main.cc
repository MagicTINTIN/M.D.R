#include <iostream>
#include <cstring>
#include <string>
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

    surface.setLight(XTOUCH_CYCLE, XTOUCH_STATUS_BLINK);
    surface.setLight(XTOUCH_SOLO, XTOUCH_STATUS_ON);
    surface.setLight(XTOUCH_REC2, XTOUCH_STATUS_OFF);

    terminatePortMidi();
    std::cout << "Ending mdr\n";
    return 0;
}