#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include "portmidi.h"
#include "communication.hh"
#include "controller.hh"
#include "xtouch.hh"

void demoLights(Controller &surface);
void initAnim(Controller &surface);