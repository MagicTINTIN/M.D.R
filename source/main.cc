#include <iostream>
#include "portmidi.h"

PmError initializePortMidi()
{
    PmError error = Pm_Initialize();
    if (error != pmNoError)
    {
        std::cerr << "Error initializing PortMidi: " << Pm_GetErrorText(error) << std::endl;
    }
    return error;
}

void displayDeviceInfo(PmDeviceInfo d) {
    std::cout << d.name << " > I/O: " << d.input << "/" << d.output << " | opened: " << d.opened << " virtual:" << d.is_virtual; 
}

void getAllDeviceInfo() {
    int devicesCount = Pm_CountDevices();
    std::cout << "Total number of devices: " << devicesCount << std::endl
    << "Default device: " << Pm_GetDefaultInputDeviceID() << std::endl;
    for (PmDeviceID i = 0; i < devicesCount; i++)
    {
        PmDeviceInfo device = *Pm_GetDeviceInfo(i);
        std::cout << "- ";
        displayDeviceInfo(device);
        std::cout << std::endl;
    }
    
}

void terminatePortMidi()
{
    Pm_Terminate();
}

int main()
{
    std::cout << "Starting mdr\n";
    
    if (initializePortMidi() != pmNoError)
    {
        return 1;
    }

    getAllDeviceInfo();

    // Terminate PortMidi
    terminatePortMidi();

    std::cout << "Ending mdr\n";
    return 0;
}