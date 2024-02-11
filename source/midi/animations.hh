#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include "portmidi.h"
#include "communication.hh"
#include "controler.hh"
#include "xtouch.hh"

void demoLights(Controler &surface);
void initAnim(Controler &surface);