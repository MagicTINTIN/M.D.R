#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include "portmidi.h"
#include "midi/communication.hh"
#include "midi/controler.hh"
#include "midi/xtouch.hh"
#include "midi/animations.hh"

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

    demoLights(surface);

    terminatePortMidi();
    std::cout << "Ending mdr\n";
    return 0;
}