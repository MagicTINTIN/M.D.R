#include <iostream>
#include <cstring>
#include <string>
#include "portmidi.h"
#include "midi/communication.hh"
#include "midi/controler.hh"

int main()
{
    std::cout << "Starting mdr\n";

    if (initializePortMidi() != pmNoError)
    {
        return 1;
    }
    PmDeviceID dInputId = Pm_GetDefaultInputDeviceID();
    PmDeviceID dOutputId = Pm_GetDefaultOutputDeviceID();
    getAllDeviceInfo();

    if (!findController(dInputId, dOutputId, "X-Touch X-TOUCH_INT"))
    {
        std::cout << "Could not find X-Touch device among MIDI controlers\n";
        return 2;
    }
    openAndSendMidiMessage(dOutputId);

    terminatePortMidi();
    std::cout << "Ending mdr\n";
    return 0;
}