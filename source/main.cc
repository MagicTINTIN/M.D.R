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

    surface.setSegmentsChar({9,8,7,6,5,4,3}, {'b','o', 'n', 'j', 'o', 'u', 'r'});
    //surface.analyser(176, 176, 48, 55, 0, 255, 20);
    //initAnim(surface);
    surface.animLCDRainbow(100, 8, true);

    surface.startThreads();

    terminatePortMidi();
    std::cout << "Ending mdr\n";
    return 0;
}