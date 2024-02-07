#include <iostream>
#include <cstring>
#include <string>
#include "portmidi.h"

bool findController(PmDeviceID &in, PmDeviceID &out, std::string const &name);
void openAndSendMidiMessage(PmDeviceID &deviceOut);