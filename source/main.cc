#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include "portmidi.h"
#include "midi/communication.hh"
#include "midi/controller.hh"
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
        std::cout << "Could not find X-Touch device among MIDI controllers\n";
        return 2;
    }
    Controller surface(surfaceName, dInputId, dOutputId);

    surface.setFader(XTOUCH_FADERS, 0);
    initAnim(surface);
    
    surface.analyser(208, 208, 0, 127, 0, 0, 2);

    surface.startInputThreads();

    // surface.setAllLights(XTOUCH_STATUS_ON);

    terminatePortMidi();
    std::cout << "Ending mdr\n";
    return 0;
}